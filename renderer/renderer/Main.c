#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL.h>
#include "array.h"
#include "upng.h"
#include "camera.h"
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

#define MAX_TRIANGLES_PER_MESH 200000
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
	//Initialize render mode and triangle culling method
	render_method = RENDER_WIRE;
	cull_method = CULL_BACKFACE;

	// Allocate the required memory in bytes to hold the color buffer
	color_buffer = (uint32_t*)malloc(sizeof(uint32_t) * window_width * window_height);
	z_buffer = (float*)malloc(sizeof(float) * window_width * window_height);

	// Creating a SDL texture that is used to display the color buffer
	color_buffer_texture = SDL_CreateTexture(
		renderer,
		SDL_PIXELFORMAT_RGBA32,
		SDL_TEXTUREACCESS_STREAMING,
		window_width,
		window_height
	);

	//Initialize the perspective projection matrix
	float fov = M_PI / 3; //the same as 60 degree
	float aspect = (float)(window_height) / (float)(window_width);
	float znear = 0.1;
	float zfar = 100;
	proj_matrix = mat4_make_perspective(fov, aspect, znear, zfar);

	//manually load the hardcoded texture data from the static array
	//mesh_texture = (uint32_t*)REDBRICK_TEXTURE;
	//texture_width = 64;
	//texture_height = 64;

	//Loads the vertex and faces values for the mesh data structure
	//load_cube_mesh_data();
	load_png_texture_data("./assets/f22.png");
    load_obj_mesh_data("./assets/f22.obj");
}

//////////////////////////////////////////////////////////////////////////////////
// Poll system events and handle keyboard event
//////////////////////////////////////////////////////////////////////////////////
void process_input(void)
{
	SDL_Event event;
	SDL_PollEvent(&event);

	switch (event.type)
	{
		case SDL_QUIT:
			is_running = false;
			break;
		case SDL_KEYDOWN:
			if (event.key.keysym.sym == SDLK_ESCAPE)
				is_running = false;
			if (event.key.keysym.sym == SDLK_1)
				render_method = RENDER_WIRE_VERTEX;
			if (event.key.keysym.sym == SDLK_2)
				render_method = RENDER_WIRE;
			if (event.key.keysym.sym == SDLK_3)
				render_method = RENDER_FILL_TRIANGLE;
			if (event.key.keysym.sym == SDLK_4)
				render_method = RENDER_FILL_TRIANGLE_WIRE;
			if (event.key.keysym.sym == SDLK_5)
				render_method = RENDER_TEXTURED;
			if (event.key.keysym.sym == SDLK_6)
				render_method = RENDER_TEXTURED_WIRE;
			if (event.key.keysym.sym == SDLK_7) {
				cull_method = CULL_BACKFACE;
			}	
			if (event.key.keysym.sym == SDLK_8) {
				cull_method = CULL_NONE;
			}
			if (event.key.keysym.sym == SDLK_9) {
				camera.position.y += 3.0 * delta_time;
			}
			if (event.key.keysym.sym == SDLK_0) {
				camera.position.y -= 3.0 * delta_time;
			}

			if (event.key.keysym.sym == SDLK_a) {
				camera.position.x += 3.0 * delta_time;
			}
			if (event.key.keysym.sym == SDLK_d) {
				camera.position.x -= 3.0 * delta_time;
			}
			if (event.key.keysym.sym == SDLK_LEFT) {
				camera.yaw += 1.0 * delta_time;
			}
			if (event.key.keysym.sym == SDLK_RIGHT) {
				camera.yaw -= 1.0 * delta_time;
			}
			if (event.key.keysym.sym == SDLK_UP) {
				camera.forward_velocity = vect3_mul(camera.direction, 3.0 * delta_time);
				camera.position = vect3_add(camera.position, camera.forward_velocity);
			}
			if (event.key.keysym.sym == SDLK_DOWN) {
				camera.forward_velocity = vect3_mul(camera.direction, 3.0 * delta_time);
				camera.position = vect3_sub(camera.position, camera.forward_velocity);
			}


			break;
	}
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

	//change the animation position per animation frame
	//camera.position.x += 0.5 * delta_time;
	//camera.position.y += 0.5 * delta_time;
	//camera.position.z += 0.5 * delta_time;
	//camera.forward_velocity = vect3_mul(camera.direction, 5 * delta_time);
	//camera.position = vect3_sub(camera.position, camera.forward_velocity);


	//initialize the target looking at the positive z-axis
	vect3_t target = { 0, 0, 1 };

	//apply yaw rotation to vectors
	mat4_t camera_yaw_rotation = mat4_make_rotation_y(camera.yaw);
	camera.direction = vect3_from_vect4(mat4_mul_vect4(camera_yaw_rotation, vect4_from_vect3(target)));


	//offset the camera position in the direction where the camera is pointing at
	target = vect3_add(camera.position, camera.direction);
	vect3_t up_direction = { 0, 1, 0 };

	//create view matrix
	view_matrix = mat4_look_at(camera.position, target, up_direction);

	//create  scale, rotation and translation matrix that will be used to multiply the mesh vertices
	mat4_t scale_matrix = mat4_make_scale(mesh.scale.x, mesh.scale.y, mesh.scale.z);
	mat4_t rotation_matrix_x = mat4_make_rotation_x(mesh.rotation.x);
	mat4_t rotation_matrix_y = mat4_make_rotation_y(mesh.rotation.y);
	mat4_t rotation_matrix_z = mat4_make_rotation_z(mesh.rotation.z);
	mat4_t translation_matrix = mat4_make_translation(mesh.translation.x,
		mesh.translation.y, mesh.translation.z);

	//Loop all triangle faces of cube mesh
	int num_faces = array_length(mesh.faces);

	for (int i = 0; i < num_faces; i++)
	{
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


		if (cull_method == CULL_BACKFACE){
			//bypassing the triangles that looking away from the camera
			if (dot_normal_cam < 0){
				continue;
			}
		}


		vect4_t projected_points[3];
		//Loop through all three vertices of this current face and apply projection
		for (int j = 0; j < 3; j++){
			//project the current vertex
			//projected_point[j] = project(vect3_from_vect4 (transformed_vertices[j]));
			projected_points[j] = mat4_mul_vect4_project(proj_matrix, transformed_vertices[j]);

			//invert the y value to account for flipped screen y coordinate
			projected_points[j].y *= -1;

			//scale into the viewport
			projected_points[j].x *= (window_width / 2.0);
			projected_points[j].y *= (window_height / 2.0);

			//translate the projected points to the middle of the screen
			projected_points[j].x += (window_width / 2.0);
			projected_points[j].y += (window_height / 2.0);		

			
		}


		//calculate how align the light direction is with the face normal (using dot product)
		float light_intensity_factor = -vect3_dot(normal_vect, light.direction);

		// Calculate the triangle color based on the light angle
		
		uint32_t  triangle_color = light_apply_intensity(mesh_face.color, light_intensity_factor);

		// save the projected 2d vertex in the array of projected triangle points
		triangle_t projected_triangle = {
			.points = {
				{projected_points[0].x, projected_points[0].y, projected_points[0].z, projected_points[0].w},
				{projected_points[1].x, projected_points[1].y, projected_points[1].z, projected_points[1].w},
				{projected_points[2].x, projected_points[2].y, projected_points[2].z, projected_points[2].w}
			},
			.texcoords = {
				{mesh_face.a_uv.u, mesh_face.a_uv.v},
				{mesh_face.b_uv.u, mesh_face.b_uv.v},
				{mesh_face.c_uv.u, mesh_face.c_uv.v}

			},
			.color = triangle_color,
		};

		//save the projected triagnle in the array of triangles to render
		if (num_triangles_to_render < MAX_TRIANGLES_PER_MESH){
			triangles_to_render[num_triangles_to_render] = projected_triangle;
			num_triangles_to_render++;
		}
		

	}
}

