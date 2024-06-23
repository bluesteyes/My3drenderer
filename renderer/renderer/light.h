#ifndef LIGHT_H
#define LIGHT_H
#include <stdint.h>
#include "vector.h"

typedef struct {

	vect3_t direction;
	vect3_t color;
	float ambient_strength;

} light_t;


void init_light(vect3_t direction, vect3_t color, float ambient);

vect3_t get_light_direction(void);
vect3_t get_light_color(void);
float get_light_ambient_strgenth(void);

uint32_t light_apply_intensity(uint32_t original_color, float percentage_factor);

uint32_t blinn_phong_shading(vect3_t normal, vect3_t light_direction, vect3_t view_direction,
	face_t* mesh_face, float shininess, float ambient_strength, float specular_strength);

#endif 

