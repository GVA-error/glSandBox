#pragma once

static const char* vertexShaderSource = R"(
#version 300 es
precision highp float;
precision highp sampler3D;

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
out vec3 ourColor;

void main()
{
	gl_Position = vec4(position, 1.0);
	ourColor = color;
}

)";