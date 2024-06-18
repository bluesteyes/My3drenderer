#include <math.h>
#include "vector.h"


//////////////////////////////////////////////////////////////////////////////////
// Implementation of all vector 2d functions
//////////////////////////////////////////////////////////////////////////////////

vect2_t vect2_new(float x, float y){
	vect2_t result = {
		.x = x,
		.y = y
	};
	return result;
}

vect2_t vect2_clone(vect2_t* v) {
	vect2_t result = {v->x, v->y};
	return result;
}

vect2_t vect2_add(vect2_t a, vect2_t b){
	vect2_t result = { 
		.x = a.x + b.x, 
		.y = a.y + b.y 
	};
	return result;
}

vect2_t vect2_sub(vect2_t a, vect2_t b){
	vect2_t result = {
		.x = a.x - b.x,
		.y = a.y - b.y
	};
	return result;
}

vect2_t vect2_mul(vect2_t a, float factor){
	vect2_t result = {
		.x = a.x * factor,
		.y = a.y * factor
	};
	return result;
}

vect2_t vect2_div(vect2_t a, float factor){
	vect2_t result = {
		.x = a.x / factor,
		.y = a.y / factor
	};
	return result;
}

float vect2_dot(vect2_t a, vect2_t b){
	float result = a.x * b.x + a.y * b.y;
	return result;
}

float vect2_length(vect2_t v){
	return sqrt(v.x * v.x + v.y * v.y);
}

void vect2_normalize(vect2_t* v){
    float length = sqrt(v->x * v->x + v->y * v->y);
	v->x /= length;
	v->y /= length;

}

//////////////////////////////////////////////////////////////////////////////////
// Implementation of all vector 3d functions
//////////////////////////////////////////////////////////////////////////////////
vect3_t vect3_new(float x, float y, float z){
	vect3_t result = {
		.x = x,
		.y = y,
		.z = z
	};
	return result;
}

vect3_t vect3_clone(vect3_t* v){
	vect3_t result = {v->x, v->y, v->z};
	return result;
}

vect3_t vect3_add(vect3_t a, vect3_t b){
	vect3_t result = {
		.x = a.x + b.x,
		.y = a.y + b.y,
		.z = a.z + b.z
	};
	return result;
}

vect3_t vect3_sub(vect3_t a, vect3_t b){
	vect3_t result = {
		.x = a.x - b.x,
		.y = a.y - b.y,
		.z = a.z - b.z
	};
	return result;
}

vect3_t vect3_mul(vect3_t a, float factor){
	vect3_t result = {
		.x = a.x * factor,
		.y = a.y * factor,
		.z = a.z * factor
	};
	return result;
}

vect3_t vect3_div(vect3_t a, float factor){
	vect3_t result = {
		.x = a.x / factor,
		.y = a.y / factor,
		.z = a.z / factor
	};
	return result;
}


vect3_t vect3_cross(vect3_t a, vect3_t b){
	vect3_t result = {
		.x = a.y * b.z - a.z * b.y,
		.y = a.z * b.x - a.x * b.z,
		.z = a.x * b.y - a.y * b.x
	};
	return result;
}


float vect3_dot(vect3_t a, vect3_t b){
	float result = a.x * b.x + a.y * b.y + a.z * b.z;
	return result;
}


float vect3_length(vect3_t v){
	return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

void vect3_normalize(vect3_t* v){
	float length = sqrt(v->x * v->x + v->y * v->y + v->z * v->z);
	v->x /= length;
	v->y /= length;
	v->z /= length;
}


vect3_t vect3_rotate_x(vect3_t v, float angle){
	vect3_t rotated_vector = {
		.x = v.x,
		.y = v.y * cos(angle) - v.z * sin(angle),
		.z = v.y * sin(angle) + v.z * cos(angle)
	};
	return rotated_vector;

}
vect3_t vect3_rotate_y(vect3_t v, float angle){
	vect3_t rotated_vector = {
		.x = v.x * cos(angle) - v.z * sin(angle),
		.y = v.y,
		.z = v.x * sin(angle) + v.z * cos(angle)
	};
	return rotated_vector;
}
vect3_t vect3_rotate_z(vect3_t v, float angle){
	vect3_t rotated_vector = {
		.x = v.x * cos(angle) - v.y * sin(angle),
		.y = v.x * sin(angle) + v.y * cos(angle),
		.z = v.z
	};
	return rotated_vector;
}

//////////////////////////////////////////////////////////////////////////////////
// Implementation of all vector 3d functions
//////////////////////////////////////////////////////////////////////////////////
vect4_t vect4_from_vect3(vect3_t v){
	vect4_t result = {v.x, v.y, v.z, 1.0};
	return result;
}

vect3_t vect3_from_vect4(vect4_t v){
	vect3_t result = { v.x, v.y, v.z };
	return result;
}

vect2_t vect2_from_vect4(vect4_t v){
	vect2_t result = { v.x, v.y };
	return result;
}