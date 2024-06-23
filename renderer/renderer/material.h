#ifndef MATERIAL_H
#define MATERIAL_H
#include "stdint.h"
#include "vector.h"

typedef struct {
	float shininess;
	float specular_strength;
	uint32_t color;
}material_t;

#endif 

void init_material(uint32_t color, float shininess, float specular_strength);
uint32_t get_material_color(void);
float get_material_shininess(void);
float get_material_specular_strength(void);
