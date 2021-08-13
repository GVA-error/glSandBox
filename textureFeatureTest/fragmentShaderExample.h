#pragma once

static const char* fragmentShaderSource = R"(
#version 300 es
precision highp float;
precision highp sampler3D;

out vec4 color;
uniform vec4 ourColor;

void main()
{
    color = ourColor;
}

)";