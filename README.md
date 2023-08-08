# What is this

This my learning process of learning 3D rendering at a hardware level using SDL and OpenGL.
I am using the https://learnopengl.com/Getting-started/ site to learn and implement this.

# OpenGL

Graphics Pipeline - A pipeline which takes 3D vertex data as input and transforms it into 2D pixel data as output

It consists of:
    - vertex data[] as input
    - vertex shader
    - shape assembly
    - geometry shader
    - rasterization
    - fragment shader
    - tests and blending

Note that all the steps with "shader" mean that those are the parts in the pipeline that the programmer can customize with their own code.

## Vertex data[]

The vertex data consists of 3 3D co-ords which form a triangle in an array. 

A vertex is just a collection of data per 3D co-ord. The vertex data can include any other data other than the co-ord. 

For this simple example we are just using the co-ord and the color.

We need to tell OpenGL what kind of render types we want to make with the vertex data (eg. point, line, triangle, etc.).
These are called "GL_POINTS", "GL_LINE_STRIP", "GL_TRIANGLES" accordingly.

"GL_QUADS" is the primitive for quads.

## Vertex Shader

Takes a single vertex as input and transforms it into a different 3D co-ord. Can also modify the values in vertex attributes. 

## Geometry Shader

Takes a collection of vertices that form a primitive and generates new primitives by emitting new vertices to build those primitives with. 

## Primitive Assembly

Takes all vertices as input that form the primitives and assembles all the points into the given primitive. 

## Rasterization 

Takes the primitives as inputs and maps them onto 2D co-rds on your screen resulting in fragments. 
Clipping is done where anything outside of the view of the camera is cut out. 

## Fragment Shader

Calculates the final color of each pixel, this is usually where all the advanced OpenGL effects occur. Usually contains data about the scene like lights, shadows, etc. 

## Testing

Alpha test and blending stage. Checks corresponding depth and stencil value and checks if it is behind (or in front) of any other objects in the scene. 
It also checks the alpha values of the objects and blends them accordingly. 

# Drawing Something

To start we have to pass in some 3D (x, y, z) vertex data. The data we pass in has to be in the range (-1.0, 1.0). 

This range (-1.0, 1.0) is called the normalized device coordinates, any vertices outside this range won't be visible. 

We want to draw a 2D triangle so all of the z coordinates will be 0 so it appears flat on the screen. 

We then send this data to the GPU, we do this by: creating memory on the GPU to store the data; configure how OpenGL should interpret the data; and specify how to send the data to the GPU.

This data is managed by "vertex buffer objects" (VBOs) that can store a large number of vertices in the GPU's memory. The advantage of using VBOs is being able to send batch amounts of data simultaniously instead of 1 vertex at a time and also keep it there if there is enough memory left. 

Sending data from the CPU to the GPU is slow so it is better to do it in chunks. Once in the GPU's memory it has almost instant read access to the vertices. 

You can make a VBO using `void glGenBuffers(GLsizei n, GLuint * buffers)` and passing the amount of buffers to generate and a pointer to the variable that holds the id of the buffer (it can generate more than 1 buffer at a time). The function essentially tells the GPU that we are going to be allocating memory and data to this buffer at some point and gives us the Id CPU side so we can reference it. 
Deleting them is done using "glDeleteBuffers"

The `glBindBuffer(target, buffer)` tells the GPU what the buffer we reference using the buffer parameter will be used for, it gives the GPU "context". This sets up the internal state of the buffer. If we just want to store data in the buffer it doesn't matter which target we specify. 

`void glBufferData(enum target, sizeiptr size, const void *data, enum usage)`
copies user defined data into the buffer (this creates mutable data). `target` is the type of buffer we want to write to (eg. the buffer we assigned using `glBindBuffer`), `size` is the size in bytes of the data, `data` is the pointer to the data we want to write to the buffer, `usage` is tells the GPU how we want to use the data. 
`usage` has 3 options: 

- GL_STREAM_DRAW - the data is set only once and used only a few times
- GL_STATIC_DRAW - the data is set only once and used many times
- GL_DYNAMIC_DRAW - the data is changed a lot and used many times

This tells the GPU how it should handle the memory, different options will allow for slower/ faster read/ write depending on which option we pick. 

# Vertex Shader

This is the first shader that we can write in the pipeline.

This is a very simple sample vertex shader:
```
#version 330 core
layout (location = 0) in vec3 aPos;

void main()
{
    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
}
```

We need to specify the version we are writing in also the mode we are in (compatability or core).

The `in` keyword specifies the input vertex attributes. This example takes the vertex data into a `vec3` called `aPos`. It also needs to specify the location of the input variable via `layout`.

`gl_Position` is a predefined variable which is a `vec4` we assign to this variable to output from the shader. The use of the 4th dimension in this variable will be explained later. 

This shader literally just forwards the data taken from the input and outputs it the same (but with the `vec4` format).

To use the shader in the GPU first we create a shader using `GLuint glCreateShader(GLenum shaderType)` passing in the shader type (in this case `GL_VERTEX_SHADER`) and it returns the id of the shader. 

