#include "triangle.h"
#include "display.h"
#include "swap.h"
#include "camera.h"
#include "light.h"
#include "material.h"

#define MIN(a,b)(((a) < (b)) ? (a):(b))
#define MAX(a,b)(((a) > (b)) ? (a):(b))
#define CLAMP(x, lower, upper) ((x) < (lower) ? (lower) : ((x) > (upper) ? (upper) : (x)))

//Create implementation for triangle.h functions
vect3_t get_face_normal(vect4_t vertices[3]) {

	vect3_t vector_a = vect3_from_vect4(vertices[0]); /*  A  */
	vect3_t vector_b = vect3_from_vect4(vertices[1]); /* / \ */
	vect3_t vector_c = vect3_from_vect4(vertices[2]); /*C---B*/

	//get the vector subtraction of (B - A) and (C - B)
	vect3_t vector_ab = vect3_sub(vector_b, vector_a);
	vect3_t vector_ac = vect3_sub(vector_c, vector_a);

	vect3_normalize(&vector_ab);
	vect3_normalize(&vector_ac);

	//compute the face normal (using cross product to find perpendicular)
	vect3_t normal_vect = vect3_cross(vector_ab, vector_ac);
	
	//normalize the face normal vector
	vect3_normalize(&normal_vect);

	return normal_vect;
}



///////////////////////////////////////////////////////////////////////////////
// Return barycentric weights alpha, beta and gamma for point p inside a triangle
///////////////////////////////////////////////////////////////////////////////
//
//         (B)
//         /|\
//        / | \
//       /  |  \
//      /  (P)  \
//     /  /   \  \
//    / /       \ \
//   //           \\
//  (A)------------(C)
//
///////////////////////////////////////////////////////////////////////////////
vect3_t barycentric_weights(vect2_t a, vect2_t b, vect2_t c, vect2_t p) {

	//find the vectors between vertices A, B, C and point P
	vect2_t ab = vect2_sub(b, a);
	vect2_t ac = vect2_sub(c, a);
	vect2_t ap = vect2_sub(p, a);
	vect2_t pb = vect2_sub(b, p);
	vect2_t pc = vect2_sub(c, p);

	//computer the area of the full parallelogram / triangle ABC using 2d cross product
	float area_parallelogram_abc = (ac.x * ab.y - ab.x * ac.y);// det|ac ab|

	//computer the barycentric coordinates alpha, beta and gamma

	//alpha is the area of small triangle PBC divided by area of full triangle ABC
	float alpha = (pc.x * pb.y - pb.x * pc.y) / area_parallelogram_abc;
	
	//beta is the area of small triangle PAC divided by area of full triangle ABC
	float beta = (ac.x * ap.y - ap.x * ac.y) / area_parallelogram_abc;

	float gamma = 1 - alpha - beta;

	vect3_t weights = {alpha, beta, gamma};
	return weights;
}

///////////////////////////////////////////////////////////////////////////////
// Draw a filled a triangle with a flat bottom
///////////////////////////////////////////////////////////////////////////////
//
//        (x0,y0)
//          / \
//         /   \
//        /     \
//       /       \
//      /         \
//  (x1,y1)------(x2,y2) <---(Mx,My)
//
///////////////////////////////////////////////////////////////////////////////
void fill_flat_bottom_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color){
	//start from the top(x0,x0) calculate the slope1 of left edge and slope2 of right edge
	float inverse_slope_1 = (float)(x1 - x0) / (y1 - y0);
	float inverse_slope_2 = (float)(x2 - x0) / (y2 - y0);

	//initialize x_start and x_end from the top vertex (x0,y0)
	float x_start = x0;
	float x_end = x0;

	//Loop all scanlines from top to bottom (y0 to y2)
	for (int y = y0; y <= y2; y++){
		//draw a line from x_start until x_end
		draw_line(x_start, y, x_end, y, color);

		//based on the slope value increment x_start and x_end for the next scanline
		x_start += inverse_slope_1;
		x_end += inverse_slope_2;
	}

}
///////////////////////////////////////////////////////////////////////////////
// Draw a filled a triangle with a flat top
///////////////////////////////////////////////////////////////////////////////
//        (x0,y0)-----------(x1,y1)  <---(Mx,My)
//              \           /
//               \         /
//                \       /   
//                 \     /   
//                  \   /
//                   \ /            
//                  (x2,y2)
///////////////////////////////////////////////////////////////////////////////
void fill_flat_top_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color){
	//start from the bottom (x2,x2) calculate the slope1 of left edge and slope2 of right edge
	float inverse_slope_1 = (float)(x2 - x0) / (y2 - y0);
	float inverse_slope_2 = (float)(x2 - x1) / (y2 - y1);

	//initialize x_start and x_end from the bottom vertex (x2,y2)
	float x_start = x2;
	float x_end = x2;

	//Loop all scanlines from bottom to top (y2 to y0)
	for (int y = y2; y >= y0; y--){
		//draw a line from x_start until x_end
		draw_line(x_start, y, x_end, y, color);

		//based on the slope value increment x_start and x_end for the next scanline
		x_start -= inverse_slope_1;
		x_end -= inverse_slope_2;
	}
}

