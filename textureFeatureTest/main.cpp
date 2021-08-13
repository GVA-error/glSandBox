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

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

const bool SUBTEXTURE_BINDING = false;

const GLuint w_width  = 800;
const GLuint w_height = 600;

SDL_Window* window  = GL_NONE;
GLuint shaderProgram = GL_NONE;

GLuint colorTexture = GL_NONE;
GLuint colorPositionsTexture = GL_NONE;

// drawing data
GLuint VBO = GL_NONE;
GLuint VAO = GL_NONE;

//Color texture data and sizes
const GLuint tc_w=3;
const GLuint tc_h=1;
const GLuint tc_d=1;
GLuint textureColorData[] = {
	125, 125, 125,
	125, 125, 125,
	125, 125, 125
};

//Positions/proportions texture data and sizes
const GLuint tp_w = 11;
const GLuint tp_h = 1;
const GLuint tp_d = 1;
GLuint texturePositionsData[] = {
	0, 0, 0, 0, 0, 125, 255, 255, 255, 255, 255
};

bool initGlContext(); // init gl, sdl etc.
bool initShaders();   // init shaderProgram
void bindBuffers();   // buffer geometry data, init VBO, VAO
void fillTextures();  // init colorTexture and colorPositionsTexture from textureColorData and texturePositionsData
bool drawFrame();     // check events and draw scene
void drawScene();     // draw buffered data

void texParameter(int i_param);
bool is_exitEvent(SDL_Event& ev);
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
	fillTextures();
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

	SDL_GL_SwapWindow(window);
	SDL_UpdateWindowSurface(window);
	return true;
}

void drawScene()
{
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(shaderProgram);

	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_3D, colorTexture);
	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_3D, colorPositionsTexture);

	// Draw the triangle
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glBindVertexArray(0);

}

void texParameter(int i_param)
{
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void fillTextures()
{
	glGenTextures(1, &colorTexture);
	if (SUBTEXTURE_BINDING)
	{
		glBindTexture(GL_TEXTURE_3D, colorTexture);
		glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, 0, tc_w, tc_h, tc_d, GL_RGB, GL_UNSIGNED_BYTE, textureColorData);
	}
	else
	{
		glBindTexture(GL_TEXTURE_3D, colorTexture);
		glTexImage3D(GL_TEXTURE_3D, 0, GL_RGB, tc_w, tc_h, tc_d, 0, GL_RGB, GL_UNSIGNED_BYTE, textureColorData);
	}
	texParameter(0);
	/*glBindTexture(GL_TEXTURE_3D, 0);
	
	glGenTextures(1, &colorPositionsTexture);
	texParameter(1);
	glBindTexture(GL_TEXTURE_3D, colorPositionsTexture);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_R, tp_w, tp_h, tp_d, 0, GL_R, GL_UNSIGNED_BYTE, texturePositionsData);
	glBindTexture(GL_TEXTURE_3D, 0);*/
}

void bindBuffers()
{
	GLfloat vertices[] = {   
		 0.5f, -0.5f, 0.0f, 1.0, 0.0,
		-0.5f, -0.5f, 0.0f, 0.0, 0.0,
		 0.0f,  0.5f, 0.0f, 0.5, 1.0,
	};
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	size_t stride = 5 * sizeof(GLfloat);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, BUFFER_OFFSET(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

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

	window = SDL_CreateWindow(NULL, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w_width, w_height, SDL_WINDOW_OPENGL);
	if (window == NULL)
	{
		std::cerr << "\nERROR:We can\'t create window " << SDL_GetError();
		return false;
	}

	SDL_GLContext context = SDL_GL_CreateContext(window);
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