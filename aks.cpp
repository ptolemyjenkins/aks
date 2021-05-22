#include <stdlib.h>
#include <iostream>
#include "include/glad/glad.h"
#include "deps/glad.c"
#include "deps/linmath.h"
#include <GLFW/glfw3.h>

// Test verts

static const struct
	{
		float x,y,z;
		float r,g,b;
	} vertices[3] {
		{-1.0f,-1.0f,0.0f,1.0f,0.0f,0.0f},
		{1.0f,-1.0f,0.0f,0.0f,1.0f,0.0f},
		{1.0f,1.0f,0.0f,0.0f,0.0f,1.0f},
	};
	
static const char* vertex_shader_text =
	"#version 110\n"
	"uniform mat4 MVP;\n"
	"attribute vec3 vCol;\n"
	"attribute vec2 vPos;\n"
	"varying vec3 color;\n"
	"void main()\n"
	"{\n"
	"    gl_Position = MVP * vec4(vPos, 0.0, 1.0);\n"
	"    color = vCol;\n"
	"}\n";

static const char* fragment_shader_text =
	"#version 110\n"
	"varying vec3 color;\n"
	"void main()\n"
	"{\n"
	"    gl_FragColor = vec4(color, 1.0);\n"
	"}\n";

// Variables

int defaultWidth{1024}, defaultHeight{768}, defaultFPS{60};
int fullWidth{1440}, fullHeight{900};
bool fullscreen{false};

// Callbacks

void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

void windowHints()
{
	glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
	if (key == GLFW_KEY_F && action == GLFW_PRESS)
	{
		fullscreen = !fullscreen;
		glfwSetWindowMonitor(window, fullscreen ? glfwGetPrimaryMonitor() : NULL, 0, 0, 
		fullscreen ? fullWidth: defaultWidth, 
		fullscreen ? fullHeight: defaultHeight, GLFW_DONT_CARE);
		glViewport(0, 0, fullscreen? fullWidth : defaultWidth, 
					 fullscreen? fullHeight : defaultHeight);
		
	}
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}



int main()
{
	if( !glfwInit() )
	{
	    fprintf( stderr, "Failed to initialize GLFW\n" );
	    return -1;
	}
	
	// Set our error_callback to be the one used
	glfwSetErrorCallback(error_callback);
	windowHints();
	
	// More Glad stuff that I need to unpack
    GLuint vertex_buffer, vertex_shader, fragment_shader, program;
    GLint mvp_location, vpos_location, vcol_location;
	
	// Open a window and create its OpenGL context
	GLFWwindow* window = glfwCreateWindow( defaultWidth, defaultHeight, "Aks Engine Test",
	fullscreen? glfwGetPrimaryMonitor() : NULL, NULL);
	if( window == NULL ){
	    glfwTerminate();
	    return -1;
	}
	
	// Set current context
	glfwMakeContextCurrent(window); 
	
	// Set window to have default callbacks
	glfwSetKeyCallback(window,key_callback); 
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSwapInterval(1);
	
	// Fucking glad this works
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
	    std::cout << "Failed to initialize GLAD" << std::endl;
	    return -1;
	}    
	
	//Set size of rendering to be size of window and same position
	glViewport(0, 0, fullscreen? fullWidth : defaultWidth, 
				fullscreen? fullHeight : defaultHeight);
				
    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
    glCompileShader(vertex_shader);

    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
    glCompileShader(fragment_shader);

    program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    mvp_location = glGetUniformLocation(program, "MVP");
    vpos_location = glGetAttribLocation(program, "vPos");
    vcol_location = glGetAttribLocation(program, "vCol");

    glEnableVertexAttribArray(vpos_location);
    glVertexAttribPointer(vpos_location, 2, GL_FLOAT, GL_FALSE,
                          sizeof(vertices[0]), (void*) 0);
    glEnableVertexAttribArray(vcol_location);
    glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE,
                          sizeof(vertices[0]), (void*) (sizeof(float) * 2));
					
					
	// Window loop
	while (!glfwWindowShouldClose(window))
	{
        mat4x4 m, p, mvp;
		
		glClear(GL_COLOR_BUFFER_BIT);
		
        mat4x4_identity(m);
        mat4x4_rotate_Z(m, m, (float) glfwGetTime());
        mat4x4_ortho(p, -1.5f, 1.5f, -1.0f, 1.0f, 1.0f, -1.0f);
        mat4x4_mul(mvp, p, m);
		
		glUseProgram(program);
		glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*) mvp);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
