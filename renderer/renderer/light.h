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

uint32_t blinn_phong_reflection(vect3_t normal, vect3_t light_direction, vect3_t view_direction, 
	uint32_t material_color, float shininess, float ambient_strength, float specular_strength);

uint32_t phong_reflection(vect3_t normal, vect3_t tangent, vect3_t bitangent, vect3_t light_direction, vect3_t view_direction,
	uint32_t color, uint32_t glowmap, uint32_t roughmap, uint32_t tangent_normal, float shininess);

uint32_t pbr_reflection(vect3_t normal, vect3_t tangent, vect3_t bitangent, vect3_t light_direction,
	vect3_t view_direction, uint32_t albedo_map, uint32_t normal_map, uint32_t metallic_map,
	uint32_t roughness_map, uint32_t ao_map);

#endif 

