#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL.h>
#include "display.h"
#include "vector.h"
#include "mesh.h"

//////////////////////////////////////////////////////////////////////////////////
// Declare an array of vectors/points
//////////////////////////////////////////////////////////////////////////////////
//const int N_POINTS = 9 * 9 * 9;
//vect3_t cube_points[9 * 9 * 9]; // 9x9x9 cube
//vect2_t projected_points[9 * 9 * 9];

triangle_t triangles_to_render[N_MESH_FACES];

int fov_factor = 640;
vect3_t camera_position = {.x = 0, .y= 0, .z = -5};
vect3_t cube_rotation = {.x = 0, .y = 0, .z = 0};


bool is_running = false;
int previous_frame_time = 0;



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

	previous_frame_time = SDL_GetTicks();

	cube_rotation.x += 0.01;
	cube_rotation.y += 0.01;
	cube_rotation.z += 0.01;
	
	//Loop all triangle faces of cube mesh
	for (int i = 0; i < N_MESH_FACES; i++)
	{
		face_t mesh_face = mesh_faces[i];
		
		vect3_t face_vertices[3];
		face_vertices[0] = mesh_vertices[mesh_face.a - 1];
		face_vertices[1] = mesh_vertices[mesh_face.b - 1];
		face_vertices[2] = mesh_vertices[mesh_face.c - 1];
	
		triangle_t projected_triangle;

		//Loop through all three vertices of this current face and apply transformation
		for (int j = 0; j < 3; j++)
		{
			vect3_t transformed_vertex = face_vertices[j];

			transformed_vertex = vect3_rotate_x(transformed_vertex, cube_rotation.x);
			transformed_vertex = vect3_rotate_y(transformed_vertex, cube_rotation.y);
			transformed_vertex = vect3_rotate_z(transformed_vertex, cube_rotation.z);
		
			//Translate the points away from the camera through z axis
			transformed_vertex.z -= camera_position.z;

			//project the current vertex
			vect2_t projected_point = project(transformed_vertex);

			//scale and translated the projected points to the middle of the screen
			projected_point.x += (window_width / 2);
			projected_point.y += (window_height / 2);


			//save the projected 2d vertex in the array of projected triangle points
			projected_triangle.points[j] = projected_point;
		}

		//save the projected triagnle in the array of triangles to render
		triangles_to_render[i] = projected_triangle;

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

void render(void)
{

	draw_grid();

	

	//Loop all projected triangles and render them
	for (int i = 0; i < N_MESH_FACES; i++)
	{
		triangle_t triangle = triangles_to_render[i];

		//draw vertex of triangle
		draw_rect(triangle.points[0].x, triangle.points[0].y, 3, 3, 0xFFFFFF00);
		draw_rect(triangle.points[1].x, triangle.points[1].y, 3, 3, 0xFFFFFF00);
		draw_rect(triangle.points[2].x, triangle.points[2].y, 3, 3, 0xFFFFFF00);

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


	render_color_buffer();

	clear_color_buffer(0xFF000000);
	SDL_RenderPresent(renderer);
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

	return 0;
}