#include "glad/glad.h"
#include <SDL2/SDL.h>
#include <fstream>
#include <iostream>
#include <string>

const char* vertex_shader_source = R"(
#version 330 core
layout(location=0)in vec3 aPos;

void main()
{
	gl_Position=vec4(aPos.x,aPos.y,aPos.z,1.);
}
)";

const char* fragment_shader_source = R"(
#version 330 core
out vec4 FragColor;

void main()
{
    FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
} 
)";

static const int SCREEN_WIDTH = 800;
static const int SCREEN_HEIGHT = 600;
static SDL_Window* window = NULL;
static SDL_GLContext main_context;

static void sdl_die(const char* message);
void init_screen(const char* caption);
void init_opengl();
void check_shader_compile_success(GLuint shader_id, const char* message);

// Triangle vertices
static float vertices[] = {
    -0.5f, -0.25f, 0.5f,  -0.25, 0.0f,	 0.25f,
    -1.0f, -1.0f,  -0.9f, -1.0f, -0.95f, -0.95f,
};

GLuint shader_program, VAO;

int main() {
	init_screen("OpenGL Experiment");

	init_opengl();

	// keep window up
	SDL_Event e;
	while (true) {
		SDL_UpdateWindowSurface(window);

		SDL_PollEvent(&e);
		if (e.type == SDL_QUIT) {
			SDL_Quit();
			break;
		}

		// set default color when clearing screen
		glClearColor(0.0f, 0.5f, 1.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Tell the GPU to use the program
		glUseProgram(shader_program);
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		SDL_GL_SwapWindow(window);
	}
}

void init_opengl() {
	// Create vertex buffer (eg. VBO)
	GLuint vertex_buffer;
	glGenBuffers(1, &vertex_buffer);

	// Create vertex array object (eg. VAO)
	glGenVertexArrays(1, &VAO);

	/// DATA

	// start using VAO
	glBindVertexArray(VAO);

	// create buffer context
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);

	// send data
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices,
		     GL_STATIC_DRAW);

	// Specify what the attributes in the shaders are
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);
	// enable the attribute
	glEnableVertexAttribArray(0);

	/// SHADERS

	// vertex shader
	uint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
	glCompileShader(vertex_shader);
	check_shader_compile_success(
	    vertex_shader, "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n");

	// fragment shader
	uint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
	glCompileShader(fragment_shader);
	check_shader_compile_success(
	    fragment_shader, "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n");

	// Create shader program and link shaders
	shader_program = glCreateProgram();
	glAttachShader(shader_program, vertex_shader);
	glAttachShader(shader_program, fragment_shader);
	glLinkProgram(shader_program);

	GLint program_link_success;
	char info_log[512];
	glGetProgramiv(shader_program, GL_LINK_STATUS, &program_link_success);
	if (!program_link_success) {
		glGetProgramInfoLog(shader_program, 512, NULL, info_log);
		std::cout << "ERROR:PROGRAM::LINK_FAILED\n" << info_log << "\n";
	}

	// Delete source code because we no longer need it
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);
}

void check_shader_compile_success(GLuint shader_id, const char* message) {
	int compile_success;
	glGetShaderiv(shader_id, GL_COMPILE_STATUS, &compile_success);
	if (!compile_success) {
		char info_log[512];
		glGetShaderInfoLog(shader_id, 512, NULL, info_log);
		std::cout << message << info_log << "\n";
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