#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL.h>
#include "array.h"
#include "display.h"
#include "vector.h"
#include "mesh.h"

//////////////////////////////////////////////////////////////////////////////////
// Array of triangles that should be rendered frame by frame
//////////////////////////////////////////////////////////////////////////////////

triangle_t* triangles_to_render = NULL;

//////////////////////////////////////////////////////////////////////////////////
// Declare an array of vectors/points
//////////////////////////////////////////////////////////////////////////////////
//const int N_POINTS = 9 * 9 * 9;
//vect3_t cube_points[9 * 9 * 9]; // 9x9x9 cube
//vect2_t projected_points[9 * 9 * 9];

//////////////////////////////////////////////////////////////////////////////////
// Global variables for excution status and game loop
//////////////////////////////////////////////////////////////////////////////////

int fov_factor = 1024;
vect3_t camera_position = {.x = 0, .y= 0, .z = 0};
//vect3_t cube_rotation = {.x = 0, .y = 0, .z = 0};


bool is_running = false;
int previous_frame_time = 0;


//////////////////////////////////////////////////////////////////////////////////
// setup functions to initialize variables and objects
//////////////////////////////////////////////////////////////////////////////////
void setup()
{
	// Allocate the required memory in bytes to hold the color buffer
	color_buffer = (uint32_t*)malloc(sizeof(uint32_t) * window_width * window_height);

	// Creating a SDL texture that is used to display the color buffer
	color_buffer_texture = SDL_CreateTexture(
		renderer,
		SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_STREAMING,
		window_width,
		window_height
	);
	//Loads the cube values in the mesh data structure
	//load_cube_mesh_data();

	load_obj_mesh_data("./assets/cube.obj");

	//Start loading my array of vectors
	//From -1 to 1 (in this 9*9*9 cube)

	/*int point_count = 0;

	for (float x = -1; x <= 1; x += 0.25)
	{
		for (float y = -1; y <= 1; y += 0.25)
		{
			for (float z = -1; z <= 1; z += 0.25)
			{
				vect3_t new_points = { .x = x, .y = y, .z = z };
				cube_points[point_count++] = new_points;
			}
		}
	}*/

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
			break;
			
	}
}