///////////////////////////////////////////////////////////////////////////////
// Draw a triangle using three raw line calls
///////////////////////////////////////////////////////////////////////////////
void draw_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color) {
	draw_line(x0, y0, x1, y1, color);
	draw_line(x1, y1, x2, y2, color);
	draw_line(x2, y2, x0, y0, color);
}

///////////////////////////////////////////////////////////////////////////////
// Function to draw the z-tested pixel at x,y position 
///////////////////////////////////////////////////////////////////////////////

void draw_triangle_pixel(
	int x, int y, 
	vect4_t point_a, vect4_t point_b, vect4_t point_c,
	vect3_t n0, vect3_t n1, vect3_t n2,
	vect3_t c0, vect3_t c1, vect3_t c2,
	uint32_t flat_color ) {

	vect2_t p = { x, y };

	vect2_t a = vect2_from_vect4(point_a);
	vect2_t b = vect2_from_vect4(point_b);
	vect2_t c = vect2_from_vect4(point_c);

	vect3_t weights = barycentric_weights(a, b, c, p);

	float alpha = weights.x;
	float beta = weights.y;
	float gamma = weights.z;


	float interpolated_reciprocal_w = (1 / point_a.w) * alpha + (1 / point_b.w) * beta + (1 / point_c.w) * gamma;

	//interpolate accumulated vertex normals
	vect3_t interpolated_normal = vect3_add(vect3_mul(n0, alpha), vect3_add(vect3_mul(n1, beta), vect3_mul(n2, gamma)));
	vect3_normalize(&interpolated_normal);


	//printf("Interpolated Normal: (%f, %f, %f)\n", interpolated_normal.x, interpolated_normal.y, interpolated_normal.z);

	vect3_t target_position = vect3_new(x, y, interpolated_reciprocal_w);

	vect3_t view_direction = vect3_sub(get_camera_position(), target_position);


	//adjust the value of 1/w so the pixels that are closer to the camera with smaller values
	interpolated_reciprocal_w = 1.0 - interpolated_reciprocal_w;

	// Only draw the pixel if the depth value is less than the one previously stored in the z-buffer
	if (interpolated_reciprocal_w < get_z_buffer_at(x ,y)) {

		// Draw a pixel at position (x,y) with a solid color
		draw_pixel(x, y, flat_color);

		// Update the z-buffer value with the 1/w of this current pixel
		update_z_buffer_at(x,y,interpolated_reciprocal_w);
	}
}

///////////////////////////////////////////////////////////////////////////////
// Function to draw the textured pixel at x,y position using interpolation
///////////////////////////////////////////////////////////////////////////////

