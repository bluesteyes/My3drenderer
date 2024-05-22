#ifndef MATRIX_H
#define MATRIX_H
#include "vector.h"

typedef struct {
	
	float m[4][4];

} mat4_t;

mat4_t mat4_identity(void);
mat4_t mat4_make_scale(float sx, float sy, float sz);
mat4_t mat4_make_translation(float tx, float ty, float tz);
mat4_t mat4_make_rotation_x(float angle);
mat4_t mat4_make_rotation_y(float angle);
mat4_t mat4_make_rotation_z(float angle);
mat4_t mat4_mul_mat4(mat4_t a, mat4_t b);
vect4_t mat4_mul_vect4(mat4_t m, vect4_t v);

#endif 


