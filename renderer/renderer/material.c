#include "material.h"
#include "stdint.h"

material_t material;


void init_material(uint32_t color, float shininess, float specular_strength) {

	material.color = color;
	material.shininess = shininess;
	material.specular_strength = specular_strength;

}
uint32_t get_material_color(void) {
	return material.color;
}

float get_material_shininess(void) {
	return material.shininess;
}
float get_material_specular_strength(void) {
	return material.specular_strength;
}