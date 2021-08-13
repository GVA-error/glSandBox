#include <iostream>

#include <GL/glew.h>
#include <GL/glut.h>
#include <SDL.h>
#include <SDL_opengl.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/bind.h>
#endif

#include "fragmentShaderExample.h"
#include "vertexShaderExample.h"

const GLuint w_width  = 800;
const GLuint w_height = 600;

SDL_Window* mWindow  = GL_NONE;
GLuint shaderProgram = GL_NONE;

// drawing data
GLuint VBO = GL_NONE;
GLuint VAO = GL_NONE;

GLuint textureColorData[] = {
	255, 0, 0,
	0, 255, 0,
	0, 0, 255
};

GLuint texturePositionsData[] = {
	0, 0, 0, 0, 0, 125, 255, 255, 255, 255, 255
};

bool initGlContext(); // gl context
bool initShaders();   // init shaderProgram
void bindBuffers();   // buffer geometry data, init VBO, VAO
bool drawFrame();     // check events and draw scene
void drawScene();     // draw buffered data

bool init();

// desctop entery point
#ifndef __EMSCRIPTEN__
int main(int argc, char** argv)
{	
	if (init() == false)
		return 1;
	while (drawFrame())
		;
	return 0;
}
#else
extern "C" int main(int argc, char** argv)
{
	if (init() == false)
		return 1;
	emscripten_set_main_loop(drawFrame, 60, 1);
	return 0;
}
#endif

bool init()
{
	if (!initGlContext())
	{
		std::cerr << "\nCan\'t init context";
		return false;
	}
	if (!initShaders())
		return false;

	bindBuffers();
	return true;
}

bool is_exitEvent(SDL_Event& ev)
{
	if (ev.type == SDL_QUIT)
		return true;
	if (ev.type == SDL_KEYDOWN)
		if (ev.key.keysym.sym == SDLK_ESCAPE)
			return true;
	return false;
}

bool drawFrame()
{
	SDL_Event ev;
	while (SDL_PollEvent(&ev) != 0)
		if (is_exitEvent(ev))
			return false;

	drawScene();

	SDL_GL_SwapWindow(mWindow);
	SDL_UpdateWindowSurface(mWindow);
	return true;
}

void drawScene()
{
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(shaderProgram);

	// Draw the triangle
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glBindVertexArray(0);

}

void bindBuffers()
{
	GLfloat vertices[] = {   
		 0.5f, -0.5f, 0.0f,
		-0.5f, -0.5f, 0.0f,
		 0.0f,  0.5f, 0.0f
	};
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
}

bool initShaders()
{
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	// Check for compile time errors
	GLint success=GL_NONE;
	bool allSucces=true;
	GLchar infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	allSucces = allSucces && success;
	// Fragment shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	// Check for compile time errors
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	allSucces = allSucces && success;
	// Link shaders
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	// Check for linking errors
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	allSucces = allSucces && success;
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	return allSucces;
}

bool initGlContext()
{
	SDL_Init(SDL_INIT_VIDEO);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);

	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

	mWindow = SDL_CreateWindow(NULL, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w_width, w_height, SDL_WINDOW_OPENGL);
	if (mWindow == NULL)
	{
		std::cerr << "\nERROR:We can\'t create window " << SDL_GetError();
		return false;
	}

	SDL_GLContext context = SDL_GL_CreateContext(mWindow);
	if (context == NULL)
	{
		std::cerr << "\nERROR: We can\'t create context " << SDL_GetError();
		return false;
	}

	glewExperimental = true;
	GLenum glewError = glewInit();
	GLenum err;
	if (err = glewError != GLEW_OK)
	{
		std::cerr << "\nERROR: Glew init problem: " << glewGetErrorString(err);
		std::cerr << "\nERROR: GL version: " << glGetString(GL_VERSION) << std::endl;
		return false;
	}

	return true;
}