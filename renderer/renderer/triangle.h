#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "vector.h"

typedef struct {
	int a;
	int b;
	int c;

} face_t; // stores vexter index

typedef struct {

	vect2_t points[3];

} triangle_t ; // stores actual vec2 points of the triangle in the screen

#endif 