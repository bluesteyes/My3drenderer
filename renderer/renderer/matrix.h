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
mat4_t mat4_make_perspective(float fov, float aspect, float znear, float zfar);
mat4_t mat4_mul_mat4(mat4_t a, mat4_t b);
vect4_t mat4_mul_vect4(mat4_t m, vect4_t v);
vect4_t mat4_mul_vect4_project(mat4_t mat_proj, vect4_t v);
mat4_t mat4_look_at(vect3_t eye, vect3_t target, vect3_t up);

mat4_t mat4_make_transpose(mat4_t m);
vect3_t mat4_mul_vect3_no_translation(mat4_t m, vect3_t v);
mat4_t mat4_make_inverse(mat4_t m);
vect3_t transform_NBT_to_world(vect3_t tangent, vect3_t bitangent, vect3_t normal, vect3_t tangent_normal);
vect3_t transform_TBN_to_world(vect3_t tangent, vect3_t bitangent, vect3_t normal, vect3_t tangent_normal);



#endif 


