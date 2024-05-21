#ifndef MESH_H
#define MESH_H
#include "vector.h"
#include "triangle.h"

#define N_CUBE_VERTICES 8 // 8 vertices per cube
#define N_CUBE_FACES (6*2) // 6 cube faces, 2 triangles per face

extern vect3_t cube_vertices[N_CUBE_VERTICES];
extern face_t cube_faces[N_CUBE_FACES];



//////////////////////////////////////////////////////////////////////////////////
// Define a struct for dynamic size of mesh for array of vertices and faces
//////////////////////////////////////////////////////////////////////////////////
typedef struct {
	 
	vect3_t* vertices;		//dynamic array of vertices
	face_t* faces;			//dynamic array of faces
	vect3_t rotation;       //rotation with x, y, and z values
	vect3_t scale;          //scale with x, y, and z values
	vect3_t translation;    //translation with x, y, and z values

} mesh_t;

extern mesh_t mesh;
void load_cube_mesh_data(void);
void load_obj_mesh_data(char* filename);
#endif // !MESH_H

