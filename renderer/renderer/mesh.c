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
}

void load_mesh_with_normalmap(char* obj_filename, char* png_filename, char* normalmap_filename, 
	char* glowmap_filename, char* metalmap_filename,
	vect3_t scale, vect3_t translation, vect3_t rotation) {

	load_mesh_obj_data(&meshes[mesh_count], obj_filename);
	load_mesh_png_data(&meshes[mesh_count], png_filename);
	load_mesh_normalmap_data(&meshes[mesh_count], normalmap_filename);
	load_mesh_glowmap_data(&meshes[mesh_count], glowmap_filename);
	load_mesh_roughmap_data(&meshes[mesh_count], metalmap_filename);
	meshes[mesh_count].scale = scale;
	meshes[mesh_count].rotation = rotation;
	meshes[mesh_count].translation = translation;
	mesh_count++;

}


void load_mesh_with_pbr(char* obj_filename, char* png_filename, char* normalmap_filename,
	char* glowmap_filename, char* roughmap_filename, char* metalmap_filename, char* aomap_filename,
	vect3_t scale, vect3_t translation, vect3_t rotation) {

	load_mesh_obj_data(&meshes[mesh_count], obj_filename);
	load_mesh_png_data(&meshes[mesh_count], png_filename);
	load_mesh_normalmap_data(&meshes[mesh_count], normalmap_filename);
	load_mesh_glowmap_data(&meshes[mesh_count], glowmap_filename);
	load_mesh_metalmap_data(&meshes[mesh_count], metalmap_filename);
	load_mesh_roughmap_data(&meshes[mesh_count], roughmap_filename);
	load_mesh_aomap_data(&meshes[mesh_count], aomap_filename);
	meshes[mesh_count].scale = scale;
	meshes[mesh_count].rotation = rotation;
	meshes[mesh_count].translation = translation;
	mesh_count++;

}


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
				.n2 = normal_indices[2] - 1,
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
	mesh->tangents = (vect3_t*)calloc(mesh->num_faces, sizeof(vect3_t));
	mesh->bitangents = (vect3_t*)calloc(mesh->num_faces, sizeof(vect3_t));

	/*for (size_t i = 0; i < mesh->num_vertices; i++)
	{
		printf("Model Vertices %d: (%f, %f, %f)\n", i, mesh->vertices[i].x, mesh->vertices[i].y, mesh->vertices[i].z);
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

void load_mesh_normalmap_data(mesh_t* mesh, char* normalmap_filename) {
	upng_t* normalmap_image = upng_new_from_file(normalmap_filename);
	if (normalmap_image != NULL){
		upng_decode(normalmap_image);
		if (upng_get_error(normalmap_image) == UPNG_EOK){
			mesh->normalmaps = normalmap_image;
		}
	}
}

void load_mesh_glowmap_data(mesh_t* mesh, char* glowmap_filename) {
	upng_t* glowmap_image = upng_new_from_file(glowmap_filename);
	if (glowmap_image != NULL){
		upng_decode(glowmap_image);
		if (upng_get_error(glowmap_image) == UPNG_EOK){
			mesh->glowmaps = glowmap_image;
		}
	}
}

void load_mesh_roughmap_data(mesh_t* mesh, char* roughmap_filename) {
	upng_t* roughmap_image = upng_new_from_file(roughmap_filename);
	if (roughmap_image != NULL) {
		upng_decode(roughmap_image);
		if (upng_get_error(roughmap_image) == UPNG_EOK) {
			mesh->roughmaps = roughmap_image;
		}
	}
}


void load_mesh_metalmap_data(mesh_t* mesh, char* metalmap_filename) {
	upng_t* metalmap_image = upng_new_from_file(metalmap_filename);
	if (metalmap_image != NULL) {
		upng_decode(metalmap_image);
		if (upng_get_error(metalmap_image) == UPNG_EOK) {
			mesh->metallic = metalmap_image;
		}
	}
}


void load_mesh_aomap_data(mesh_t* mesh, char* aomap_filename) {
	upng_t* aomap_image = upng_new_from_file(aomap_filename);
	if (aomap_image != NULL) {
		upng_decode(aomap_image);
		if (upng_get_error(aomap_image) == UPNG_EOK) {
			mesh->ao = aomap_image;
		}
	}
}




int get_num_meshes(void){
	return mesh_count;
}

mesh_t* get_mesh(int mesh_index){
	return &meshes[mesh_index];
}


//calculate each average vertex normal of a mesh
void calculate_vertex_normal(mesh_t* mesh) {

	for (int i = 0; i < mesh->num_vertices; i++) {
		mesh->normals[i] = vect3_new(0.0f, 0.0f, 0.0f);
	}

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

	/*for (size_t i = 0; i < mesh->num_vertices; ++i)
	{
		printf("calculated vertex Normal %d: (%f, %f, %f)\n", i, mesh->normals[i].x, mesh->normals[i].y, mesh->normals[i].z);
	}*/
}