`void glShaderSource(GLuint shader, GLsizei count, const GLchar **string, const GLint *length)` passes in the shader source code to the GPU.

Parameters

`shader`
- Specifies the handle of the shader object whose source code is to be replaced.
`count`
- Specifies the number of elements in the string and length arrays.
`string`
- Specifies an array of pointers to strings containing the source code to be loaded into the shader.
`length`
- Specifies an array of string lengths.

For this I wrote the shader in a single raw string so we only have 1 count and the array only has 1 string and the length of `NULL` because we only have 1 string. 

`void glCompileShader(GLuint shader)` compiles the shader.

Parameters

`shader`
- The ID of the shader.

We can check if the shader compiles correctly using this code: 

```c++
int compile_success;
glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &compile_success);
if (!compile_success) {
    char info_log[512];
    glGetShaderInfoLog(vertex_shader, 512, NULL, info_log);
    std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
          << info_log << "\n";
}
```

# Fragment Shader

The fragment shader is responsible for colouring all of the pixels in the shader program. 
Colours are represented using 4 floats (red, green, blue, alpha) each between 0.0 and 1.0.

Sample fragment shader: 
```
#version 330 core
out vec4 FragColor;

void main()
{
    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
} 
```

# Shader Program

To actually create the shader program we first have to link the shaders together to create it. This is where we will get linking errors if the inputs of the shaders do not match. 

`GLuint glCreateProgram()` creates a shader program and returns the id. 

`glAttachShader(GLuint program_id, GLuint shader_id)` attaches the shader to the program. 

`glLinkProgram(GLuint program_id)` links together the shaders of the program together. 

We can check if the linking failed using this code:
```c++
glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
if(!success) {
    glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
    ...
}
```

Then we tell the GPU to use the program using `glUseProgram(GLuint program_id)`.
Every rendering call after this will call this program to render the results.

After linking the shaders to the program we don't need the source code anymore so we can call `glDeleteShader(GLuint shader_id)` to delete the shader. 

## Vertex Attributes

The vertex shader allows us to specify any input we want in the form of vertex attributes, this means we have to manually specify the location of all the attributes we want to use.

The buffer we created is in the format of 3 vertices made of `vec3` which form a triangle.
It has the following attributes:

- The position data is stored as 32-bit (4 byte) floating point values.
- Each position is composed of 3 of those values.
- There is no space (or other values) between each set of 3 values. The values are tightly packed in the array.
- The first value in the data is at the beginning of the buffer.

`glVertexAttributePointer` tells OpenGL how to interpret that data. 
The VBO which the attribute recieves its data from is determined by which VBO is bound before calling `glVertexAttributePointer`.
- From OpenGL version 3.2 index 0 is invalid
```c++
void glVertexAttribPointer(
    GLuint index,
  	GLint size,
  	GLenum type,
  	GLboolean normalized,
  	GLsizei stride,
  	const void * pointer);
 
void glVertexAttribIPointer(
    GLuint index,
  	GLint size,
  	GLenum type,
  	GLsizei stride,
  	const void * pointer);
 
void glVertexAttribLPointer(
    GLuint index,
  	GLint size,
  	GLenum type,
  	GLsizei stride,
  	const void * pointer);
```

Parameters

`index`
- specifies which vertex attribute we want to configure
- this changes the data in `layout (location = x)` that we mentioned earlier
- From OpenGL version 3.2 index 0 is invalid

`size`
- the size of the vertex attribute 
- in this case we want to configure position which is a `vec3` so it is made of 3 floats so we pass `3`

`type`
- specifies the type of data
- with position it is `GL_FLOAT`

`normalized`
- specifies if we want the data to be normalized 
- if this is activated and we are passing in integer data then the data is transformed to be between 0.0 (or -1.0 when signed) and 1.0 
- takes `GL_TRUE` or `GL_FALSE`

`stride`
- specifies the space between consecutive vertex attributes 
- for position it is made of 3 floats so we pass in the size of 3 floats (`sizeof(float) * 3`) 
- passing in `0` will tell OpenGL to try determine the stride itself (only works for tightly packed data) 
- useful for when the buffer is filled with more than 1 kind of data. eg. position and color

`pointer`
- this specifies where the start of the data is in the buffer
- since our data starts at the beginning we pass in `0`


We also need to enable the attribute after specifying it using `glEnableVertexAttribArray(GLuint index)`

## Vertex Array Object (VAO)

Instead of making many calls to assign which objects will use which shaders and specify which data buffers to use, we can store all of that information inside a single object and tell the GPU when we want to render it. 

Generate a VAO using:
```c++
uint32_t VAO;
glGenVertexArrays(1, &VAO);
```

Then bind to the VAO using:
```c++
glBindVertexArray(VAO);
```

# Drawing
Each frame needs to be drawn to the screen. This can be done in a loop using:
```c++
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
		glDrawArrays(GL_TRIANGLES, 0, 3);

		SDL_GL_SwapWindow(window);
	}
```

`glDrawArrays` specifies the type of object to draw, the starting index in the array and how many vertices to draw. 
