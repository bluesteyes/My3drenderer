#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <stdint.h>
#include "texture.h"
#include "vector.h"
#include "upng.h"


typedef struct {
	int a;         //model triangle face vertex index
	int b;
	int c;
	int n0;        //model triangle face vertex normal index
	int n1;
	int n2;
	tex2_t a_uv;
	tex2_t b_uv;
	tex2_t c_uv;
	uint32_t color;

} face_t; // stores vertex index <--- it's a triangle face

typedef struct {

	vect4_t points[3];
	vect3_t normals[3];
	tex2_t texcoords[3];
	vect3_t vertex_colors[3];
	uint32_t color;
	upng_t* texture;
	float light_intensity_factor;

} triangle_t ; // stores actual vec2 points of the triangle in the screen

void draw_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color);

void draw_filled_triangle(
	int x0, int y0, float z0, float w0,
	int x1, int y1, float z1, float w1,
	int x2, int y2, float z2, float w2,
	vect3_t n0, vect3_t n1, vect3_t n2,
	vect3_t c0, vect3_t c1, vect3_t c2,
	uint32_t color
);

void draw_textured_triangle(
	int x0, int y0, float z0, float w0, float u0, float v0,
	int x1, int y1, float z1, float w1, float u1, float v1,
	int x2, int y2, float z2, float w2, float u2, float v2,
	upng_t* texture, float light_intensity_factor);

void draw_triangle_pixel( 
	int x, int y,   
	vect4_t point_a, vect4_t point_b, vect4_t point_c,
	vect3_t n0, vect3_t n1, vect3_t n2,
	vect3_t c0, vect3_t c1, vect3_t c2,
	uint32_t color  
	);

void draw_triangle_texel(int x, int y, upng_t* texture,
	vect4_t point_a, vect4_t point_b, vect4_t point_c,
	tex2_t a_uv, tex2_t b_uv, tex2_t c_uv, 
	float light_intensity_factor);

vect3_t get_face_normal(vect4_t vertices[3]);


#endif 