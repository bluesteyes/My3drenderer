#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "array.h"
#include "mesh.h"
#include "material.h"

#define MAX_NUM_MESHES 10
static mesh_t meshes[MAX_NUM_MESHES];
static int mesh_count = 0;

void load_mesh(char* obj_filename, char* png_filename, vect3_t scale, vect3_t translation, vect3_t rotation) {

	load_mesh_obj_data(&meshes[mesh_count], obj_filename);
	load_mesh_png_data(&meshes[mesh_count], png_filename);
	meshes[mesh_count].scale = scale;
	meshes[mesh_count].rotation = rotation;
	meshes[mesh_count].translation = translation;
	mesh_count++;
};

void load_mesh_obj_data(mesh_t* mesh, char* obj_filename){
	FILE* file;
	file = fopen(obj_filename, "r");
	char line[1024];
	
	tex2_t* texcoords = NULL;
	mesh->num_faces = 0;
	mesh->num_vertices = 0;
	mesh->num_model_normals = 0;
	while (fgets(line, 1024, file))
	{
		//model vertex infomation
		if (strncmp(line, "v ", 2) == 0){
			vect3_t vertex;
			sscanf(line, "v %f %f %f", &vertex.x, &vertex.y, &vertex.z);
			array_push(mesh->vertices, vertex);
			mesh->num_vertices++;
		}

		//model texture infomation
		if (strncmp(line, "vt ", 3) == 0){

			tex2_t texcoord;
			sscanf(line, "vt %f %f", &texcoord.u, &texcoord.v);
			array_push(texcoords, texcoord);
		}

		//model normal infomation
		if (strncmp(line, "vn ", 3) == 0) {
			vect3_t model_normal;
			sscanf(line, "vn %f %f %f", &model_normal.x, &model_normal.y, &model_normal.z);
			array_push(mesh->model_normals, model_normal);
			mesh->num_model_normals++;
		}

		//face infomation
		if (strncmp(line, "f ",2) == 0){
			int vertex_indices[3];
			int texture_indices[3];
			int normal_indices[3];
			
			sscanf(
				line, "f %d/%d/%d %d/%d/%d %d/%d/%d",
				&vertex_indices[0], &texture_indices[0], &normal_indices[0],
				&vertex_indices[1], &texture_indices[1], &normal_indices[1],
				&vertex_indices[2], &texture_indices[2], &normal_indices[2]
				);
			
			face_t face = {
				.a = vertex_indices[0] - 1,
				.b = vertex_indices[1] - 1,
				.c = vertex_indices[2] - 1,
				.n0 = normal_indices[0] - 1,
				.n1 = normal_indices[1] - 1,
				.n2 = normal_indices[2] - 2,
				.a_uv = texcoords[texture_indices[0] - 1],
				.b_uv = texcoords[texture_indices[1] - 1],
				.c_uv = texcoords[texture_indices[2] - 1],
				.color = get_material_color()
			};

			array_push(mesh->faces, face);
			mesh->num_faces++;

		}
	}

	//Allocate memory
	mesh->normals = (vect3_t*)calloc(mesh->num_faces, sizeof(vect3_t));

	/*for (size_t i = 0; i < mesh->num_model_normals; i++)
	{
		printf("Model Normal %d: (%f, %f, %f)\n", i, mesh->model_normals[i].x, mesh->model_normals[i].y, mesh->model_normals[i].z);
	}*/

	
}

void load_mesh_png_data(mesh_t* mesh, char* png_filename){
	upng_t* png_image = upng_new_from_file(png_filename);
	if (png_image != NULL){
		upng_decode(png_image);
		if (upng_get_error(png_image) == UPNG_EOK){
			mesh->textures = png_image;
		}
	}
}

int get_num_meshes(void){
	return mesh_count;
}

mesh_t* get_mesh(int mesh_index){
	return &meshes[mesh_index];
}

void calculate_vertex_normal(mesh_t* mesh) {
	//loop all triangle faces of mesh object
	for (int i = 0; i < mesh->num_faces; i++){
		face_t mesh_face = mesh->faces[i];

		//Get each face vertices->3

		vect4_t face_vertices[3];
		face_vertices[0] = vect4_from_vect3(mesh->vertices[mesh_face.a]);
		face_vertices[1] = vect4_from_vect3(mesh->vertices[mesh_face.b]);
		face_vertices[2] = vect4_from_vect3(mesh->vertices[mesh_face.c]);

		//Calculate the triangle face normal
		vect3_t face_normal = get_face_normal(face_vertices);

		//////////////////////////////////////////////////////////////////////////////////////////////////////
		// Accumulate triangle face normal to triangle vertex normal <-- a very cool method to calculate
		// vertex normal from face normal
		//////////////////////////////////////////////////////////////////////////////////////////////////////

		mesh->normals[mesh_face.a].x += face_normal.x;
		mesh->normals[mesh_face.a].y += face_normal.y;
		mesh->normals[mesh_face.a].z += face_normal.z;

		mesh->normals[mesh_face.b].x += face_normal.x;
		mesh->normals[mesh_face.b].y += face_normal.y;
		mesh->normals[mesh_face.b].z += face_normal.z;

		mesh->normals[mesh_face.c].x += face_normal.x;
		mesh->normals[mesh_face.c].y += face_normal.y;
		mesh->normals[mesh_face.c].z += face_normal.z;
		
	}
	//Loop all vertices and normalize vertex normal
	for (int i = 0; i < mesh->num_vertices; i++){
		vect3_normalize(&mesh->normals[i]);
	}
}

void free_meshes(void) {
	for (int i = 0; i < mesh_count; i++){

		free(meshes[i].normals);
		upng_free(meshes[i].textures);
		array_free(meshes[i].vertices);
		array_free(meshes[i].faces);
		array_free(meshes[i].model_normals);

	}
	
}


