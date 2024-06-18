#include <math.h>
#include "clipping.h"

#define NUM_PLANES 6
plane_t frustum_planes[NUM_PLANES];

float float_lerp(float a, float b, float t) {
	 return a + t * (b - a);
}

///////////////////////////////////////////////////////////////////////////////
// Frustum planes are defined by a point and a normal vector
///////////////////////////////////////////////////////////////////////////////
// Near plane   :  P=(0, 0, znear), N=(0, 0,  1)
// Far plane    :  P=(0, 0, zfar),  N=(0, 0, -1)
// Top plane    :  P=(0, 0, 0),     N=(0, -cos(fov/2), sin(fov/2))
// Bottom plane :  P=(0, 0, 0),     N=(0, cos(fov/2), sin(fov/2))
// Left plane   :  P=(0, 0, 0),     N=(cos(fov/2), 0, sin(fov/2))
// Right plane  :  P=(0, 0, 0),     N=(-cos(fov/2), 0, sin(fov/2))
///////////////////////////////////////////////////////////////////////////////
//
//           /|\
//         /  | | 
//       /\   | |
//     /      | |
//  P*|-->  <-|*|   ----> +z-axis
//     \      | |
//       \/   | |
//         \  | | 
//           \|/
//
///////////////////////////////////////////////////////////////////////////////
void init_frustum_planes(float fov_x, float fov_y, float z_near, float z_far) {

	float cos_half_fov_x = cos(fov_x / 2);
	float sin_half_fov_x = sin(fov_x / 2);
	float cos_half_fov_y = cos(fov_y / 2);
	float sin_half_fov_y = sin(fov_y / 2);

	frustum_planes[LEFT_FRUSTUM_PLANE].point = vect3_new(0, 0, 0);
	frustum_planes[LEFT_FRUSTUM_PLANE].normal = vect3_new(cos_half_fov_x, 0, sin_half_fov_x);

	frustum_planes[RIGHT_FRUSTUM_PLANE].point = vect3_new(0, 0, 0);
	frustum_planes[RIGHT_FRUSTUM_PLANE].normal = vect3_new(-cos_half_fov_x, 0, sin_half_fov_x);
	

	frustum_planes[TOP_FRUSTUM_PLANE].point = vect3_new(0, 0, 0);
	frustum_planes[TOP_FRUSTUM_PLANE].normal = vect3_new(0, -cos_half_fov_y, sin_half_fov_y);


	frustum_planes[BOTTOM_FRUSTUM_PLANE].point = vect3_new(0, 0, 0);
	frustum_planes[BOTTOM_FRUSTUM_PLANE].normal = vect3_new(0, cos_half_fov_y, sin_half_fov_y);
	
	
	frustum_planes[NEAR_FRUSTUM_PLANE].point = vect3_new(0, 0, z_near);
	frustum_planes[NEAR_FRUSTUM_PLANE].normal = vect3_new(0, 0, 1);


	frustum_planes[FAR_FRUSTUM_PLANE].point = vect3_new(0, 0, z_far);
	frustum_planes[FAR_FRUSTUM_PLANE].normal = vect3_new(0, 0, -1);;

} 

polygon_t polygon_from_triangle(vect3_t v0, vect3_t v1, vect3_t v2, tex2_t t0, tex2_t t1, tex2_t t2){

	polygon_t polygon = {
		.vertices = {v0, v1, v2},
		.texcoords = {t0, t1, t2},
		.num_vertices = 3,
	};
	return polygon;
}

void triangles_from_polygon(polygon_t* polygon, triangle_t triangles[], int* num_triangles) {
	for (int i = 0; i < polygon->num_vertices - 2; i++){
		int index0 = 0;
		int index1 = i + 1;
		int index2 = i + 2;

		triangles[i].points[0] = vect4_from_vect3(polygon->vertices[index0]);
		triangles[i].points[1] = vect4_from_vect3(polygon->vertices[index1]);
		triangles[i].points[2] = vect4_from_vect3(polygon->vertices[index2]);

		triangles[i].texcoords[0] = polygon->texcoords[index0];
		triangles[i].texcoords[1] = polygon->texcoords[index1];
		triangles[i].texcoords[2] = polygon->texcoords[index2];
	}
	
	*num_triangles = polygon->num_vertices - 2;
}

