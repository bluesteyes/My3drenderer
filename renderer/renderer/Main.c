#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <sdl.h>
#include "array.h"
#include "upng.h"
#include "camera.h"
#include "clipping.h"
#include "display.h"
#include "matrix.h"
#include "vector.h"
#include "triangle.h"
#include "texture.h"
#include "mesh.h"
#include "light.h"


//////////////////////////////////////////////////////////////////////////////////
// Global variables for excution status and game loop
//////////////////////////////////////////////////////////////////////////////////

bool is_running = false;
int previous_frame_time = 0;
float delta_time = 0;

//////////////////////////////////////////////////////////////////////////////////
// Array of triangles that should be rendered frame by frame
//////////////////////////////////////////////////////////////////////////////////

#define MAX_TRIANGLES_PER_MESH 10000
triangle_t triangles_to_render[MAX_TRIANGLES_PER_MESH];
int num_triangles_to_render = 0;

//////////////////////////////////////////////////////////////////////////////////
// Declaration of our global transformation matrices
//////////////////////////////////////////////////////////////////////////////////

mat4_t world_matrix;
mat4_t view_matrix;
mat4_t proj_matrix;


//////////////////////////////////////////////////////////////////////////////////
// setup functions to initialize variables and objects
//////////////////////////////////////////////////////////////////////////////////
void setup()
{
	//initialize light direction
	init_light(vect3_new(0, 0, 1));

	//initialize camera
	vect3_t position = vect3_new(0, 0, 0);
	vect3_t direction = vect3_new(0, 0, 1);
	vect3_t fwd_velocity = vect3_new(0, 0, 0);
	float yaw = 0.0;
	float pitch = 0.0;
	init_camera(position, direction, fwd_velocity, yaw, pitch);


	//Initialize render mode and triangle culling method
	set_render_method(RENDER_WIRE);
	set_cull_method(CULL_BACKFACE);

	//Initialize the perspective projection matrix
	float aspect_y = (float)(get_window_height()) / (float)(get_window_width());
	float aspect_x = (float)(get_window_width()) / (float)(get_window_height());
	float fov_y = 3.1415926 / 3.0; //the same as 60 degree
	float fov_x = atan(tan(fov_y/2) * aspect_x) * 2.0;

	float z_near = 1.0;
	float z_far = 20;
	proj_matrix = mat4_make_perspective(fov_y, aspect_y, z_near, z_far);

	//Initialize the frustum plane with a point and normal
	init_frustum_planes(fov_x, fov_y, z_near, z_far);

	//Loads the vertex and faces values for the mesh data structure
	//load_cube_mesh_data();
	
    load_obj_mesh_data("./assets/cube.obj");
	//load the texture information from an external png file
	load_png_texture_data("./assets/cube.png");
}

//////////////////////////////////////////////////////////////////////////////////
// Poll system events and handle keyboard event
//////////////////////////////////////////////////////////////////////////////////
void process_input(void)
{
	SDL_Event event;
	while (SDL_PollEvent(&event)) {

		switch (event.type){
		case SDL_QUIT:
			is_running = false;
			break;
		case SDL_KEYDOWN:
			if (event.key.keysym.sym == SDLK_ESCAPE){
				is_running = false;
				break;
			}
			if (event.key.keysym.sym == SDLK_1){
				set_render_method(RENDER_WIRE_VERTEX);
				break;
			}
			if (event.key.keysym.sym == SDLK_2){
				set_render_method(RENDER_WIRE);
				break;
			}
			if (event.key.keysym.sym == SDLK_3){
				set_render_method(RENDER_FILL_TRIANGLE);
			}
			if (event.key.keysym.sym == SDLK_4){
				set_render_method(RENDER_FILL_TRIANGLE_WIRE);
				break;
			}
			if (event.key.keysym.sym == SDLK_5){
				set_render_method(RENDER_TEXTURED);
				break;
			}
			if (event.key.keysym.sym == SDLK_6){
				set_render_method(RENDER_TEXTURED_WIRE);
				break;
			}
			if (event.key.keysym.sym == SDLK_7){
				set_cull_method(CULL_BACKFACE);
				break;
			}
			if (event.key.keysym.sym == SDLK_8){
				set_cull_method(CULL_NONE);
				break;
			}
			
			if (event.key.keysym.sym == SDLK_9) {
				set_camera_position_y(get_camera_position().y + 3.0 * delta_time);
				break;
			
			}
			if (event.key.keysym.sym == SDLK_0) {
				set_camera_position_y(get_camera_position().y - 3.0 * delta_time);
				break;
			}
			if (event.key.keysym.sym == SDLK_F7) {
				set_camera_position_x(get_camera_position().x + 3.0 * delta_time);
				break;
			}
			if (event.key.keysym.sym == SDLK_F8) {
				set_camera_position_x(get_camera_position().x - 3.0 * delta_time);
				break;
			}
			if (event.key.keysym.sym == SDLK_F9) {
				set_camera_pitch(get_camera_pitch() + 1.0 * delta_time);
				break;
			}
			if (event.key.keysym.sym == SDLK_F10) {
				set_camera_pitch(get_camera_pitch() - 1.0 * delta_time);
				break;
			}

			if (event.key.keysym.sym == SDLK_LEFT) {
				set_camera_yaw(get_camera_yaw() + 1.0 * delta_time);
				
				break;
			}
			if (event.key.keysym.sym == SDLK_RIGHT) {
				set_camera_yaw(get_camera_yaw() - 1.0 * delta_time);
			
				break;
			}


			if (event.key.keysym.sym == SDLK_UP) {
				set_camera_fwd_velocity (vect3_mul(get_camera_direction(), 3.0 * delta_time));
				set_camera_position(vect3_add(get_camera_position(), get_camera_fwd_velocity()));
				break;
			}
			if (event.key.keysym.sym == SDLK_DOWN) {
				set_camera_fwd_velocity(vect3_mul(get_camera_direction(), 3.0 * delta_time));
				set_camera_position(vect3_sub(get_camera_position(), get_camera_fwd_velocity()));
				break;
			}
			break;
	}
	};
}

