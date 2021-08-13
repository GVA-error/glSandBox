#pragma once

static const char* vertexShaderSource = R"(
#version 300 es
precision highp float;
precision highp sampler3D;

layout (location = 0) in vec3 position;
out vec3 vColor;

void main()
{
	gl_Position = vec4(position, 1.0);
	vColor = vec3(1.0);
}

)";