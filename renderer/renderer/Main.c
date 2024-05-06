#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL.h>



bool is_running = false;
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

uint32_t* color_buffer = NULL;
SDL_Texture* color_buffer_texture = NULL;

int window_width = 800;
int window_height = 600;

bool initialize_window(void)
{
	if(SDL_Init(SDL_INIT_EVERYTHING) != 0)
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

	SDL_SetWindowFullscreen(window,SDL_WINDOW_FULLSCREEN);

	return true;
}

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
void update(void)
{
}

void draw_rect(int upper_left_pos_x, int upper_left_pos_y, int width, int height, uint32_t color)
{
	for (int y = 0; y < height; y += 1) 
	{
		for (int x = 0; x < width; x += 1)
		{
			int current_x = upper_left_pos_x + x;
			int current_y = upper_left_pos_y + y;

			color_buffer[(window_width) * current_y + current_x] = color;
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
				color_buffer[(window_width)*y + x] = 0xFF00FF00;
			
			
}


void clear_color_buffer(uint32_t color)
{
	for (int y = 0; y < window_height; y = y + 1)
		for (int x = 0; x < window_width; x = x + 1)
			color_buffer[(window_width) * y + x] = color;
	
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

void render(void)
{
	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
	SDL_RenderClear(renderer);
	draw_grid();
	draw_rect(window_width/2,window_height/2,500,300,0xFF009900);
	
	render_color_buffer();

	clear_color_buffer(0xFF000000);
	SDL_RenderPresent(renderer);
}

void destroy_window(void)
{
	free(color_buffer);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
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