/// Calculate each vertex tangents and bitangents for a mesh
void calculate_tangents_and_bitangents(mesh_t* mesh) {

	//Initialize tangents and bitangents to zero
	for (int i = 0; i < mesh->num_vertices; i++){
		mesh->tangents[i] = vect3_new(0.0f, 0.0f, 0.0f);
		mesh->bitangents[i] = vect3_new(0.0f, 0.0f, 0.0f);
	}

	///calculate tangents and bitangents for each triangle
	//loop all triangle faces
	for (int i = 0; i < mesh->num_faces; i++){
		face_t mesh_face = mesh->faces[i];

		//Get each face vertices
		vect3_t v0 = mesh->vertices[mesh_face.a];
		vect3_t v1 = mesh->vertices[mesh_face.b];
		vect3_t v2 = mesh->vertices[mesh_face.c];

		vect3_t edge1 = vect3_sub(v1, v0);
		vect3_t edge2 = vect3_sub(v2, v0);

		float delta_u1 = mesh_face.b_uv.u - mesh_face.a_uv.u;
		float delta_v1 = mesh_face.b_uv.v - mesh_face.a_uv.v;
		float delta_u2 = mesh_face.c_uv.u - mesh_face.a_uv.u;
		float delta_v2 = mesh_face.c_uv.v - mesh_face.a_uv.v;

		float f = 1.0f / (delta_u1 * delta_v2 - delta_u2 * delta_v1); 

		vect3_t tangent = {
			f * (delta_v2 * edge1.x - delta_v1 * edge2.x),
			f * (delta_v2 * edge1.y - delta_v1 * edge2.y),
			f * (delta_v2 * edge1.z - delta_v1 * edge2.z)
		};

		vect3_t bitangent = {
			f * (-delta_u2 * edge1.x + delta_u1 * edge2.x),
			f * (-delta_u2 * edge1.y + delta_u1 * edge2.y),
			f * (-delta_u2 * edge1.z + delta_u1 * edge2.z)
		};

		vect3_normalize(&tangent);
		vect3_normalize(&bitangent);


		//Accumulates tangents and bitangents for each vertex
		mesh->tangents[mesh_face.a].x += tangent.x;
		mesh->tangents[mesh_face.a].y += tangent.y;
		mesh->tangents[mesh_face.a].z += tangent.z;

		mesh->tangents[mesh_face.b].x += tangent.x;
		mesh->tangents[mesh_face.b].y += tangent.y;
		mesh->tangents[mesh_face.b].z += tangent.z;
		
		mesh->tangents[mesh_face.c].x += tangent.x;
		mesh->tangents[mesh_face.c].y += tangent.y;
		mesh->tangents[mesh_face.c].z += tangent.z;

		mesh->bitangents[mesh_face.a].x += bitangent.x;
		mesh->bitangents[mesh_face.a].y += bitangent.y;
		mesh->bitangents[mesh_face.a].z += bitangent.z;

		mesh->bitangents[mesh_face.b].x += bitangent.x;
		mesh->bitangents[mesh_face.b].y += bitangent.y;
		mesh->bitangents[mesh_face.b].z += bitangent.z;

		mesh->bitangents[mesh_face.c].x += bitangent.x;
		mesh->bitangents[mesh_face.c].y += bitangent.y;
		mesh->bitangents[mesh_face.c].z += bitangent.z;

	}

	// Orthogonalize and normalize tangents and bitangents
	for (int i = 0; i < mesh->num_vertices; i++) {


		mesh->bitangents[i] = vect3_cross(mesh->model_normals[i], mesh->tangents[i]);
		mesh->tangents[i] = vect3_cross(mesh->model_normals[i], mesh->bitangents[i]);

		//mesh->bitangents[i] = vect3_cross(mesh->tangents[i], mesh->model_normals[i]);
		//mesh->tangents[i] = vect3_cross(mesh->bitangents[i], mesh->model_normals[i]);

		//Orthogonalize tangent
	/*	float dot_NT = vect3_dot(mesh->normals[i], mesh->tangents[i]);
		mesh->tangents[i].x -= mesh->normals[i].x * dot_NT;
		mesh->tangents[i].y -= mesh->normals[i].y * dot_NT;
		mesh->tangents[i].z -= mesh->normals[i].z * dot_NT;*/

		//float dot_NT = vect3_dot(mesh->model_normals[i], mesh->tangents[i]);
		//mesh->tangents[i].x -= mesh->model_normals[i].x * dot_NT;
		//mesh->tangents[i].y -= mesh->model_normals[i].y * dot_NT;
		//mesh->tangents[i].z -= mesh->model_normals[i].z * dot_NT;

		
		vect3_normalize(&mesh->tangents[i]);

		//Recompute bitanget
		//mesh->bitangents[i] = vect3_cross(mesh->normals[i], mesh->tangents[i]);
		
		//mesh->bitangents[i] = vect3_cross(mesh->model_normals[i], mesh->tangents[i]);
		vect3_normalize(&mesh->bitangents[i]);
	}
//
//	for (size_t i = 0; i < mesh->num_vertices; i++)
//	{
//		printf("vertex tangents %d: (%f, %f, %f)\n", i, mesh->tangents[i].x, mesh->tangents[i].y, mesh->tangents[i].z);
//		printf("vertex bitangents %d: (%f, %f, %f)\n", i, mesh->bitangents[i].x, mesh->bitangents[i].y, mesh->bitangents[i].z);
//		printf("vertex normals %d: (%f, %f, %f)\n", i, mesh->model_normals[i].x, mesh->model_normals[i].y, mesh->model_normals[i].z);
//	}
 }


void free_meshes(void) {
	for (int i = 0; i < mesh_count; i++){

		free(meshes[i].normals);
		free(meshes[i].tangents);
		free(meshes[i].bitangents);

		if (meshes[i].textures != NULL){
			upng_free(meshes[i].textures);
		}

		if (meshes[i].normalmaps != NULL){
			upng_free(meshes[i].normalmaps);
		}

		if (meshes[i].glowmaps != NULL) {
			upng_free(meshes[i].glowmaps);
		}

		if (meshes[i].roughmaps != NULL) {
			upng_free(meshes[i].roughmaps);
		}

		if (meshes[i].metallic != NULL) {
			upng_free(meshes[i].metallic);
		}

		if (meshes[i].ao != NULL) {
			upng_free(meshes[i].ao);
		}

		array_free(meshes[i].vertices);
		array_free(meshes[i].faces);
		array_free(meshes[i].model_normals);

	}
}


