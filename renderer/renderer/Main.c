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
#include "material.h"

//////////////////////////////////////////////////////////////////////////////////
// Global variables for excution status and game loop
//////////////////////////////////////////////////////////////////////////////////

bool is_running = false;
int previous_frame_time = 0;
float delta_time = 0;

//////////////////////////////////////////////////////////////////////////////////
// Array of triangles that should be rendered frame by frame
//////////////////////////////////////////////////////////////////////////////////

#define MAX_TRIANGLES_PER_MESH 100000
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
	//Initialize light direction and light color
	init_light(vect3_new(0, 0, 1), vect3_new(1.0, 1.0, 1.0), 0.1);
	
	//Initialize material
	init_material(0xFF00FF00, 32.0, 1.0);

	//Initialize camera
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
	//load_mesh("./assets/runway.obj", "./assets/runway.png", vect3_new(1, 1, 1), vect3_new(0, -1.5, +23), vect3_new(0, 0, 0));
	//load_mesh("./assets/f22.obj", "./assets/f22.png", vect3_new(1, 1, 1), vect3_new(0, -1.3, +5), vect3_new(0, -M_PI/2, 0));
	//load_mesh("./assets/efa.obj", "./assets/efa.png", vect3_new(1, 1, 1), vect3_new(-2, -1.3, +9), vect3_new(0, -M_PI/2, 0));
	//load_mesh("./assets/f117.obj", "./assets/f117.png", vect3_new(1, 1, 1), vect3_new(+2, -1.3, +9), vect3_new(0, -M_PI/2, 0));
	

	//load_mesh("./assets/sphere.obj", "./assets/f22.png", vect3_new(1, 1, 1), vect3_new(0, 0, +5), vect3_new(0, 0, 0));
	load_mesh("./assets/samus.obj", "./assets/cube.png", vect3_new(1, 1, 1), vect3_new(0, 0, +5), vect3_new(0, 0, 0));
	//load_mesh("./assets/crab.obj", "./assets/crab.png", vect3_new(1, 1, 1), vect3_new(0, 0, +5), vect3_new(0, 0, 0));
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
//  +--------------+
//  |  Model space | <-- original mesh vertices
//  +--------------+	
//  |   +--------------+
//  `-> |  World space | <-- multiply by world matrix
//	    +--------------+
//	    |   +--------------+
//      `-> |  View space  | <-- multiply by view matrix
//	        +--------------+
//	        |   +--------------+
//          `-> |    Clipping  | <-- clip against six frustum planes 
// 	            +--------------+    
//			    |   +--------------+
//              `-> |  Projection  | <-- multiply by projection matrix  
//				    +--------------+		
//                  |   +--------------+
//                  `-> |  Image Space | <-- apply perspective divide
//				        +--------------+		
//					    |	+--------------+
//                      `-> | Screen Space | <-- ready to render
//				            +--------------+				
//////////////////////////////////////////////////////////////////////////////////