void draw_triangle_texel(
	int x, int y, upng_t* texture, 
	vect4_t point_a, vect4_t point_b, vect4_t point_c, 
	tex2_t a_uv, tex2_t b_uv, tex2_t c_uv,
	vect3_t n0, vect3_t n1, vect3_t n2,
	float light_intensity_factor, uint32_t flat_color
){
	vect2_t p = {x, y};

	vect2_t a = vect2_from_vect4(point_a);
	vect2_t b = vect2_from_vect4(point_b);
	vect2_t c = vect2_from_vect4(point_c);

	vect3_t weights = barycentric_weights(a, b, c, p);

	float alpha = weights.x;
	float beta = weights.y;
	float gamma = weights.z;

	//Variables to store the interpolated values of u,v and also 1/w for the current pixel
	float interpolated_u;
	float interpolated_v;
	float interpolated_reciprocal_w; // w is not linear only 1/w is linear


	//Perform interpolation of all u/w and v/w and 1/w using barycentric weights -> perform texture correctness
	interpolated_u = (a_uv.u / point_a.w) * alpha + (b_uv.u / point_b.w) * beta + (c_uv.u / point_c.w) * gamma;
	interpolated_v = (a_uv.v / point_a.w) * alpha + (b_uv.v / point_b.w) * beta + (c_uv.v / point_c.w) * gamma;
	interpolated_reciprocal_w = (1 / point_a.w) * alpha + (1 / point_b.w) * beta + (1 / point_c.w) * gamma;

	//undoes the perspective transformation
	interpolated_u /= interpolated_reciprocal_w;
	interpolated_v /= interpolated_reciprocal_w;

	int texture_width = upng_get_width(texture);
	int texture_height = upng_get_height(texture);

	//map the uv coordinates to the full texture width and height
	int tex_x = abs((int)(interpolated_u * texture_width)) % texture_width;
	int tex_y = abs((int)(interpolated_v * texture_height)) % texture_height;

	//* adjust the value of 1/w so the pixels that are closer to the camera with smaller values
	interpolated_reciprocal_w = 1.0 - interpolated_reciprocal_w;

	//interpolate accumulated vertex normals
	vect3_t interpolated_normal = vect3_add(vect3_mul(n0, alpha), vect3_add(vect3_mul(n1, beta), vect3_mul(n2, gamma)));
	vect3_normalize(&interpolated_normal);

	vect3_t target_position = vect3_new(x, y, interpolated_reciprocal_w);

	vect3_t view_direction = vect3_sub(get_camera_position(), target_position);


	//Interpolate vertex colors
	//vect3_t interpolated_color = vect3_add(vect3_mul(c0, alpha), vect3_add(vect3_mul(c1, beta), vect3_mul(c2, gamma)));

	//Pack the final color into uint32_t
	//uint32_t gouraud_color = pack_color(interpolated_color.x, interpolated_color.y, interpolated_color.z, 1.0); //Assuming full opacity;

	//Phong shading
	/*uint32_t phong_color = blinn_phong_shading(interpolated_normal, get_light_direction(), view_direction,
		get_material_color(), get_material_shininess(), get_light_ambient_strgenth(), get_material_specular_strength());*/


	//only draw the pixel if the depth value is less than the one perviously stored in the z-buffer
	if (interpolated_reciprocal_w < get_z_buffer_at(x,y)) {
		
		uint32_t* texture_buffer =(uint32_t*)upng_get_buffer(texture);

		uint32_t texture_pixel = texture_buffer[(texture_width * tex_y) + tex_x];

		////unpack the texture pixel to pixel color 
		vect4_t pixel_color = vect4_new(0.0, 0.0, 0.0, 0.0);
		unpack_color(texture_pixel, &pixel_color.x, &pixel_color.y, &pixel_color.z, &pixel_color.w);

		//vect4_t phong_shading = vect4_new(0.0, 0.0, 0.0, 0.0);
		//unpack_color(phong_color, &phong_shading.x, &phong_shading.y, &phong_shading.z, &phong_shading.w);

		vect4_t flat_shading = vect4_new(0.0, 0.0, 0.0, 0.0);
		unpack_color(flat_color, &flat_shading.x, &flat_shading.y, &flat_shading.z, &flat_shading.w);


		//vect4_t diffuse_light = vect4_new(0.0, 0.0, 0.0, 0.0); 
		//vect4_t ambient_light = vect4_new(0.0, 0.0, 0.0, 0.0);
		//vect4_t specular_light = vect4_new(0.0, 0.0, 0.0, 0.0);

		//unpack_color(diffuse, &diffuse_light.x, &diffuse_light.y, &diffuse_light.z, &diffuse_light.w);
		//unpack_color(ambient, &ambient_light.x, &ambient_light.y, &ambient_light.z, &ambient_light.w);
		//unpack_color(specular, &specular_light.x, &specular_light.y, &specular_light.z, &specular_light.w);
		//
		//vect3_t unpacked_color = vect3_add(vect3_from_vect4(ambient_light), 
		//vect3_add(vect3_from_vect4(diffuse_light), vect3_from_vect4(specular_light)));
		//unpacked_color = vect3_div(unpacked_color, 3.0);

		//unpacked_color.x *= pixel_color.x;
		//unpacked_color.y *= pixel_color.y;
		//unpacked_color.z *= pixel_color.z;

		////Pack the final color into uint32_t
		//uint32_t shaded_texture_pixel = pack_color(unpacked_color.x, unpacked_color.y, unpacked_color.z, 1.0); //Assuming full opacity

		//vect4_t result = mul_colors(pixel_color, flat_shading);

		vect4_t result = {
			pixel_color.x * flat_shading.x,
			pixel_color.y * flat_shading.y,
			pixel_color.z * flat_shading.z,
		};

		uint32_t shaded_texture_pixel = pack_color(result.x, result.y, result.z, result.w);	
		draw_pixel(x, y, shaded_texture_pixel);

		//update the z-buffer value with the 1/w value of this current pixel
		update_z_buffer_at(x,y,interpolated_reciprocal_w);
	}
	
}

