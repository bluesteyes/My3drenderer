
#include "light.h"
#include "display.h"
#include "mesh.h"
#define CLAMP(x, lower, upper) ((x) < (lower) ? (lower) : ((x) > (upper) ? (upper) : (x)))

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

/// <summary>
/// blinn phong reflection model which using halfway direction to dot view direciton
/// </summary>
/// <param name="normal"></param>
/// <param name="light_direction"></param>
/// <param name="view_direction"></param>
/// <param name="material_color"></param>
/// <param name="shininess"></param>
/// <param name="ambient_strength"></param>
/// <param name="specular_strength"></param>
/// <returns></returns>
uint32_t blinn_phong_reflection(vect3_t normal, vect3_t light_direction, vect3_t view_direction,
	uint32_t material_color, float shininess, float ambient_strength, float specular_strength) {

	//normalize the input vector
	vect3_normalize(&normal);
	vect3_normalize(&light_direction);
	vect3_normalize(&view_direction);

	//unpack the material color and assign r,g,b,a to diffuse color
	vect4_t diffuse_color = vect4_new(0.0, 0.0, 0.0, 0.0);
	unpack_color(material_color, &diffuse_color.x, &diffuse_color.y, &diffuse_color.z, &diffuse_color.w);

	//Ambient component
	//vect3_t ambient = vect3_mul(get_light_color(), 0.2f);
	
	vect3_t ambient = vect3_mul(vect3_from_vect4(diffuse_color), ambient_strength);

	//Diffuse component
	float diff = fmax(vect3_dot(normal, vect3_mul(light_direction, -1.0)), 0.0); //inverse the light direction
	//float diff = 0.0f;
	//vect3_t diffuse = vect3_mul(get_light_color(), diff);

	vect3_t diffuse = {
		diff * get_light_color().x * diffuse_color.x,
		diff * get_light_color().y * diffuse_color.y,
		diff * get_light_color().z * diffuse_color.z,
	};

	//Specular component 
	//Find the half vector between light direction and view direction near the face normal
	vect3_t halfway_direction = vect3_add(view_direction, vect3_mul(light_direction, -1.0)); 
	vect3_normalize(&halfway_direction);
	float spec = pow(fmax(vect3_dot(normal, halfway_direction), 0.0), shininess);

	////Find the relection direction -> worse than halfway direction
	//vect3_t reflection_direction = vect3_sub(vect3_mul(normal,2.0f), vect3_mul(light_direction, -1.0));
	//vect3_normalize(&reflection_direction);
	//float spec = pow(fmax(vect3_dot(view_direction, reflection_direction), 0.0), shininess);
	
	//vect3_t specular = vect3_mul(get_light_color(), spec * specular_strength);

	vect3_t specular = {
		spec * get_light_color().x * specular_strength,
		spec * get_light_color().y * specular_strength,
		spec * get_light_color().z * specular_strength,
	};

	//Combine all components 
	vect3_t phong_color = vect3_add(ambient, vect3_add(diffuse, specular));
	
	phong_color = vect3_div(phong_color, 1.0f); //normalize the color

	//vect4_t result = mul_colors(vect4_from_vect3(phong_color), vect4_new(1.0f,1.0f,1.0f,1.0f));
	//vect4_t result = mul_colors(vect4_from_vect3(phong_color), diffuse_color);
	vect4_t result = vect4_from_vect3(phong_color);


	// Clamp the results to [0, 1]
	result.x = CLAMP(result.x, 0.0f, 1.0f);
	result.y = CLAMP(result.y, 0.0f, 1.0f);
	result.z = CLAMP(result.z, 0.0f, 1.0f);
	result.w = CLAMP(result.w, 0.0f, 1.0f);


	//Pack the final color into uint32_t
	uint32_t shaded_color = pack_color(result.x, result.y, result.z, result.w); //Assuming full opacity;

	return shaded_color;
}


/// <summary>
/// phong reflection model which using reflection direction to dot view direcion
/// </summary>
/// <param name="normal"></param>
/// <param name="light_direction"></param>
/// <param name="view_direction"></param>
/// <param name="color"></param>
/// <param name="shininess"></param>
/// <returns></returns>
uint32_t phong_reflection(vect3_t normal, vect3_t light_direction, vect3_t view_direction, 
	uint32_t color, float shininess ) {


	vect3_normalize(&light_direction);
	vect3_normalize(&normal);
	vect3_normalize(&view_direction);


	vect3_t light_color = get_light_color();
	vect3_t ambient_color = { 0.2f, 0.2f, 0.2f };

	//unpack the material color and assign r,g,b,a to diffuse color
	vect4_t diffuse_color = vect4_new(0.0f, 0.0f, 0.0f, 0.0f);
	unpack_color(color, &diffuse_color.x, &diffuse_color.y, &diffuse_color.z, &diffuse_color.w);
	//vect3_t diffuse_color = {color >> 16 & 0xFF, color >> 8 & 0xFF, color & 0xFF};
	vect3_t specular_color = { 0.3f, 0.3f, 0.3f };

	vect3_t ambient = {
		ambient_color.x * diffuse_color.x,
		ambient_color.y * diffuse_color.y,
		ambient_color.z * diffuse_color.z
	};

	// diffuse factor 
	float diff = fmaxf(vect3_dot(normal, vect3_mul(light_direction, -1.0f)), 0.0f);
	//float diff = 0.0f;

	vect3_t diffuse = {
		diff * light.color.x * diffuse_color.x,
		diff * light.color.y * diffuse_color.y,
		diff * light.color.z * diffuse_color.z,
	};

	vect3_t reflect_direction = {
		2.0f * normal.x * diff - light_direction.x * (-1.0f),
		2.0f * normal.y * diff - light_direction.y * (-1.0f),
		2.0f * normal.z * diff - light_direction.z * (-1.0f),
	};
	vect3_normalize(&reflect_direction);

	vect3_t halfway_direction = {
		view_direction.x + light_direction.x * (-1.0f),
		view_direction.y + light_direction.y * (-1.0f),
		view_direction.z + light_direction.z * (-1.0f),
	};
	vect3_normalize(&halfway_direction);

	//specular factor
	float spec = powf(fmaxf(vect3_dot(view_direction, reflect_direction), 0.0f), 32.0f);
	//float spec = powf(fmaxf(vect3_dot(normal, halfway_direction), 0.0f), shininess);


	vect3_t specular = {
		spec * specular_color.x * light_color.x,
		spec * specular_color.y * light_color.y,
		spec * specular_color.z * light_color.z,
	};

	vect3_t result = {
		ambient.x + diffuse.x + specular.x,
		ambient.y + diffuse.y + specular.y,
		ambient.z + diffuse.z + specular.z,
	};

	// Clamp the results to [0, 1]
	result.x = CLAMP(result.x, 0.0f, 1.0f);
	result.y = CLAMP(result.y, 0.0f, 1.0f);
	result.z = CLAMP(result.z, 0.0f, 1.0f);
	

	/*uint8_t r = (uint8_t)fminf(result.x * 255.0f, 255.0f);
	uint8_t g = (uint8_t)fminf(result.y * 255.0f, 255.0f);
	uint8_t b = (uint8_t)fminf(result.z * 255.0f, 255.0f);

	return (r << 16) | (g <<8) | b;*/

	return pack_color(result.x, result.y, result.z, 1.0f);
}