void clip_polygon_against_plane(polygon_t* polygon, int plane ){ // parameter polygon work as out parameter
	
	vect3_t plane_point = frustum_planes[plane].point;
	vect3_t plane_normal = frustum_planes[plane].normal;

	//declare a static array of inside vertices that will be part of the final polygon returned via parameter
	vect3_t inside_vertices[MAX_NUM_POLY_VERTICES];
	tex2_t inside_texcoords[MAX_NUM_POLY_VERTICES];
	int num_inside_vertices = 0;

	//start the current vertex with the first polygon vertex and previous vertex with the last polygon vertex
	vect3_t* current_vertex = &polygon->vertices[0];
	tex2_t* current_texcoord = &polygon->texcoords[0];

	vect3_t* previous_vertex = &polygon->vertices[polygon->num_vertices - 1];
	tex2_t* previous_texcoord = &polygon->texcoords[polygon->num_vertices - 1];

	//calculate the dot product of the current and previous polygon vertex
	float current_dot = 0;
	float previous_dot = vect3_dot(plane_normal, vect3_sub(*previous_vertex, plane_point));


	//loop all the vertices while the current is different than the last one
	while (current_vertex != &polygon->vertices[polygon->num_vertices]){
		current_dot = vect3_dot(plane_normal, vect3_sub(*current_vertex, plane_point));
		
		//if we changed from inside to outside or outside to inside
		if (current_dot * previous_dot < 0){
			
			//TODO: find the interpolation factor t
			float interpolation_factor = previous_dot / (previous_dot - current_dot);

			//find the intersection point I = Q1 + t(Q2 - Q1) ---> use the lerp fromula
			vect3_t intersection_point = {
				.x = float_lerp(previous_vertex->x, current_vertex->x, interpolation_factor),
				.y = float_lerp(previous_vertex->y, current_vertex->y, interpolation_factor),
				.z = float_lerp(previous_vertex->z, current_vertex->z, interpolation_factor)
			};

			//use the lerp formula to get the interpolated texcoords
			tex2_t interpolated_texcoord = {
				.u = float_lerp(previous_texcoord->u, current_texcoord->u, interpolation_factor),
				.v = float_lerp(previous_texcoord->v, current_texcoord->v, interpolation_factor)
			};

			//insert the intersection point to the list of "inside vertices"
			inside_vertices[num_inside_vertices] = vect3_clone(&intersection_point);
			inside_texcoords[num_inside_vertices] = tex2_clone(&interpolated_texcoord);
			num_inside_vertices++;
		}

		//current vertex is inside the plane
		if (current_dot > 0){
			//insert the current vertex to the list of "inside vertices"
			inside_vertices[num_inside_vertices] = vect3_clone(current_vertex);

			//insert the current texcoord to the list of "inside texcoords"
			inside_texcoords[num_inside_vertices] = tex2_clone(current_texcoord);

			num_inside_vertices++;
		}

		//move to the next vertex and next texcoord
		previous_dot = current_dot;
		previous_vertex = current_vertex;
		previous_texcoord = current_texcoord;
		current_vertex++; //pointer move to next address
		current_texcoord++;

	}

	//At the end, copy the list of inside vertices into the destination polygon (out parameter)
	for (int i = 0; i < num_inside_vertices; i++){
		polygon->vertices[i] = vect3_clone(&inside_vertices[i]);
		polygon->texcoords[i] = tex2_clone(&inside_texcoords[i]);
	}
	polygon->num_vertices = num_inside_vertices;
}

void clip_polygon(polygon_t* polygon) {
	clip_polygon_against_plane(polygon, LEFT_FRUSTUM_PLANE);
	clip_polygon_against_plane(polygon, RIGHT_FRUSTUM_PLANE);
	clip_polygon_against_plane(polygon, TOP_FRUSTUM_PLANE);
	clip_polygon_against_plane(polygon, BOTTOM_FRUSTUM_PLANE);
	clip_polygon_against_plane(polygon, NEAR_FRUSTUM_PLANE);
	clip_polygon_against_plane(polygon, FAR_FRUSTUM_PLANE);


}