//////////////////////////////////////////////////////////////////////////////////
// Draw a filled triangle with the flat-top/flat-bottom method
// We split the original triangle in two, half flat-bottom and half flat-top
//////////////////////////////////////////////////////////////////////////////////
//
//                 (x0,y0)
//                   / \
//                  /   \
//                 /     \    
//                /       \
//               /         \
//         (x1,y1)---------(Mx,My)
//              \            \
//                --          \
//                   \         \
//                     --       \
//                        \      \
//                          --    \
//                             \   \
//                               -- \
//                                   \    
//                                 (x2,y2)
// 
//////////////////////////////////////////////////////////////////////////////////
void draw_filled_triangle(
	int x0, int y0, float z0, float w0, 
	int x1, int y1, float z1, float w1, 
	int x2, int y2, float z2, float w2, 
	vect3_t n0, vect3_t n1, vect3_t n2,
	vect3_t c0, vect3_t c1, vect3_t c2,
	uint32_t color
){
	
	if (y0 > y1) {
		int_swap(&y0, &y1);
		int_swap(&x0, &x1);
		float_swap(&z0, &z1);
		float_swap(&w0, &w1);
	}
	if (y1 > y2) {
		int_swap(&y1, &y2);
		int_swap(&x1, &x2);
		float_swap(&z1, &z2);
		float_swap(&w1, &w2);
	}
	if (y0 > y1) {
		int_swap(&y0, &y1);
		int_swap(&x0, &x1);
		float_swap(&z0, &z1);
		float_swap(&w0, &w1);
	}

	//create vector points after we sorted the vertices
	vect4_t point_a = { x0, y0, z0, w0 };
	vect4_t point_b = { x1, y1, z1, w1 };
	vect4_t point_c = { x2, y2, z2, w2 };


	////////////////////////////////////////////////////////////////////////////
	//render the upper part of the triangle (flat-bottom)
	////////////////////////////////////////////////////////////////////////////

	float inverse_slope_1 = 0;
	float inverse_slope_2 = 0;

	if (y0 != y1) inverse_slope_1 = (float)(x1 - x0) / abs(y1 - y0);
	if (y0 != y2) inverse_slope_2 = (float)(x2 - x0) / abs(y2 - y0);

	if (y1 != y0) {
		//Loop all scanlines from top to bottom (y0 to y2)
		for (int y = y0; y <= y1; y++) {

			//based on the slope value increment x_start and x_end for the next scanline
			int x_start = x1 + (y - y1) * inverse_slope_1;
			int x_end = x0 + (y - y0) * inverse_slope_2;

			if (x_end < x_start){
				int_swap(&x_start, &x_end); //swap the position if x_end at the left of x_start
			}

			for (int x = x_start; x < x_end; x++){
				//Draw our pixel with the color from left to right
				draw_triangle_pixel(x, y, point_a, point_b, point_c, n0, n1, n2, c0, c1, c2, color);
			}
		}
	}

	////////////////////////////////////////////////////////////////////////////
	//render the bottom part of the triangle (flat-top)
	////////////////////////////////////////////////////////////////////////////

	inverse_slope_1 = 0;
	inverse_slope_2 = 0;

	if (y1 != y2) inverse_slope_1 = (float)(x2 - x1) / abs(y2 - y1);
	if (y0 != y2) inverse_slope_2 = (float)(x2 - x0) / abs(y2 - y0);

	if (y1 != y2) {

		//Loop all scanlines from bottom to top (y2 to y1)
		for (int y = y1; y <= y2; y++) {

			//based on the slope value increment x_start and x_end for the next scanline
			int x_start = x1 + (y - y1) * inverse_slope_1;
			int x_end = x0 + (y - y0) * inverse_slope_2;

			if (x_end < x_start) {
				int_swap(&x_start, &x_end); //swap the position if x_end at the left of x_start
			}

			for (int x = x_start; x < x_end; x++) {

				//Draw our pixel with the solid color from left to right
				draw_triangle_pixel(x, y, point_a, point_b, point_c, n0, n1, n2, c0, c1, c2, color);

			}
		}
	}

}
//////////////////////////////////////////////////////////////////////////////////
// Draw a textured triangle with the flat-top/flat-bottom method
// We split the original triangle in two, half flat-bottom and half flat-top
//////////////////////////////////////////////////////////////////////////////////
//
//                    v0
//                   / \
//                  /   \
//                 /     \    
//                /       \
//               /         \
//             v1           \
//              \            \
//                --          \
//                   \         \
//                     --       \
//                        \      \
//                          --    \
//                             \   \
//                               -- \
//                                   \    
//                                   v2
//////////////////////////////////////////////////////////////////////////////////
void draw_textured_triangle(
	int x0, int y0, float z0, float w0, float u0, float v0,
	int x1, int y1, float z1, float w1, float u1, float v1,
	int x2, int y2, float z2, float w2, float u2, float v2,
	vect3_t n0, vect3_t n1, vect3_t n2,
	upng_t* texture, float light_intensity_factor, uint32_t color) {

	//TODO:
	//loop all the pixels of the triangle to render them based on the 
	//color that comes from the texture
	//we need to sort the vertices by y coordinates ascending(y0< y1 < y2)

	if (y0 > y1) {
		int_swap(&y0, &y1);
		int_swap(&x0, &x1);

		float_swap(&z0, &z1);
		float_swap(&w0, &w1);

		float_swap(&v0, &v1);
		float_swap(&u0, &u1);
		
	}
	if (y1 > y2) {
		int_swap(&y1, &y2);
		int_swap(&x1, &x2);
		float_swap(&z1, &z2);
		float_swap(&w1, &w2);
		float_swap(&v1, &v2);
		float_swap(&u1, &u2);
	}
	if (y0 > y1) {
		int_swap(&y0, &y1);
		int_swap(&x0, &x1);
		float_swap(&z0, &z1);
		float_swap(&w0, &w1);
		float_swap(&v0, &v1);
		float_swap(&u0, &u1);
	}

	//flip the v component to account for inverted uv-coordinates (v grows downwards)
	v0 = 1.0 - v0;
	v1 = 1.0 - v1;
	v2 = 1.0 - v2;

	//create vector points after we sorted the vertices
	vect4_t point_a = { x0, y0, z0, w0 };
	vect4_t point_b = { x1, y1, z1, w1 };
	vect4_t point_c = { x2, y2, z2, w2 };

	tex2_t a_uv = { u0, v0 };
	tex2_t b_uv = { u1, v1 };
	tex2_t c_uv = { u2, v2 };

	////////////////////////////////////////////////////////////////////////////
	//render the upper part of the triangle (flat-bottom)
	////////////////////////////////////////////////////////////////////////////
	
	float inverse_slope_1 = 0;
	float inverse_slope_2 = 0;

	if(y0 != y1) inverse_slope_1 = (float)(x1 - x0) / abs(y1 - y0);
	if(y0 != y2) inverse_slope_2 = (float)(x2 - x0) / abs(y2 - y0);

	if (y1 != y0){
		//Loop all scanlines from top to bottom (y0 to y2)
		for (int y = y0; y <= y1; y++){

			//based on the slope value increment x_start and x_end for the next scanline
			int x_start = x1 + (y - y1) * inverse_slope_1;
			int x_end = x0 + (y - y0) * inverse_slope_2;

			if (x_end < x_start)
			{
				int_swap(&x_start, &x_end); //swap the position if x_end at the left of x_start
			}

			for (int x = x_start; x < x_end; x++)
			{
				//Draw our pixel with the color that comes from the texture
				draw_triangle_texel(x, y, texture, point_a, point_b, point_c, a_uv, b_uv, c_uv, n0, n1, n2, light_intensity_factor, color);
			}
		}
	}

	////////////////////////////////////////////////////////////////////////////
	//render the bottom part of the triangle (flat-top)
	////////////////////////////////////////////////////////////////////////////
	inverse_slope_1 = 0;
	inverse_slope_2 = 0;

	if (y1 != y2) inverse_slope_1 = (float)(x2 - x1) / abs(y2 - y1);
	if (y0 != y2) inverse_slope_2 = (float)(x2 - x0) / abs(y2 - y0);

	if (y1 != y2) {

		//Loop all scanlines from bottom to top (y2 to y1)
		for (int y = y1; y <= y2; y++) {

			//based on the slope value increment x_start and x_end for the next scanline
			int x_start = x1 + (y - y1) * inverse_slope_1;
			int x_end = x0 + (y - y0) * inverse_slope_2;

			if (x_end < x_start){
				int_swap(&x_start, &x_end); //swap the position if x_end at the left of x_start
			}
			for (int x = x_start; x < x_end; x++){
				//Draw our pixel with the color that comes from the texture
				draw_triangle_texel(x, y, texture, point_a, point_b, point_c, a_uv, b_uv, c_uv, n0, n1, n2, light_intensity_factor, color);
			}
		}
	}
}

