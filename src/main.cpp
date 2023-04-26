#include "glad/glad.h"
#include <SDL2/SDL.h>
#include <iostream>

static const int SCREEN_WIDTH = 800;
static const int SCREEN_HEIGHT = 600;
static SDL_Window* window = NULL;
static SDL_GLContext main_context;

static void sdl_die(const char* message);
void init_screen(const char* caption);

float vertices[] = { -0.5f, -0.5f, 0.0f, 0.5f, -0.5, 0.0f, 0.0f, 0.5f, 0.0f };

int main() {
	init_screen("OpenGL Experiment");

	// Create vertex buffer
	GLuint vertex_buffer;
	glGenBuffers(1, &vertex_buffer);

	// create buffer context
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);

	// send data
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// keep window up
	SDL_Event e;
	while (true) {
		SDL_UpdateWindowSurface(window);

		SDL_PollEvent(&e);
		if (e.type == SDL_QUIT) {
			SDL_Quit();
			break;
		}

		glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

		// set default color when clearing screen
		glClearColor(0.0f, 0.5f, 1.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		SDL_GL_SwapWindow(window);
	}
}

static void sdl_die(const char* message) {
	fprintf(stderr, "%s: %s\n", message, SDL_GetError());
	exit(2);
}

void init_screen(const char* caption) {
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
		sdl_die("Failed to initialize the SDL library\n");
	}

	SDL_GL_LoadLibrary(NULL);

	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

	if (!(window = SDL_CreateWindow(caption, SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED, SCREEN_HEIGHT,
		SCREEN_WIDTH, SDL_WINDOW_OPENGL))) {
		sdl_die("Failed to create window");
	}

	if (!(main_context = SDL_GL_CreateContext(window))) {
		sdl_die("Failed to create OpenGL context");
	}

	if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
		sdl_die("Failed to load glad library");
	}

	// Check if everything was initialized correctly
	printf("Vendor: %s\n", glGetString(GL_VENDOR));
	printf("Renderer: %s\n", glGetString(GL_RENDERER));
	printf("Version: %s\n", glGetString(GL_VERSION));

	// Sync refresh rate with monitor
	SDL_GL_SetSwapInterval(1);

	// disable depth testing and face culling
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
}