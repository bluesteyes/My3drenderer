#ifndef MESH_H
#define MESH_H
#include "vector.h"
#include "triangle.h"
#include "upng.h"


//////////////////////////////////////////////////////////////////////////////////
// Define a struct for dynamic size of mesh for array of vertices and faces
//////////////////////////////////////////////////////////////////////////////////
typedef struct {
	vect3_t* vertices;			//mesh dynamic array of vertices
	vect3_t* model_normals;		//mesh dynamic array of model normals
	face_t* faces;				//mesh dynamic array of faces
	vect3_t* normals;			//mesh dynamic array of calculated vertex normals from face normal
	vect3_t* tangents;
	vect3_t* bitangents;
	upng_t* textures;			//mesh dynamic array of textures
	upng_t* normalmaps;			//mesh dynamic array of normalmaps
	upng_t* glowmaps;			//mesh dynamic array of glowmaps
	vect3_t rotation;			//mesh rotation with x, y, and z values
	vect3_t scale;				//mesh scale with x, y, and z values
	vect3_t translation;		//mesh translation with x, y, and z values
	int num_vertices;
	int num_faces;
	int num_model_normals;
} mesh_t;


void load_mesh(char* obj_filename, char* png_filename, vect3_t scale,
	vect3_t translation, vect3_t rotation);

void load_mesh_with_normalmap(char* obj_filename, char* png_filename, char* normalmap_filename, char* glowmap_filename,
	vect3_t scale, vect3_t translation, vect3_t rotation);

void load_mesh_obj_data(mesh_t* mesh, char* obj_filename);
void load_mesh_png_data(mesh_t* mesh, char* png_filename);
void load_mesh_normalmap_data(mesh_t* mesh, char* normalmap_filename);
void load_mesh_glowmap_data(mesh_t* mesh, char* glowmap_filename);

int get_num_meshes(void);
mesh_t* get_mesh(int mesh_index);

void calculate_vertex_normal(mesh_t* mesh);
void calculate_tangents_and_bitangents(mesh_t* mesh);

void free_meshes(void);
#endif 

