#ifndef PBR_H
#define PBR_H

#include <stdint.h>
#include "vector.h"


vect3_t lerp(vect3_t a, vect3_t b, float t);
float GGX_Distribution(float NdotH, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(float NdotV, float NdotL, float roughness);
vect3_t FresnelSchlick(float cosTheta, vect3_t F0);

uint32_t BRDF_PBR_MetallicRoughness(vect3_t normal, vect3_t tangent, vect3_t bitangent, vect3_t light_direction,
    vect3_t view_direction, uint32_t albedo_map, uint32_t normal_map, uint32_t metallic_map,
    uint32_t roughness_map, uint32_t ao_map);

uint32_t BRDF_PBR_SpecularGlossiness(vect3_t normal, vect3_t tangent, vect3_t bitangent, vect3_t light_direction,
    vect3_t view_direction, uint32_t albedo_map, uint32_t normal_map, uint32_t specular_map,
    uint32_t glossiness_map, uint32_t ao_map);



#endif // !PBR_H