//////////////////////////////////////////////////////////////////////////////////
// Function that recieve a 3d vector and return a projected 2d point
//////////////////////////////////////////////////////////////////////////////////
vect2_t project(vect3_t point)
{
	vect2_t projected_point = {
		.x = fov_factor * point.x / point.z,
		.y = fov_factor * point.y / point.z,
	};

	return projected_point;
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
	if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME )
	{
		SDL_Delay(time_to_wait);
	}

	//Initialize the array of triangles to render
	triangles_to_render = NULL;

	previous_frame_time = SDL_GetTicks();

	mesh.rotation.x += 0.03;
	mesh.rotation.y += 0.03;
	mesh.rotation.z += 0.03;
	
	//Loop all triangle faces of cube mesh
	int num_faces = array_length(mesh.faces);

	for (int i = 0; i < num_faces; i++)
	{
		face_t mesh_face = mesh.faces[i];
		
		vect3_t face_vertices[3];
		face_vertices[0] = mesh.vertices[mesh_face.a - 1];
		face_vertices[1] = mesh.vertices[mesh_face.b - 1];
		face_vertices[2] = mesh.vertices[mesh_face.c - 1];
	
		triangle_t projected_triangle;

		vect3_t transformed_vertices[3];

		//Loop through all three vertices of this current face and apply transformation
		for (int j = 0; j < 3; j++)
		{
			vect3_t transformed_vertex = face_vertices[j];

			transformed_vertex = vect3_rotate_x(transformed_vertex, mesh.rotation.x);
			transformed_vertex = vect3_rotate_y(transformed_vertex, mesh.rotation.y);
			transformed_vertex = vect3_rotate_z(transformed_vertex, mesh.rotation.z);

			//Translate the points away from the camera through z axis
			transformed_vertex.z += camera_position.z + 5;

			//Save transformed vertex in the array of transformed vertices
			transformed_vertices[j] = transformed_vertex;
		}
		
		
		//check backface culling

		vect3_t vector_a = transformed_vertices[0]; /*  A  */
		vect3_t vector_b = transformed_vertices[1]; /* / \ */
		vect3_t vector_c = transformed_vertices[2]; /*C---B*/

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
		vect3_t cam_ray = vect3_sub(camera_position, vector_a);

		//calculate how align the camera ray is with the face normal (using dot product)
		float dot_normal_cam = vect3_dot(normal_vect, cam_ray);

		//bypassing the triangles that looking away from the camera
		if (dot_normal_cam < 0)
			continue;
		
		
		//Loop through all three vertices of this current face and apply projection
		for (int j = 0; j < 3; j++)
		{
			//project the current vertex
			vect2_t projected_point = project(transformed_vertices[j]);

			//scale and translated the projected points to the middle of the screen
			projected_point.x += (window_width / 2);
			projected_point.y += (window_height / 2);


			//save the projected 2d vertex in the array of projected triangle points
			projected_triangle.points[j] = projected_point;
		}

		//save the projected triagnle in the array of triangles to render
		//triangles_to_render[i] = projected_triangle;
		array_push(triangles_to_render, projected_triangle);
		

		

		

	}

	//for (int i = 0; i < N_POINTS; i++)
	//{
	//	vect3_t point = cube_points[i];

	//	vect3_t transformed_point = vect3_rotate_x(point, cube_rotation.x);
	//	transformed_point = vect3_rotate_y(transformed_point, cube_rotation.y);
	//	transformed_point = vect3_rotate_z(transformed_point, cube_rotation.z);

	//	//Translate the points away from the camera through z axis
	//	transformed_point.z  -= camera_position.z;

	//	//Project the current point
	//	vect2_t projected_point = project(transformed_point);

	//	//Save the projected 2d vector in the array of projected points
	//	projected_points[i] = projected_point;
	//	
	//}
}

//////////////////////////////////////////////////////////////////////////////////
// Render function to draw objects on the display
//////////////////////////////////////////////////////////////////////////////////
void render(void)
{

	draw_grid();

	int num_triangles = array_length(triangles_to_render);

	//Loop all projected triangles and render them
	for (int i = 0; i < num_triangles; i++)
	{
		triangle_t triangle = triangles_to_render[i];

		//draw vertex of triangle
		draw_rect(triangle.points[0].x, triangle.points[0].y, 3, 3, 0xFFFFFF00); //draw vertex a
		draw_rect(triangle.points[1].x, triangle.points[1].y, 3, 3, 0xFFFFFF00); //draw vertex b
		draw_rect(triangle.points[2].x, triangle.points[2].y, 3, 3, 0xFFFFFF00); //draw vertex c

		//draw unfilled triangle
		draw_triangle(
			triangle.points[0].x,
			triangle.points[0].y,
			triangle.points[1].x,
			triangle.points[1].y,
			triangle.points[2].x,
			triangle.points[2].y,
			0xFF00FF00
		
		);

	}

	

			
	
	//Loop all projected points and render them
	/*for (int i = 0; i < N_POINTS; i++)
	{
		vect2_t projected_point = projected_points[i];
		
		draw_rect(		
			projected_point.x + (window_width/2),
			projected_point.y + (window_height/2),
			4,
			4,
			0xffffff00
		);
	}*/

	//Clear the array of "triangle to render" every frame loop

	render_color_buffer();

	clear_color_buffer(0xFF000000);
	SDL_RenderPresent(renderer);
}

//////////////////////////////////////////////////////////////////////////////////
// Free the memory that was dynamically allocated by the program
//////////////////////////////////////////////////////////////////////////////////
void free_resource(void)
{
	free(color_buffer);
	array_free(mesh.vertices);
	array_free(mesh.faces);
}

int main(int argc, char* args[])
{	
	is_running = initialize_window();

	setup();
	
	while(is_running)
	{
		process_input();
		update();
		render();

	}

	destroy_window();
	free_resource();

	return 0;
}