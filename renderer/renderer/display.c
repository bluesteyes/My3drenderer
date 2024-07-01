
#include "display.h"

#define GAMMA 1.0
#define INV_GAMMA (1.0 / GAMMA)
#define CLAMP(x, lower, upper) ((x) < (lower) ? (lower) : ((x) > (upper) ? (upper) : (x)))

 static SDL_Window* window = NULL;
 static SDL_Renderer* renderer = NULL;
 static float* z_buffer = NULL;
 static uint32_t* color_buffer = NULL;
 static SDL_Texture* color_buffer_texture = NULL;

 static int window_width = 800;
 static int window_height = 600;


 float gamma_correct(float value, float gamma) {
	 return pow(value, gamma);
 }

 vect4_t gamma_correct_color(vect4_t color, float gamma) {
	 vect4_t corrected;
	 corrected.x = gamma_correct(color.x, gamma);
	 corrected.y = gamma_correct(color.y, gamma);
	 corrected.z = gamma_correct(color.z, gamma);
	 corrected.w = color.w;  // Alpha is usually not gamma-corrected
	 return corrected;
 }



vect4_t mul_colors(vect4_t c1, vect4_t  c2) {

	vect4_t corrected_c1 = gamma_correct_color(c1, INV_GAMMA);
	vect4_t corrected_c2 = gamma_correct_color(c2, INV_GAMMA);

	vect4_t  result;
	 result.x = corrected_c1.x * corrected_c2.x;
	 result.y = corrected_c1.y * corrected_c2.y;
	 result.z = corrected_c1.z * corrected_c2.z;
	 result.w = corrected_c1.w * corrected_c2.w;

	 // Clamp the results to [0, 1]
	 result.x = CLAMP(result.x, 0.0f, 1.0f);
	 result.y = CLAMP(result.y, 0.0f, 1.0f);
	 result.z = CLAMP(result.z, 0.0f, 1.0f);
	 result.w = CLAMP(result.w, 0.0f, 1.0f);

	 return result;
 }


 // Color packing function
 uint32_t pack_color(float r, float g, float b, float a) {
	 uint32_t color = 0;
	 color |= ((uint32_t)(a * 255) & 0xFF) << 24;
	 color |= ((uint32_t)(r * 255) & 0xFF) << 16;
	 color |= ((uint32_t)(g * 255) & 0xFF) << 8;
	 color |= ((uint32_t)(b * 255) & 0xFF);

	 return color;
 }

 // Color unpacking function
 void unpack_color(uint32_t color, float* r, float* g, float* b, float* a) {

	 *a = ((color >> 24) & 0xFF) / 255.0;
	 *r = ((color >> 16) & 0xFF) / 255.0;
	 *g = ((color >> 8) & 0xFF) / 255.0;
	 *b = (color & 0xFF) / 255.0;

 }


 int get_window_width(void) {
	 return window_width;
 }

 int get_window_height(void) {
	 return window_height;
 }

 void set_cull_method(int method) {
	 cull_method = method;
 }

 void set_render_method(int method) {
	 render_method = method;
 }

 bool is_cull_backface(void) {
	return cull_method == CULL_BACKFACE;
 }

 bool should_render_fill_triangle(void) {
	 return ( 
		 render_method == RENDER_FILL_TRIANGLE ||
		 render_method == RENDER_FILL_TRIANGLE_WIRE
		 );
 }

 bool should_render_aabb_triangle(void) {
	 return (
		 render_method == RENDER_AABB_TRIANGLE 
		 );
 }

 bool should_render_texture_triangle(void) {
	 return(
		 render_method == RENDER_TEXTURED || 
		 render_method == RENDER_TEXTURED_WIRE
		 );
 }
 bool should_render_aabb_texture_triangle(void) {
	 return(
		 render_method == RENDER_AABB_TEXTURED_TRIANGLE
		 );
 }

 bool should_render_wireframe(void) {
	 return(
		 render_method == RENDER_WIRE ||
		 render_method == RENDER_WIRE_VERTEX ||
		 render_method == RENDER_FILL_TRIANGLE_WIRE ||
		 render_method == RENDER_TEXTURED_WIRE
		 );
 }

 bool should_render_wire_vertex(void) {
	 return(render_method == RENDER_WIRE_VERTEX);
 }


