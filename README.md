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

# Continue from https://learnopengl.com/Getting-started/Hello-Triangle