//////////////////////////////////////////////////////////////////////////////////
// Render function to draw objects on the display
//////////////////////////////////////////////////////////////////////////////////
void render(void)
{
	draw_grid();


	//loop all projected triangles and render them
	for (int i = 0; i < num_triangles_to_render; i++){
		triangle_t triangle = triangles_to_render[i];


		//draw filled triangle
		if (render_method == RENDER_FILL_TRIANGLE || render_method == RENDER_FILL_TRIANGLE_WIRE){
				
			draw_filled_triangle(
				triangle.points[0].x, triangle.points[0].y, triangle.points[0].z, triangle.points[0].w, //VERTEX A
				triangle.points[1].x, triangle.points[1].y, triangle.points[1].z, triangle.points[1].w, //VERTEX B
				triangle.points[2].x, triangle.points[2].y, triangle.points[2].z, triangle.points[2].w, //VERTEX C

				triangle.color
			);
		}
		//draw textureed triangle
		if (render_method == RENDER_TEXTURED || render_method == RENDER_TEXTURED_WIRE ){
			draw_textured_triangle(
				triangle.points[0].x, triangle.points[0].y, triangle.points[0].z, triangle.points[0].w, triangle.texcoords[0].u, triangle.texcoords[0].v, //VERTEX A
				triangle.points[1].x, triangle.points[1].y, triangle.points[1].z, triangle.points[1].w, triangle.texcoords[1].u, triangle.texcoords[1].v, //VERTEX B
				triangle.points[2].x, triangle.points[2].y, triangle.points[2].z, triangle.points[2].w, triangle.texcoords[2].u, triangle.texcoords[2].v, //VERTEX C
				mesh_texture
			);
			
		}

		//draw triangle wireframe
		if (render_method == RENDER_WIRE || render_method == RENDER_WIRE_VERTEX || 
			render_method == RENDER_FILL_TRIANGLE_WIRE || render_method == RENDER_TEXTURED_WIRE){
			draw_triangle(
				triangle.points[0].x, triangle.points[0].y, //VERTEX A
				triangle.points[1].x, triangle.points[1].y, //VERTEX B
				triangle.points[2].x, triangle.points[2].y, //VERTEX C
				0xFFFFFFFF
			);

		}
		
		//draw vertex of triangle
		if (render_method == RENDER_WIRE_VERTEX){
			draw_rect(triangle.points[0].x - 3, triangle.points[0].y - 3, 6, 6, 0xFFFF0000); //draw vertex a
			draw_rect(triangle.points[1].x - 3, triangle.points[1].y - 3, 6, 6, 0xFFFF0000); //draw vertex b
			draw_rect(triangle.points[2].x - 3, triangle.points[2].y - 3, 6, 6, 0xFFFF0000); //draw vertex c
		}	
	}

	render_color_buffer();
	clear_color_buffer(0xFF000000);
	clear_z_buffer();
	SDL_RenderPresent(renderer);
}

//////////////////////////////////////////////////////////////////////////////////
// Free the memory that was dynamically allocated by the program
//////////////////////////////////////////////////////////////////////////////////
void free_resource(void){
	free(color_buffer);
	free(z_buffer);
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