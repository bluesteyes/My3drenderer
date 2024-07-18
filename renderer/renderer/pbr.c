#include "pbr.h"
#include "light.h"
#include "display.h"
#include "mesh.h"
#include "matrix.h"

#define CLAMP(x, lower, upper) ((x) < (lower) ? (lower) : ((x) > (upper) ? (upper) : (x)))

vect3_t lerp(vect3_t a, vect3_t b, float t) {

	return vect3_add(vect3_mul(a, 1.0f - t), vect3_mul(b, t));
}

// Function to compute the GGX NDF
float GGX_Distribution(float NdotH, float roughness) {
    float alpha = roughness * roughness;
    float alpha2 = alpha * alpha;
    float NdotH2 = NdotH * NdotH;

    float denominator = (NdotH2 * (alpha2 - 1.0f) + 1.0f);
    denominator = M_PI * denominator * denominator;

    return alpha2 / fmaxf(denominator, SDL_FLT_EPSILON);
}

// Function to compute the Schlick-GGX geometric attenuation factor
float GeometrySchlickGGX(float NdotV, float roughness) {
    float alpha = roughness * roughness;
    float k = alpha / 2.0f;

    float numerator = NdotV;
    float denominator = NdotV * (1.0f - k) + k;

    return numerator / fmaxf(denominator, SDL_FLT_EPSILON);
}

// Function to compute the combined geometric attenuation factor
float GeometrySmith(float NdotV, float NdotL, float roughness) {
    float ggxV = GeometrySchlickGGX(NdotV, roughness);
    float ggxL = GeometrySchlickGGX(NdotL, roughness);

    return ggxV * ggxL;
}

// Fresnel function using Schlick's approximation
vect3_t FresnelSchlick(float cosTheta, vect3_t F0) {
    return lerp(F0, (vect3_t) { 1.0f, 1.0f, 1.0f }, powf(1.0f - cosTheta, 5.0f));
}