bool initialize_window(void){
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0){
		fprintf(stderr, "Error initializing SDL. \n");
		return false;
	}

	// Use SDL to query what is the fullscreen max. width and height
	SDL_DisplayMode display_mode;
	SDL_GetCurrentDisplayMode(0, &display_mode);
	int fullscreen_width = display_mode.w;
	int fullscreen_height = display_mode.h;
	window_width = fullscreen_width / 3;
	window_height = fullscreen_height / 3;

	// Create a SDL window		
	window = SDL_CreateWindow(
		NULL,
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		fullscreen_width,
		fullscreen_height,
		SDL_WINDOW_BORDERLESS
	);
	if (!window){
		fprintf(stderr, "Error creating SDL window. \n");
		return false;
	}

	// Create a SDL renderer
	renderer = SDL_CreateRenderer(window, -1, 0);

	if (!renderer){
		fprintf(stderr, "Error creating SDL renderer. \n");
		return false;
	}

	SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

	// Allocate the required memory in bytes to hold the color buffer and z buffer
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

	return true;
}

void draw_pixel(int x, int y, uint32_t color){
	if (x < 0 || x >= window_width || y < 0 || y >= window_height){
		return;
	}	
	color_buffer[(window_width * y) + x] = color;
}

// this draw line function use the algorithm of DDA
void draw_line(int x0, int y0, int x1, int y1, uint32_t color){
	int delta_x = (x1 - x0);
	int delta_y = (y1 - y0);

	int side_length = (abs(delta_x) >= abs(delta_y)) ? abs(delta_x) : abs(delta_y);

	float current_x = x0;
	float current_y = y0;

	//find how much we should increment in both x and y of each step
	float x_inc = delta_x / (float)side_length;
	float y_inc = delta_y / (float)side_length;

	for (int i = 0; i < side_length; i++){
		draw_pixel(round(current_x),round(current_y), color);

		current_x += x_inc;
		current_y += y_inc;
	}
}

void draw_rect(int upper_left_pos_x, int upper_left_pos_y, int width, int height, uint32_t color){
	for (int y = 0; y < height; y += 1){
		for (int x = 0; x < width; x += 1){
			int current_x = upper_left_pos_x + x;
			int current_y = upper_left_pos_y + y;

			draw_pixel(current_x, current_y, color);
		}
	}
}

void draw_grid(void){
	//Draw a background grid that fills the entire window.
	//Lines should be rendered at every row/col multiple of 10.

	for (int y = 0; y < window_height; y += 20)
		for (int x = 0; x < window_width; x += 20)
			color_buffer[(window_width)*y + x] = 0xFF333333;

}


void clear_color_buffer(uint32_t color){
	for (int i = 0; i < window_width * window_height; i++) {
		color_buffer[i] = color;
	}
}

void clear_z_buffer(void) {
	for (int i = 0; i < window_width * window_height; i++) {
		z_buffer[i] = 1.0;
	}
}

float get_z_buffer_at(int x, int y) {
	if (x < 0 || x >= window_width || y < 0 || y >= window_height){
		return 1.0;
	}
	return z_buffer[(window_width * y) + x];
}

void update_z_buffer_at(int x, int y, float value) {
	if (x < 0 || x >= window_width || y < 0 || y >= window_height) {
		return;
	}
	z_buffer[(window_width * y) + x] = value;
}

void render_color_buffer(void){
	SDL_UpdateTexture(
		color_buffer_texture,
		NULL,
		color_buffer,
		(int)(window_width * sizeof(uint32_t))
	);
	SDL_RenderCopy(renderer, color_buffer_texture, NULL, NULL);
	SDL_RenderPresent(renderer);
}


void destroy_window(void){
	free(color_buffer);
	free(z_buffer);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}