//////////////////////////////////////////////////////////////////////////////////
// Call update function every frame
//////////////////////////////////////////////////////////////////////////////////
void update(void)
{
	// this while loop will use 100% of CPU
	//while (!SDL_TICKS_PASSED(SDL_GetTicks(), previous_frame_time + FRAME_TARGET_TIME))
	
	//Wait some time until reaching the target frame time in miliseconds
	int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - previous_frame_time);

	//Only delay excution if running too fast
	if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME ){
		SDL_Delay(time_to_wait);
	}

	//get a delta time factor converted to seconds to be used to update our game object
	delta_time = (SDL_GetTicks() - previous_frame_time) / 1000.0; //-> 1/framerate
	
	previous_frame_time = SDL_GetTicks();

	//Initialize the counter of triangles to render for the current frame
	num_triangles_to_render = 0;

	
	//Change the mesh scale/rotation values per animation frame
	mesh.rotation.x += 0.0 * delta_time;
	mesh.rotation.y += 0.0 * delta_time;
	mesh.rotation.z += 0.0 * delta_time;
	//mesh.scale.x += 0;
	//mesh.scale.y += 0;
	//mesh.translation.x += 0.000;
	mesh.translation.z = 5;


	//initialize the target looking at the positive z-axis
	vect3_t target = { 0, 0, 1 };

	//apply yaw rotation to vectors
	mat4_t camera_yaw_rotation = mat4_make_rotation_y(get_camera_yaw());
	mat4_t camera_pitch_rotation = mat4_make_rotation_x(get_camera_pitch());
	//camera.direction = vect3_from_vect4(mat4_mul_vect4(camera_yaw_rotation, vect4_from_vect3(target)));
	set_camera_direction(vect3_from_vect4(mat4_mul_vect4(camera_pitch_rotation, 
		mat4_mul_vect4(camera_yaw_rotation, vect4_from_vect3(target)))));

	//offset the camera position in the direction where the camera is pointing at
	target = vect3_add(get_camera_position(), get_camera_direction());
	vect3_t up_direction = { 0, 1, 0 };

	//create view matrix
	view_matrix = mat4_look_at(get_camera_position(), target, up_direction);

	//create  scale, rotation and translation matrix that will be used to multiply the mesh vertices
	mat4_t scale_matrix = mat4_make_scale(mesh.scale.x, mesh.scale.y, mesh.scale.z);
	mat4_t rotation_matrix_x = mat4_make_rotation_x(mesh.rotation.x);
	mat4_t rotation_matrix_y = mat4_make_rotation_y(mesh.rotation.y);
	mat4_t rotation_matrix_z = mat4_make_rotation_z(mesh.rotation.z);
	mat4_t translation_matrix = mat4_make_translation(mesh.translation.x,
		mesh.translation.y, mesh.translation.z);

	//Loop all triangle faces of object mesh
	int num_faces = array_length(mesh.faces);

	for (int i = 0; i < num_faces; i++){
	
		face_t mesh_face = mesh.faces[i];
		
		vect3_t face_vertices[3];
		face_vertices[0] = mesh.vertices[mesh_face.a ];
		face_vertices[1] = mesh.vertices[mesh_face.b ];
		face_vertices[2] = mesh.vertices[mesh_face.c ];

		vect4_t transformed_vertices[3];

		//Loop through all three vertices of this current face and apply transformation
		for (int j = 0; j < 3; j++){
			vect4_t transformed_vertex = vect4_from_vect3 (face_vertices[j]);

			//create a world matrix combining scale, rotation and translation
			world_matrix = mat4_identity();

			//multiply all matrices and load the world matrix
			//order matters: first scale, next rotate, then translate >>> [T]*[R]*[S]*v
			world_matrix = mat4_mul_mat4(scale_matrix, world_matrix);
			world_matrix = mat4_mul_mat4(rotation_matrix_z, world_matrix);
			world_matrix = mat4_mul_mat4(rotation_matrix_y, world_matrix);
			world_matrix = mat4_mul_mat4(rotation_matrix_x, world_matrix);
			world_matrix = mat4_mul_mat4(translation_matrix, world_matrix);

			//multiply the world matrix with the original vector
			transformed_vertex = mat4_mul_vect4(world_matrix, transformed_vertex);

			//multiply the view matrix with the world matrix transformed vector to transform the scene to camera space
			transformed_vertex = mat4_mul_vect4(view_matrix, transformed_vertex);

			//Save transformed vertex in the array of transformed vertices
			transformed_vertices[j] = transformed_vertex;
		}
			
		//backface culling to see if the current face should be projected	
		vect3_t vector_a = vect3_from_vect4(transformed_vertices[0]); /*  A  */
		vect3_t vector_b = vect3_from_vect4(transformed_vertices[1]); /* / \ */
		vect3_t vector_c = vect3_from_vect4(transformed_vertices[2]); /*C---B*/

		//get the vector subtraction of (B - A) and (C - B)
		vect3_t vector_ab = vect3_sub(vector_b, vector_a);
		vect3_t vector_ac = vect3_sub(vector_c, vector_a);

		vect3_normalize(&vector_ab);
		vect3_normalize(&vector_ac);

		//compute the face normal (using cross product to find perpendicular)
		vect3_t normal_vect = vect3_cross(vector_ab, vector_ac);

		//normalize the face normal vector
		vect3_normalize(&normal_vect);

		//find the vector between a point in the triangle and the camera origin
		vect3_t origin = {0, 0, 0};
		vect3_t cam_ray = vect3_sub(origin, vector_a);

		//calculate how align the camera ray is with the face normal (using dot product)
		float dot_normal_cam = vect3_dot(normal_vect, cam_ray);

		if (is_cull_backface()){
			//bypassing the triangles that looking away from the camera
			if (dot_normal_cam < 0){
				continue;
			}
		}

		//create a polygon from the original transformed triangle to be clipped
		polygon_t polygon = polygon_from_triangle(
			vect3_from_vect4(transformed_vertices[0]),
			vect3_from_vect4(transformed_vertices[1]),
			vect3_from_vect4(transformed_vertices[2]),
			mesh_face.a_uv,
			mesh_face.b_uv,
			mesh_face.c_uv
		);

		//clip the polygon and returns a new polygon with potential new vertices
		clip_polygon(&polygon);
		
		//break the clipped polygon apart back into the individual triangles
		triangle_t triangles_after_clipping[MAX_NUM_POLY_TRIANGLES];
		int num_triangles_after_clipping = 0;

		triangles_from_polygon(&polygon, triangles_after_clipping, &num_triangles_after_clipping);

		
		//loops all the assembled triangles after clipping
		for (int t = 0; t < num_triangles_after_clipping; t++){

			triangle_t triangle_after_clipping = triangles_after_clipping[t];
			vect4_t projected_points[3];
			//Loop through all three vertices of this current face and apply projection
			for (int j = 0; j < 3; j++) {
				//project the current vertex using a perspective projection matrix
				projected_points[j] = mat4_mul_vect4(proj_matrix, triangle_after_clipping.points[j]);

				//TODO:perform perspective divide
				if (projected_points[j].w != 0) {
					projected_points[j].x /= projected_points[j].w;
					projected_points[j].y /= projected_points[j].w;
					projected_points[j].z /= projected_points[j].w;
				}

				//invert the y value to account for flipped screen y coordinate
				projected_points[j].y *= -1;

				//scale into the viewport
				projected_points[j].x *= (get_window_width() / 2.0);
				projected_points[j].y *= (get_window_height() / 2.0);

				//translate the projected points to the middle of the screen
				projected_points[j].x += (get_window_width() / 2.0);
				projected_points[j].y += (get_window_height() / 2.0);

			}

			//calculate how align the light direction is with the face normal (using dot product)
			float light_intensity_factor = -vect3_dot(normal_vect, get_light_direction());

			// Calculate the triangle color based on the light angle	
			uint32_t  triangle_color = light_apply_intensity(mesh_face.color, light_intensity_factor);

			// save the projected 2d vertex in the array of projected triangle points
			triangle_t triangle_to_render = {
				.points = {
					{projected_points[0].x, projected_points[0].y, projected_points[0].z, projected_points[0].w},
					{projected_points[1].x, projected_points[1].y, projected_points[1].z, projected_points[1].w},
					{projected_points[2].x, projected_points[2].y, projected_points[2].z, projected_points[2].w}
				},
				.texcoords = {
					{triangle_after_clipping.texcoords[0].u, triangle_after_clipping.texcoords[0].v},
					{triangle_after_clipping.texcoords[1].u, triangle_after_clipping.texcoords[1].v},
					{triangle_after_clipping.texcoords[2].u, triangle_after_clipping.texcoords[2].v}
				},
				.color = triangle_color,
			};

			//save the projected triagnle in the array of triangles to render
			if (num_triangles_to_render < MAX_TRIANGLES_PER_MESH) {
				triangles_to_render[num_triangles_to_render] = triangle_to_render;
				num_triangles_to_render++;
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////
// Render function to draw objects on the display
//////////////////////////////////////////////////////////////////////////////////
void render(void){

	//clear all the arrays to get ready for the next frame
	clear_color_buffer(0xFF000000);
	clear_z_buffer();

	draw_grid();


	//loop all projected triangles and render them
	for (int i = 0; i < num_triangles_to_render; i++){
		triangle_t triangle = triangles_to_render[i];


		//draw filled triangle
		if (should_render_fill_triangle()){
				
			draw_filled_triangle(
				triangle.points[0].x, triangle.points[0].y, triangle.points[0].z, triangle.points[0].w, //VERTEX A
				triangle.points[1].x, triangle.points[1].y, triangle.points[1].z, triangle.points[1].w, //VERTEX B
				triangle.points[2].x, triangle.points[2].y, triangle.points[2].z, triangle.points[2].w, //VERTEX C

				triangle.color
			);
		}
		//draw textureed triangle
		if (should_render_texture_triangle()){
			draw_textured_triangle(
				triangle.points[0].x, triangle.points[0].y, triangle.points[0].z, triangle.points[0].w, triangle.texcoords[0].u, triangle.texcoords[0].v, //VERTEX A
				triangle.points[1].x, triangle.points[1].y, triangle.points[1].z, triangle.points[1].w, triangle.texcoords[1].u, triangle.texcoords[1].v, //VERTEX B
				triangle.points[2].x, triangle.points[2].y, triangle.points[2].z, triangle.points[2].w, triangle.texcoords[2].u, triangle.texcoords[2].v, //VERTEX C
				mesh_texture
			);
			
		}

		//draw triangle wireframe
		if (should_render_wireframe()){
			draw_triangle(
				triangle.points[0].x, triangle.points[0].y, //VERTEX A
				triangle.points[1].x, triangle.points[1].y, //VERTEX B
				triangle.points[2].x, triangle.points[2].y, //VERTEX C
				0xFFFFFFFF
			);
		}
		
		//draw vertex of triangle
		if (should_render_wire_vertex()){
			draw_rect(triangle.points[0].x - 3, triangle.points[0].y - 3, 6, 6, 0xFFFF0000); //draw vertex a
			draw_rect(triangle.points[1].x - 3, triangle.points[1].y - 3, 6, 6, 0xFFFF0000); //draw vertex b
			draw_rect(triangle.points[2].x - 3, triangle.points[2].y - 3, 6, 6, 0xFFFF0000); //draw vertex c
		}	
	}

	//finally draw the color buffer to the SDL window
	render_color_buffer();
	
	
}

//////////////////////////////////////////////////////////////////////////////////
// Free the memory that was dynamically allocated by the program
//////////////////////////////////////////////////////////////////////////////////
void free_resource(void){
	upng_free(png_texture);
	array_free(mesh.vertices);
	array_free(mesh.faces);
}

int main(int argc, char* args[]){	
	is_running = initialize_window();

	setup();
	
	while(is_running){
		process_input();
		update();
		render();
	}

	destroy_window();
	free_resource();

	return 0;
}