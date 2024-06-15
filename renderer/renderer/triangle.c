#include "triangle.h"
#include "display.h"
#include "swap.h"

//Create implementation for triangle.h functions

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
	uint32_t color
) {
	vect2_t p = { x, y };

	vect2_t a = vect2_from_vect4(point_a);
	vect2_t b = vect2_from_vect4(point_b);
	vect2_t c = vect2_from_vect4(point_c);

	vect3_t weights = barycentric_weights(a, b, c, p);

	float alpha = weights.x;
	float beta = weights.y;
	float gamma = weights.z;


	//interpolate the value of 1/w for the current pixel
	float interpolated_reciprocal_w = (1 / point_a.w) * alpha + (1 / point_b.w) * beta + (1 / point_c.w) * gamma;

	//adjust the value of 1/w so the pixels that are closer to the camera with smaller values
	interpolated_reciprocal_w = 1.0 - interpolated_reciprocal_w;


	// Only draw the pixel if the depth value is less than the one previously stored in the z-buffer
	if (interpolated_reciprocal_w < z_buffer[(window_width * y) + x]) {
		// Draw a pixel at position (x,y) with a solid color
		draw_pixel(x, y, color);

		// Update the z-buffer value with the 1/w of this current pixel
		z_buffer[(window_width * y) + x] = interpolated_reciprocal_w;
	}

}


///////////////////////////////////////////////////////////////////////////////
// Function to draw the textured pixel at x,y position using interpolation
///////////////////////////////////////////////////////////////////////////////

void draw_triangle_texel(
	int x, int y, uint32_t* texture, 
	vect4_t point_a, vect4_t point_b, vect4_t point_c, 
	tex2_t a_uv, tex2_t b_uv, tex2_t c_uv
){

	vect2_t p = {x, y};

	vect2_t a = vect2_from_vect4(point_a);
	vect2_t b = vect2_from_vect4(point_b);
	vect2_t c = vect2_from_vect4(point_c);

	vect3_t weights = barycentric_weights(a, b, c, p);

	float alpha = weights.x;
	float beta = weights.y;
	float gamma = weights.z;

	//variables to store the interpolated values of u,v and 1/w for the current pixel
	float interpolated_u;
	float interpolated_v;
	float interpolated_reciprocal_w;


	//perform interpolation of all u/w and v/w and 1/w using barycentric weights
	interpolated_u = (a_uv.u / point_a.w) * alpha + (b_uv.u / point_b.w) * beta + (c_uv.u / point_c.w) * gamma;
	interpolated_v = (a_uv.v / point_a.w) * alpha + (b_uv.v / point_b.w) * beta + (c_uv.v / point_c.w) * gamma;
	interpolated_reciprocal_w = (1 / point_a.w) * alpha + (1 / point_b.w) * beta + (1 / point_c.w) * gamma;

	//undoes the perspective transformation
	interpolated_u /= interpolated_reciprocal_w;
	interpolated_v /= interpolated_reciprocal_w;

	//map the uv coordinates to the full texture width and height
	int tex_x = abs((int)(interpolated_u * texture_width)) % texture_width;
	int tex_y = abs((int)(interpolated_v * texture_height)) % texture_height;

	//adjust the value of 1/w so the pixels that are closer to the camera with smaller values
	interpolated_reciprocal_w = 1.0 - interpolated_reciprocal_w;

	//only draw the pixel if the depth value is less than the one perviously stored in the z-buffer
	if (interpolated_reciprocal_w < z_buffer[(window_width * y) + x]) {
		draw_pixel(x, y, texture[(texture_width * tex_y) + tex_x]);

		//update the z-buffer value with the 1/w value of this current pixel
		z_buffer[(window_width * y) + x] = interpolated_reciprocal_w;
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
//////////////////////////////////////////////////////////////////////////////////
void draw_filled_triangle(
	int x0, int y0, float z0, float w0, 
	int x1, int y1, float z1, float w1, 
	int x2, int y2, float z2, float w2, 
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

			if (x_end < x_start)
			{
				int_swap(&x_start, &x_end); //swap the position if x_end at the left of x_start
			}

			for (int x = x_start; x < x_end; x++)
			{
				//Draw our pixel with the color from left to right
				draw_triangle_pixel(x,y,point_a,point_b,point_c,color);
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
				draw_triangle_pixel(x, y, point_a, point_b, point_c, color);

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
	uint32_t* texture) {

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
				draw_triangle_texel(x, y, texture, point_a, point_b, point_c, a_uv, b_uv, c_uv);
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
				draw_triangle_texel(x, y, texture, point_a, point_b, point_c, a_uv, b_uv, c_uv);

			}
		}
	}



}