static bool is_top_left(vect2_t* start, vect2_t* end) {
	
	vect2_t edge = {end->x - start->x, end->y - start->y};

	bool is_top_edge = edge.y == 0 && edge.x > 0;
	bool is_left_edge = edge.y < 0;                  //clock wise triangle left edge point up, then end.y is neg 
	return is_top_edge || is_left_edge;
}

static int edge_cross(vect2_t* a, vect2_t* b, vect2_t* p) {
		
	vect2_t ab = {b->x - a->x, b->y - a->y };
	vect2_t ap = {p->x - a->x, p->y - a->y};
	return ab.x * ap.y - ab.y * ap.x;
}

void draw_aabb_triangle(
	int x0, int y0, float z0, float w0,
	int x1, int y1, float z1, float w1,
	int x2, int y2, float z2, float w2,
	vect3_t n0, vect3_t n1, vect3_t n2,
	vect3_t c0, vect3_t c1, vect3_t c2,
	uint32_t flat_color
	) {
	
	vect4_t point_a = { x0, y0, z0, w0 };
	vect4_t point_b = { x1, y1, z1, w1 };
	vect4_t point_c = { x2, y2, z2, w2 };

	vect2_t v0 = vect2_from_vect4(point_a);
	vect2_t v1 = vect2_from_vect4(point_b);
	vect2_t v2 = vect2_from_vect4(point_c);

	//Finds the bounding box with all candidate pixels -> Axis Aligned Bounding Box (AABB)
	int x_min = MIN(MIN(v0.x, v1.x), v2.x);
	int y_min = MIN(MIN(v0.y, v1.y), v2.y);
	int x_max = MAX(MAX(v0.x, v1.x), v2.x);
	int y_max = MAX(MAX(v0.y, v1.y), v2.y);


	//Finds the areas of entire triangle / paralellogram
	int area = edge_cross(&v0, &v1, &v2);
	int bias0 = is_top_left(&v1, &v2) ? 0 : -1;
	int bias1 = is_top_left(&v2, &v0) ? 0 : -1;
	int bias2 = is_top_left(&v0, &v1) ? 0 : -1;


	//Loop all candidate pixels inside the bounding box
	for (int y = y_min; y <= y_max; y++){
	
		for (int x = x_min; x <= x_max; x++){
			vect2_t p = {x, y};

			float w0 = edge_cross(&v1, &v2, &p) + bias0;
			float w1 = edge_cross(&v2, &v0, &p) + bias1;
			float w2 = edge_cross(&v0, &v1, &p) + bias2;

			bool is_inside = w0 >= 0 && w1 >= 0 & w2 >= 0;

			if(is_inside){

				//Finds the barycentric weights of triangle 
				float alpha = w0 / area;
				float beta  = w1 / area;
				float gamma = w2 / area;

				//interpolate the value of 1/w for the current pixel
				float interpolated_reciprocal_w = (1 / point_a.w) * alpha + (1 / point_b.w) * beta + (1 / point_c.w) * gamma;

				//interpolate accumulated vertex normals
				vect3_t interpolated_normal = vect3_add(vect3_mul(n0, alpha), vect3_add(vect3_mul(n1, beta), vect3_mul(n2, gamma)));
				vect3_normalize(&interpolated_normal);

				//adjust the value of 1/w so the pixels that are closer to the camera with smaller values
				interpolated_reciprocal_w = 1.0 - interpolated_reciprocal_w;

				vect3_t target_position = vect3_new(x, y, interpolated_reciprocal_w);

				vect3_t view_direction = vect3_sub(get_camera_position(), target_position);

				//Phong shading
				uint32_t phong_color = blinn_phong_reflection(interpolated_normal, get_light_direction(), view_direction,
					get_material_color(), get_material_shininess(), get_light_ambient_strgenth(), get_material_specular_strength());

				//Interpolate vertex colors
				vect3_t interpolated_color = vect3_add(vect3_mul(c0, alpha), vect3_add(vect3_mul(c1, beta), vect3_mul(c2, gamma)));

				//Pack the final color into uint32_t
				uint32_t gouraud_color = pack_color(interpolated_color.x, interpolated_color.y, interpolated_color.z, 1.0); //Assuming full opacity;

				// Only draw the pixel if the depth value is less than the one previously stored in the z-buffer
				if (interpolated_reciprocal_w < get_z_buffer_at(x, y)) {

					// Draw a pixel at position (x,y) with a color
					draw_pixel(x, y, phong_color);

					// Update the z-buffer value with the 1/w of this current pixel
					update_z_buffer_at(x, y, interpolated_reciprocal_w);
				}	
			}
		}
	}
}