void process_graphic_pipeline_stages(mesh_t* mesh){

	//Create  scale, rotation and translation matrix that will be used to multiply the mesh vertices
	mat4_t scale_matrix = mat4_make_scale(mesh->scale.x, mesh->scale.y, mesh->scale.z);
	mat4_t rotation_matrix_x = mat4_make_rotation_x(mesh->rotation.x);
	mat4_t rotation_matrix_y = mat4_make_rotation_y(mesh->rotation.y);
	mat4_t rotation_matrix_z = mat4_make_rotation_z(mesh->rotation.z);
	mat4_t translation_matrix = mat4_make_translation(mesh->translation.x,mesh->translation.y, mesh->translation.z);

	//Update camera look at target to create view matrix
	vect3_t target = get_camera_look_at_target();
	vect3_t up_direction = { 0, 1, 0 };
	view_matrix = mat4_look_at(get_camera_position(), target, up_direction);

	//Loop all triangle faces of object mesh
	int num_faces = array_length(mesh->faces);

	for (int i = 0; i < num_faces; i++) {
		face_t mesh_face = mesh->faces[i];

		vect3_t face_vertices[3];
		face_vertices[0] = mesh->vertices[mesh_face.a];
		face_vertices[1] = mesh->vertices[mesh_face.b];
		face_vertices[2] = mesh->vertices[mesh_face.c];

		vect4_t transformed_vertices[3];

		//Loop through all three vertices of this current face and apply transformation
		for (int j = 0; j < 3; j++) {
			vect4_t transformed_vertex = vect4_from_vect3(face_vertices[j]);

			//Create a world matrix combining scale, rotation and translation
			world_matrix = mat4_identity();

			//Multiply all matrices and load the world matrix
			//*order matters: first scale, next rotate, then translate >>> [T]*[R]*[S]*v
			world_matrix = mat4_mul_mat4(scale_matrix, world_matrix);
			world_matrix = mat4_mul_mat4(rotation_matrix_z, world_matrix);
			world_matrix = mat4_mul_mat4(rotation_matrix_y, world_matrix);
			world_matrix = mat4_mul_mat4(rotation_matrix_x, world_matrix);
			world_matrix = mat4_mul_mat4(translation_matrix, world_matrix);

			//Multiply the world matrix with the original vector
			transformed_vertex = mat4_mul_vect4(world_matrix, transformed_vertex);

			//Multiply the view matrix with the world matrix transformed vector to transform the scene to camera space
			transformed_vertex = mat4_mul_vect4(view_matrix, transformed_vertex);

			//Save transformed vertex in the array of transformed vertices
			transformed_vertices[j] = transformed_vertex;
		}

		//Calculate the triangle normal
		vect3_t face_normal = get_triangle_face_normal(transformed_vertices);

		//Find the vector between vertex A in the triangle and the camera origin
		vect3_t cam_ray = vect3_sub(vect3_new(0,0,0), vect3_from_vect4(transformed_vertices[0]));


		//Calculate how align the camera ray is with the face normal (using dot product)
		float dot_normal_cam = vect3_dot(face_normal, cam_ray);

		//Backface culling test to see if the current face should be projected
		if (is_cull_backface()) {
			
			//Bypassing the triangles that looking away from the camera
			if (dot_normal_cam < 0) {
				continue;
			}
		}

		//Create a polygon from the original transformed triangle to be clipped
		polygon_t polygon = polygon_from_triangle(
			vect3_from_vect4(transformed_vertices[0]),
			vect3_from_vect4(transformed_vertices[1]),
			vect3_from_vect4(transformed_vertices[2]),
			mesh_face.a_uv,
			mesh_face.b_uv,
			mesh_face.c_uv
		);

		//Clip the polygon and returns a new polygon with potential new vertices
		clip_polygon(&polygon);

		//Break the clipped polygon apart back into the individual triangles
		triangle_t triangles_after_clipping[MAX_NUM_POLY_TRIANGLES];
		int num_triangles_after_clipping = 0;

		triangles_from_polygon(&polygon, triangles_after_clipping, &num_triangles_after_clipping);

		//Loops all the assembled triangles after clipping
		for (int t = 0; t < num_triangles_after_clipping; t++) {

			triangle_t triangle_after_clipping = triangles_after_clipping[t];
			vect4_t projected_points[3];

			//Loop through all three vertices of this current face and apply projection
			for (int j = 0; j < 3; j++) {
				//Project the current vertex using a perspective projection matrix
				projected_points[j] = mat4_mul_vect4(proj_matrix, triangle_after_clipping.points[j]);

				//Perform perspective divide -> which means all vertices are now in NDC (normalized device coordinates)
				if (projected_points[j].w != 0) {
					projected_points[j].x /= projected_points[j].w;
					projected_points[j].y /= projected_points[j].w;
					projected_points[j].z /= projected_points[j].w;
				}
				//Invert the y value to account for flipped screen y coordinate
				projected_points[j].y *= -1;

				//Scale into the viewport -> all vertices are now in screen space
				projected_points[j].x *= (get_window_width() / 2.0);
				projected_points[j].y *= (get_window_height() / 2.0);

				//Translate the projected points to the middle of the screen
				projected_points[j].x += (get_window_width() / 2.0);
				projected_points[j].y += (get_window_height() / 2.0);

			}

		
			//Calculate how align the light direction is with the face normal (using dot product) -> shade frequency (flat shading)
			vect3_t light_direction = get_light_direction();
			vect3_normalize(&light_direction);

			float light_intensity_factor = fmax(-vect3_dot(face_normal, light_direction), 0.0); // need minus sign in front becouse of inverse light direction 
			
			//Ambient component
			float ambient_strength = get_light_ambient_strgenth();

			//Specular component
			float shininess = get_material_shininess();
			float specular_strength = get_material_specular_strength();

			//Find the half vector between  light direction and view direction near the face normal
			vect3_t view_direction = vect3_sub(get_camera_position(), vect3_from_vect4(transformed_vertices[0]));
			vect3_normalize(&view_direction);

			uint32_t triangle_color = blinn_phong_shading(face_normal, light_direction, view_direction,
				&mesh_face, shininess, ambient_strength, specular_strength);


			////Pack the type vect3 light color into type uint32
			//uint32_t light_color = pack_color(get_light_color().x, get_light_color().y, get_light_color().z, 1.0);

			////Calculate the triangle color based on the light angle	
			//uint32_t diffuse_color = light_apply_intensity(light_color, diffuse_intensity_factor);
			//uint32_t ambient_color = light_apply_intensity(light_color, ambient_intensity_factor);
			//uint32_t specular_color = light_apply_intensity(light_color, specular_intensity_factor * specular_strength);
			////uint32_t triangle_color = (diffuse_color + ambient_color + specular_color)/3.0; // super exciting effect

			//vect4_t diffuse_light = { 0,0,0,0 };
			//vect4_t ambient_light = { 0,0,0,0 };
			//vect4_t specular_light = { 0,0,0,0 };

			//unpack_color(diffuse_color, &diffuse_light.x, &diffuse_light.y, &diffuse_light.z, &diffuse_light.w);
			//unpack_color(ambient_color, &ambient_light.x, &ambient_light.y, &ambient_light.z, &ambient_light.w);
			//unpack_color(specular_color, &specular_light.x, &specular_light.y, &specular_light.z, &specular_light.w);
			//
			//vect3_t unpacked_color = vect3_add(vect3_from_vect4(ambient_light), 
			//	vect3_add(vect3_from_vect4(diffuse_light), vect3_from_vect4(specular_light)));
			//unpacked_color = vect3_div(unpacked_color, 3.0);

			//unpacked_color.x *= mesh_color.x;
			//unpacked_color.y *= mesh_color.y;
			//unpacked_color.z *= mesh_color.z;

			//Pack the final color into uint32_t
			//uint32_t triangle_color = pack_color(unpacked_color.x, unpacked_color.y, unpacked_color.z, 1.0); //Assuming full opacity


			//Save the projected 2d vertex in the array of projected triangle points
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
				.texture = mesh->textures,
				.light_intensity_factor = light_intensity_factor,
			};
			//Save the projected triagnle in the array of triangles to render
			if (num_triangles_to_render < MAX_TRIANGLES_PER_MESH) {
				triangles_to_render[num_triangles_to_render] = triangle_to_render;
				num_triangles_to_render++;
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////
// Call update function every frame
//////////////////////////////////////////////////////////////////////////////////
void update(void){
	
	//Wait some time until reaching the target frame time in miliseconds
	int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - previous_frame_time);

	//Only delay excution if running too fast
	if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME ){
		SDL_Delay(time_to_wait);
	}

	//Get a delta time factor converted to seconds to be used to update our game object
	delta_time = (SDL_GetTicks() - previous_frame_time) / 1000.0; //-> 1/framerate
	
	previous_frame_time = SDL_GetTicks();

	//Initialize the counter of triangles to render for the current frame
	num_triangles_to_render = 0;

	//Loop all the meshes in the scene
	for (int  mesh_index = 0; mesh_index < get_num_meshes(); mesh_index++){
		mesh_t* mesh = get_mesh(mesh_index);

		//Change the mesh scale/rotation values per animation frame
		//mesh->rotation.x += 0.5 * delta_time;
		mesh->rotation.y += 0.5 * delta_time;
		//mesh->rotation.z += 0.0 * delta_time;
		//mesh->scale.x += 0;
		//mesh->scale.y += 0;
		//mesh->translation.x += 0.0;
		//mesh->translation.z = 5;

		//Process the graphic pipeline stages for every mesh in 3d scene
		process_graphic_pipeline_stages(mesh);
	}
}

//////////////////////////////////////////////////////////////////////////////////
// Render function to draw objects on the display
//////////////////////////////////////////////////////////////////////////////////
void render(void){

	//Clear all the arrays to get ready for the next frame
	clear_color_buffer(0xFF000000);
	clear_z_buffer();

	draw_grid();

	//Loop all projected triangles and render them
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
				triangle.texture,
				triangle.light_intensity_factor
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
			draw_rect(triangle.points[0].x - 3, triangle.points[0].y - 3, 6, 6, 0xFF0000FF); //draw vertex a
			draw_rect(triangle.points[1].x - 3, triangle.points[1].y - 3, 6, 6, 0xFF0000FF); //draw vertex b
			draw_rect(triangle.points[2].x - 3, triangle.points[2].y - 3, 6, 6, 0xFF0000FF); //draw vertex c
		}	
	}

	//finally draw the color buffer to the SDL window
	render_color_buffer();
}

//////////////////////////////////////////////////////////////////////////////////
// Free the memory that was dynamically allocated by the program
//////////////////////////////////////////////////////////////////////////////////
void free_resource(void){
	free_meshes();
	destroy_window();
}

//////////////////////////////////////////////////////////////////////////////////
// Main funtion of this tiny software rasterazation renderer
//////////////////////////////////////////////////////////////////////////////////

int main(int argc, char* args[]){	
	is_running = initialize_window();
	setup();
	while(is_running){
		process_input();
		update();
		render();
	}
	free_resource();
	return 0;
}