// BRDF calculation for PBR using metallic-roughness workflow
uint32_t BRDF_PBR_MetallicRoughness(vect3_t normal, vect3_t tangent, vect3_t bitangent, vect3_t light_direction,
    vect3_t view_direction, uint32_t albedo_map, uint32_t normal_map, uint32_t metallic_map,
    uint32_t roughness_map, uint32_t ao_map) {

    //Initialize light colors
    vect3_t light_color = get_light_color();

    //Normalize the input vector
    vect3_normalize(&light_direction);
    vect3_normalize(&normal);
    vect3_normalize(&tangent);
    vect3_normalize(&bitangent);
    vect3_normalize(&view_direction);

    //Calculate halfway direction
    vect3_t halfway_direction = vect3_add(view_direction, vect3_mul(light_direction, -1.0f));
    vect3_normalize(&halfway_direction);

   
    //unpack the tangent space normal data from normalmap to temp variable of range [0, 1]
    vect4_t unpacked_normal = vect4_new(0.0f, 0.0f, 0.0f, 0.0f);
    unpack_color(normal_map, &unpacked_normal.x, &unpacked_normal.y, &unpacked_normal.z, &unpacked_normal.w);
    vect3_normalize(&unpacked_normal);

    //transform tangent normal vector from [0, 1] to range [-1, 1] 
    vect3_t tangent_space_normal = vect3_sub(vect3_mul(vect3_from_vect4(unpacked_normal), 2.0f), vect3_new(1.0f, 1.0f, 1.0f, 1.0f));
    vect3_normalize(&tangent_space_normal);

    ///Transform the tangent space normal to worldspace and became perterbed normal
    vect3_t perturbed_normal = transform_NBT_to_world(tangent, bitangent, normal, tangent_space_normal);
    //vect3_t perturbed_normal = transform_TBN_to_world(tangent, bitangent, normal, tangent_space_normal);

    //unpack the material color 
    vect4_t albedo_color = vect4_new(0.0f, 0.0f, 0.0f, 0.0f);
    unpack_color(albedo_map, &albedo_color.x, &albedo_color.y, &albedo_color.z, &albedo_color.w);
    vect3_t albedo = { albedo_color.x, albedo_color.y, albedo_color.z };

    vect4_t ao_color = vect4_new(0.0f, 0.0f, 0.0f, 0.0f);
    unpack_color(ao_map, &ao_color.x, &ao_color.y, &ao_color.z, &ao_color.w);
    float ao = ao_color.x;

    vect4_t metallic_color = vect4_new(0.0f, 0.0f, 0.0f, 0.0f);
    unpack_color(metallic_map, &metallic_color.x, &metallic_color.y, &metallic_color.z, &metallic_color.w);
    float metallic = metallic_color.x;

    vect4_t roughness_color = vect4_new(0.0f, 0.0f, 0.0f, 0.0f);
    unpack_color(roughness_map, &roughness_color.x, &roughness_color.y, &roughness_color.z, &roughness_color.w);
    float roughness = roughness_color.x;
    float roughness2 = roughness * roughness;

    
    // Calculate dot product needed for the BRDF calculation
    float NdotL = fmaxf(vect3_dot(perturbed_normal, vect3_mul(light_direction, -1.0f)), 0.0f);
    float NdotV = fmaxf(vect3_dot(perturbed_normal, view_direction), 0.0f);
    float NdotH = fmaxf(vect3_dot(perturbed_normal, halfway_direction), 0.0f);
    float VdotH = fmaxf(vect3_dot(view_direction, halfway_direction), 0.0f);

    // Compute the base reflectance (F0)
    vect3_t F0 = lerp((vect3_t) { 0.04f, 0.04f, 0.04f }, albedo, metallic);

    // Calculate the GGX NDF
    float D = GGX_Distribution(NdotH, roughness);

    // Calculate the geometric attenuation
    float G = GeometrySmith(NdotV, NdotL, roughness);

    // Calculate the Fresnel term
    vect3_t F = FresnelSchlick(VdotH, F0);

    // Calculate the specular term
    vect3_t numerator = vect3_mul(F, D * G);
    float denominator = 4.0f * NdotV * NdotL;
    vect3_t specular = vect3_div(numerator, fmaxf(denominator, SDL_FLT_EPSILON));

    // Calculate the diffuse term
    vect3_t kS = F; // Fresnel term represents the specular reflection
    vect3_t kD = vect3_sub((vect3_t) { 1.0f, 1.0f, 1.0f }, kS); // Diffuse reflection
    kD = vect3_mul(kD, 1.0f - metallic); // Only non-metallic surfaces have diffuse component

    vect3_t diffuse = {
        kD.x * albedo.x,
        kD.y * albedo.y,
        kD.z * albedo.z,
    };

    // Combine the specular and diffuse components
   
   
 /*  vect3_t result = {
        (diffuse.x + specular.x)* NdotL* light_color.x,
        (diffuse.y + specular.y)* NdotL* light_color.y,
        (diffuse.z + specular.z)* NdotL* light_color.z,
    };*/

      vect3_t result = {

        (diffuse.x + specular.x)* NdotL* light_color.x,
        (diffuse.y + specular.y)* NdotL* light_color.y,
        (diffuse.z + specular.z)* NdotL* light_color.z,

   };


    // Clamp the results to [0, 1]
    result.x = CLAMP(result.x, 0.0f, 1.0f);
    result.y = CLAMP(result.y, 0.0f, 1.0f);
    result.z = CLAMP(result.z, 0.0f, 1.0f);

    //Apply ambient occlusion (AO) to the final color
    /*result.x *= ao;
    result.y *= ao;
    result.z *= ao;*/


    return pack_color(result.x, result.y, result.z, 1.0f);

}




