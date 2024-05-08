
#include "display.h"

 SDL_Window* window = NULL;
 SDL_Renderer* renderer = NULL;
 uint32_t* color_buffer = NULL;
 SDL_Texture* color_buffer_texture = NULL;

 int window_width = 800;
 int window_height = 600;


bool initialize_window(void)
{
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		fprintf(stderr, "Error initializing SDL. \n");

		return false;
	}
	// Use SDL to query what is the fullscreen max. width and height
	SDL_DisplayMode display_mode;
	SDL_GetCurrentDisplayMode(0, &display_mode);
	window_width = display_mode.w;
	window_height = display_mode.h;

	// Create a SDL window		
	window = SDL_CreateWindow(
		NULL,
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		window_width,
		window_height,
		SDL_WINDOW_BORDERLESS
	);
	if (!window)
	{
		fprintf(stderr, "Error creating SDL window. \n");
		return false;
	}

	// Create a SDL renderer
	renderer = SDL_CreateRenderer(window, -1, 0);

	if (!renderer)
	{
		fprintf(stderr, "Error creating SDL renderer. \n");
		return false;
	}

	SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

	return true;
}




void draw_pixel(int x, int y, uint32_t color)
{
	if (x>=0 && x < window_width && y>= 0 && y < window_height)
	{
		color_buffer[(window_width * y) + x] = color;
	}
		
}

void draw_rect(int upper_left_pos_x, int upper_left_pos_y, int width, int height, uint32_t color)
{
	for (int y = 0; y < height; y += 1)
	{
		for (int x = 0; x < width; x += 1)
		{
			int current_x = upper_left_pos_x + x;
			int current_y = upper_left_pos_y + y;

			//color_buffer[(window_width)*current_y + current_x] = color;
			draw_pixel(current_x, current_y, color);
		}
	}

}

void draw_grid(void)
{
	//TODO:
	//Draw a background grid that fills the entire window.
	//Lines should be rendered at every row/col multiple of 10.

	for (int y = 0; y < window_height; y += 20)
		for (int x = 0; x < window_width; x += 20)
			color_buffer[(window_width)*y + x] = 0xFF333333;


}


void clear_color_buffer(uint32_t color)
{
	for (int y = 0; y < window_height; y += 1)
		for (int x = 0; x < window_width; x += 1)
			color_buffer[(window_width)*y + x] = color;

}

void render_color_buffer(void)
{
	SDL_UpdateTexture(
		color_buffer_texture,
		NULL,
		color_buffer,
		(int)(window_width * sizeof(uint32_t))
	);

	SDL_RenderCopy(renderer, color_buffer_texture, NULL, NULL);
}



void destroy_window(void)
{
	free(color_buffer);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}