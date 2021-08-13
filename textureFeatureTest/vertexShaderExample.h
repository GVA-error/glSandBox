#pragma once

static const char* vertexShaderSource = R"(
#version 300 es
precision highp float;
precision highp sampler3D;

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv;
out vec3 vColor;
out vec2 vUV;

void main()
{
	gl_Position = vec4(position, 1.0);
	vColor = vec3(1.0);
	vUV = uv;
}

)";