// BRDF calculation for PBR using specular-glossiness workflow
uint32_t BRDF_PBR_SpecularGlossiness(vect3_t normal, vect3_t tangent, vect3_t bitangent, vect3_t light_direction,
    vect3_t view_direction, uint32_t albedo_map, uint32_t normal_map, uint32_t specular_map,
    uint32_t glossiness_map, uint32_t ao_map) {
    
    //Initialize light colors
    vect3_t light_color = get_light_color();

    //Normalize the input vector
    vect3_normalize(&light_direction);
    vect3_normalize(&normal);
    vect3_normalize(&tangent);
    vect3_normalize(&bitangent);
    vect3_normalize(&view_direction);

    //Calculate halfway direction
    vect3_t halfway_direction = vect3_add(view_direction, vect3_mul(light_direction, -1.0f));
    vect3_normalize(&halfway_direction);


    //unpack the tangent space normal data from normalmap to temp variable of range [0, 1]
    vect4_t unpacked_normal = vect4_new(0.0f, 0.0f, 0.0f, 0.0f);
    unpack_color(normal_map, &unpacked_normal.x, &unpacked_normal.y, &unpacked_normal.z, &unpacked_normal.w);
    //vect3_normalize(&unpacked_normal);

    //transform tangent normal vector from [0, 1] to range [-1, 1] 
    vect3_t tangent_space_normal = vect3_sub(vect3_mul(vect3_from_vect4(unpacked_normal), 2.0f), vect3_new(1.0f, 1.0f, 1.0f, 1.0f));
    vect3_normalize(&tangent_space_normal);

    ///Transform the tangent space normal to worldspace and became perterbed normal
    vect3_t perturbed_normal = transform_NBT_to_world(tangent, bitangent, normal, tangent_space_normal);
    //vect3_t perturbed_normal = transform_TBN_to_world(tangent, bitangent, normal, tangent_space_normal);

    //unpack the material color 
    vect4_t albedo_color = vect4_new(0.0f, 0.0f, 0.0f, 0.0f);
    unpack_color(albedo_map, &albedo_color.x, &albedo_color.y, &albedo_color.z, &albedo_color.w);
    vect3_t albedo = { albedo_color.x, albedo_color.y, albedo_color.z };

    vect4_t ao_color = vect4_new(0.0f, 0.0f, 0.0f, 0.0f);
    unpack_color(ao_map, &ao_color.x, &ao_color.y, &ao_color.z, &ao_color.w);
    float ao = ao_color.x;

    vect4_t specular_color = vect4_new(0.0f, 0.0f, 0.0f, 0.0f);
    unpack_color(specular_map, &specular_color.x, &specular_color.y, &specular_color.z, &specular_color.w);

    vect3_t specular_col = vect3_from_vect4(specular_color);

    vect4_t glossiness_color = vect4_new(0.0f, 0.0f, 0.0f, 0.0f);
    unpack_color(glossiness_map, &glossiness_color.x, &glossiness_color.y, &glossiness_color.z, &glossiness_color.w);
    float glossiness = glossiness_color.x;
   
    float roughness = 1.0f - glossiness;

    // Calculate dot product needed for the BRDF calculation
    float NdotL = fmaxf(vect3_dot(perturbed_normal, vect3_mul(light_direction, -1.0f)), 0.0f);
    float NdotV = fmaxf(vect3_dot(perturbed_normal, view_direction), 0.0f);
    float NdotH = fmaxf(vect3_dot(perturbed_normal, halfway_direction), 0.0f);
    float VdotH = fmaxf(vect3_dot(view_direction, halfway_direction), 0.0f);
    
  
    // Calculate the GGX NDF
    float D = GGX_Distribution(NdotH, roughness);

    // Calculate the geometric attenuation
    float G = GeometrySmith(NdotV, NdotL, roughness);

    // Calculate the Fresnel term
    vect3_t F = FresnelSchlick(VdotH, specular_col);

    // Calculate the specular term
    vect3_t numerator = vect3_mul(F, D * G);
    float denominator = 4.0f * NdotV * NdotL;
    vect3_t specular = vect3_div(numerator, fmaxf(denominator, SDL_FLT_EPSILON));

    // Calculate the diffuse term
    vect3_t diffuse = {
        albedo.x * (1.0f - specular_col.x),
        albedo.y * (1.0f - specular_col.y),
        albedo.z * (1.0f - specular_col.z),
    };

     vect3_t result = {
        (diffuse.x + specular.x)* NdotL* light_color.x * 1.5,
        (diffuse.y + specular.y)* NdotL* light_color.y * 1.5,
        (diffuse.z + specular.z)* NdotL* light_color.z * 1.5,
    };

    // Clamp the results to [0, 1]
    result.x = CLAMP(result.x, 0.0f, 1.0f);
    result.y = CLAMP(result.y, 0.0f, 1.0f);
    result.z = CLAMP(result.z, 0.0f, 1.0f);

    //Apply ambient occlusion (AO) to the final color
    /*result.x *= ao;
    result.y *= ao;
    result.z *= ao;*/


    return pack_color(result.x, result.y, result.z, 1.0f);

}

