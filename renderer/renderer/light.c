
#include "light.h"
#include "display.h"
#include "mesh.h"

light_t light;

void init_light(vect3_t direction, vect3_t color, float ambient) {
	light.direction = direction;
	light.color = color;
	light.ambient_strength = ambient;
}

vect3_t get_light_direction(void) {
	return light.direction;
}

vect3_t get_light_color(void) {
	return light.color;
}

float get_light_ambient_strgenth(void) {
	return light.ambient_strength;
}

uint32_t light_apply_intensity(uint32_t original_color, float percentage_factor) {

	uint32_t a = (original_color & 0xFF000000);
	uint32_t r = (original_color & 0x00FF0000) * percentage_factor;
	uint32_t g = (original_color & 0x0000FF00) * percentage_factor;
	uint32_t b = (original_color & 0x000000FF) * percentage_factor;
	
	uint32_t new_color = a | (r & 0x00FF0000) | (g & 0x0000FF00) | (b & 0x000000FF) ;

	return new_color;
}

uint32_t blinn_phong_shading(vect3_t face_normal, vect3_t light_direction, vect3_t view_direction,
	face_t* mesh_face, float shininess, float ambient_strength, float specular_strength) {


	//unpack the mesh_face.color to mesh_color
	vect4_t mesh_color = { 0.0, 0.0, 0.0, 0.0 };
	unpack_color(mesh_face->color, &mesh_color.x, &mesh_color.y, &mesh_color.z, &mesh_color.w);


	//Ambient component
	vect3_t ambient = vect3_mul(get_light_color(), ambient_strength);

	//Diffuse component
	float diffuse_intensity_factor = fmax(-vect3_dot(face_normal, light_direction), 0.0);
	vect3_t diffuse = vect3_mul(get_light_color(), diffuse_intensity_factor);

	
	//Specular component 
	//Find the half vector between light direction and view direction near the face normal
	vect3_t halfway_direction = vect3_add(view_direction, vect3_mul(get_light_direction(), -1.0)); //inverse the light direction
	vect3_normalize(&halfway_direction);

	float specular_intensity_factor = pow(fmax(vect3_dot(face_normal, halfway_direction), 0.0), shininess);
	vect3_t specular = vect3_mul(get_light_color(), specular_intensity_factor * specular_strength);

	//Combine all components 
	vect3_t color = vect3_add(ambient, vect3_add(diffuse, specular));
	color = vect3_div(color, 3.0); //normalize the color


	//Modulate with mesh color
	color.x *= mesh_color.x;
	color.y *= mesh_color.y;
	color.z *= mesh_color.z;

	//Pack the final color into uint32_t
	uint32_t triangle_color = pack_color(color.x, color.y, color.z, 1.0); //Assuming full opacity;

	return triangle_color;


}