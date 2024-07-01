#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <sdl.h>
#include"vector.h"

#define FPS 60
#define FRAME_TARGET_TIME (1000 / FPS) // this is delta time in miliseconds

enum cull_method
{
	CULL_NONE,
	CULL_BACKFACE

}cull_method;

enum render_method
{
	RENDER_WIRE,
	RENDER_WIRE_VERTEX,
	RENDER_FILL_TRIANGLE,
	RENDER_AABB_TRIANGLE,
	RENDER_AABB_TEXTURED_TRIANGLE,
	RENDER_FILL_TRIANGLE_WIRE,
	RENDER_TEXTURED,
	RENDER_TEXTURED_WIRE

}render_method;

typedef struct {
	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t a;
}color_t;


bool initialize_window(void);
int get_window_width(void);
int get_window_height(void);

void set_cull_method(int method);
void set_render_method(int method);
bool is_cull_backface(void);

bool should_render_fill_triangle(void);
bool should_render_aabb_triangle(void);
bool should_render_texture_triangle(void);
bool should_render_aabb_texture_triangle(void);
bool should_render_wireframe(void);
bool should_render_wire_vertex(void);


void draw_pixel(int x, int y, uint32_t color);
void draw_line(int x0, int y0, int x1, int y1, uint32_t color);
void draw_rect(int upper_left_pos_x, int upper_left_pos_y, int width, int height, uint32_t color);
void draw_grid(void);

void clear_color_buffer(uint32_t color);
void clear_z_buffer(void);
void render_color_buffer(void);

float get_z_buffer_at(int x, int y);
void update_z_buffer_at(int x, int y, float value);

vect4_t mul_colors(vect4_t c1, vect4_t  c2);
uint32_t pack_color(float r, float g, float b, float a);
void unpack_color(uint32_t color, float* r, float* g, float* b, float* a);

#endif // !DISPLAY_H