void draw_aabb_textured_triangle(
	int x0, int y0, float z0, float w0, float u0, float v0,
	int x1, int y1, float z1, float w1, float u1, float v1,
	int x2, int y2, float z2, float w2, float u2, float v2,
	vect3_t n0, vect3_t n1, vect3_t n2,
	vect3_t t0, vect3_t t1, vect3_t t2,
	vect3_t b0, vect3_t b1, vect3_t b2,
	vect3_t c0, vect3_t c1, vect3_t c2,
	upng_t* texture, upng_t* normalmap, upng_t* glowmap, upng_t* roughmap,
	upng_t* metallic, upng_t* ao, 
	uint32_t flat_color 

) {

	////flip the v component to account for inverted uv-coordinates (v grows downwards)
	v0 = 1.0 - v0;
	v1 = 1.0 - v1;
	v2 = 1.0 - v2;

	//create vector points after we sorted the vertices
	vect4_t point_a = { x0, y0, z0, w0 };
	vect4_t point_b = { x1, y1, z1, w1 };
	vect4_t point_c = { x2, y2, z2, w2 };

	tex2_t a_uv = { u0, v0 };
	tex2_t b_uv = { u1, v1 };
	tex2_t c_uv = { u2, v2 };

	vect2_t a0 = { x0, y0 };
	vect2_t a1 = { x1, y1 };
	vect2_t a2 = { x2, y2 };

	//Finds the bounding box with all candidate pixels -> Axis Aligned Bounding Box (AABB)

	int x_min = MIN(MIN(a0.x, a1.x), a2.x);
	int y_min = MIN(MIN(a0.y, a1.y), a2.y);
	int x_max = MAX(MAX(a0.x, a1.x), a2.x);
	int y_max = MAX(MAX(a0.y, a1.y), a2.y);

	//Finds the areas of entire triangle / paralellogram
	int area = edge_cross(&a0, &a1, &a2);
	int bias0 = is_top_left(&a1, &a2) ? 0 : -1;
	int bias1 = is_top_left(&a2, &a0) ? 0 : -1;
	int bias2 = is_top_left(&a0, &a1) ? 0 : -1;

	//Loop all candidate pixels inside the bounding box
	for (int y = y_min; y <= y_max; y++) {

		for (int x = x_min; x <= x_max; x++) {
			vect2_t p = { x, y };

			float w0 = edge_cross(&a1, &a2, &p) + bias0;
			float w1 = edge_cross(&a2, &a0, &p) + bias1;
			float w2 = edge_cross(&a0, &a1, &p) + bias2;

			bool is_inside = w0 >= 0 && w1 >= 0 & w2 >= 0;

			if (is_inside) {

				//Finds the barycentric weights of triangle 
				float alpha = w0 / area;
				float beta =  w1 / area;
				float gamma = w2 / area;


				//Variables to store the interpolated values of u,v and also 1/w for the current pixel
				float interpolated_u;
				float interpolated_v;
				float interpolated_reciprocal_w; // w is not linear only 1/w is linear


				//Perform interpolation of all u/w and v/w and 1/w using barycentric weights -> perform texture correctness
				interpolated_u = (a_uv.u / point_a.w) * alpha + (b_uv.u / point_b.w) * beta + (c_uv.u / point_c.w) * gamma;
				interpolated_v = (a_uv.v / point_a.w) * alpha + (b_uv.v / point_b.w) * beta + (c_uv.v / point_c.w) * gamma;
				interpolated_reciprocal_w = (1 / point_a.w) * alpha + (1 / point_b.w) * beta + (1 / point_c.w) * gamma;

				//undoes the perspective transformation
				interpolated_u /= interpolated_reciprocal_w;
				interpolated_v /= interpolated_reciprocal_w;

				int texture_width = upng_get_width(texture);
				int texture_height = upng_get_height(texture);
				int normalmap_width = upng_get_width(normalmap);
				int normalmap_height = upng_get_height(normalmap);
				int roughmap_width = upng_get_width(roughmap);
				int roughmap_height = upng_get_height(roughmap);


				//map the uv coordinates to the full texture & normalmap width and height
				int tex_x = abs((int)(interpolated_u * texture_width)) % texture_width;
				int tex_y = abs((int)(interpolated_v * texture_height)) % texture_height;

				int normalmap_x = abs((int)(interpolated_u * normalmap_width)) % normalmap_width;
				int normalmap_y = abs((int)(interpolated_v * normalmap_height)) % normalmap_height;

				int roughmap_x = abs((int)(interpolated_u * roughmap_width)) % roughmap_width;
				int roughmap_y = abs((int)(interpolated_v * roughmap_height)) % roughmap_height;


				//* adjust the value of 1/w so the pixels that are closer to the camera with smaller values
				interpolated_reciprocal_w = 1.0 - interpolated_reciprocal_w;


				///******************** Normal Mapping ************************///
				//interpolate vertex tangent
				vect3_t interpolated_tangent = vect3_add(vect3_mul(t0, alpha), vect3_add(vect3_mul(t1, beta), vect3_mul(t2, gamma)));
				vect3_normalize(&interpolated_tangent);
				//interpolate vertex bitangent
				vect3_t interpolated_bitangent = vect3_add(vect3_mul(b0, alpha), vect3_add(vect3_mul(b1, beta), vect3_mul(b2, gamma)));
				vect3_normalize(&interpolated_bitangent);


				///******************** Phong Shading ************************///
				//interpolate accumulated vertex normals
				vect3_t interpolated_normal = vect3_add(vect3_mul(n0, alpha), vect3_add(vect3_mul(n1, beta), vect3_mul(n2, gamma)));
				vect3_normalize(&interpolated_normal);

				vect3_t target_position = vect3_new(x, y, interpolated_reciprocal_w);

				vect3_t view_direction = vect3_sub(get_camera_position(), target_position);


                ///********************* Gouraud Shading *********************///
				//Interpolate vertex colors
				vect3_t interpolated_color = vect3_add(vect3_mul(c0, alpha), vect3_add(vect3_mul(c1, beta), vect3_mul(c2, gamma)));

				//Pack the final color into uint32_t
				uint32_t gouraud_color = pack_color(interpolated_color.x, interpolated_color.y, interpolated_color.z, 1.0); //Assuming full opacity;


				///********************* Draw Pixels ************************///
				// Only draw the pixel if the depth value is less than the one previously stored in the z-buffer
				if (interpolated_reciprocal_w < get_z_buffer_at(x, y)) {


					///Sample the texture maps
					//get diffuse texture


					uint32_t* texture_buffer = (uint32_t*)upng_get_buffer(texture);
					uint32_t texture_pixel = texture_buffer[(texture_width * tex_y) + tex_x];
					

					//get tangent normal from the normal map texture
					uint32_t* normalmap_buffer = (uint32_t*)upng_get_buffer(normalmap);
					uint32_t tangent_normal = normalmap_buffer[(normalmap_width)*normalmap_y + normalmap_x];
					

					//get glow texture
					uint32_t* glowmap_buffer = (uint32_t*)upng_get_buffer(glowmap);
					uint32_t glowmap_pixel = glowmap_buffer[(texture_width * tex_y) + tex_x];
					

					//get roughness texture
					uint32_t* roughmap_buffer = (uint32_t*)upng_get_buffer(roughmap);
					uint32_t roughmap_pixel = roughmap_buffer[(roughmap_width * roughmap_y) + roughmap_x];
					
					//get metallic texture
					uint32_t* metallic_buffer = (uint32_t*)upng_get_buffer(metallic);
					uint32_t metallic_pixel = metallic_buffer[(texture_width * tex_y) + tex_x];
					
					
					//get ao texture
					uint32_t* ao_buffer = (uint32_t*)upng_get_buffer(ao);
					uint32_t ao_pixel = ao_buffer[(texture_width * tex_y) + tex_x];
					
					
				
					///*************** Call Fragment Shading Model ***************///
					//Phong Reflection model
					uint32_t phong_color = phong_reflection(interpolated_normal, interpolated_tangent, interpolated_bitangent, 
						get_light_direction(), view_direction,texture_pixel, glowmap_pixel, roughmap_pixel, tangent_normal, get_material_shininess());

					//Blinn-Phong Reflection model 
					uint32_t blinn_phong_color = blinn_phong_reflection(interpolated_normal, get_light_direction(), view_direction,
						texture_pixel, get_material_shininess(), get_light_ambient_strgenth(), get_material_specular_strength());

					//PBR reflection model
					uint32_t pbr_color = pbr_reflection(interpolated_normal, interpolated_tangent, interpolated_bitangent,
						get_light_direction(), view_direction, texture_pixel, tangent_normal,  metallic_pixel, roughmap_pixel, ao_pixel);

					///unpack the texture pixel to pixel color 
					//vect4_t pixel_color = vect4_new(0.0, 0.0, 0.0, 0.0);
					//unpack_color(texture_pixel, &pixel_color.x, &pixel_color.y, &pixel_color.z, &pixel_color.w);

					//vect4_t blinn_phong_shading = vect4_new(0.0, 0.0, 0.0, 0.0);
					//unpack_color(blinn_phong_color, &blinn_phong_shading.x, &blinn_phong_shading.y, &blinn_phong_shading.z, &blinn_phong_shading.w);

					//vect4_t flat_shading = vect4_new(0.0, 0.0, 0.0, 0.0);
					//unpack_color(flat_color, &flat_shading.x, &flat_shading.y, &flat_shading.z, &flat_shading.w);

					//vect4_t gouraud_shading = vect4_new(0.0, 0.0, 0.0, 0.0);
					//unpack_color(gouraud_color, &gouraud_shading.x, &gouraud_shading.y, &gouraud_shading.z, &gouraud_shading.w);

					//vect4_t result = mul_colors(pixel_color, blinn_phong_shading);
					//uint32_t shaded_texture_pixel = pack_color(result.x, result.y, result.z, result.w);


					interpolated_normal.x = CLAMP(interpolated_normal.x, 0.0f, 1.0f);
					interpolated_normal.y = CLAMP(interpolated_normal.y, 0.0f, 1.0f);
					interpolated_normal.z = CLAMP(interpolated_normal.z, 0.0f, 1.0f);

					uint32_t interpolated_normal_color = pack_color(interpolated_normal.x, interpolated_normal.y, interpolated_normal.z, 1.0f);
					uint32_t interpolated_tangent_color = pack_color(interpolated_tangent.x, interpolated_tangent.y, interpolated_tangent.z, 1.0f);
					uint32_t interpolated_bitangent_color = pack_color(interpolated_bitangent.x, interpolated_bitangent.y, interpolated_bitangent.z, 1.0f);
						

					// Draw a pixel at position (x,y) with a color
					draw_pixel(x, y, pbr_color);

					// Update the z-buffer value with the 1/w of this current pixel
					update_z_buffer_at(x, y, interpolated_reciprocal_w);
				}

			}

		}
	}

}

