#pragma once

static const char* fragmentShaderSource = R"(
#version 300 es
precision highp float;
precision highp sampler3D;

in vec3 vColor;

out vec4 color;

void main()
{
    color = vec4(vColor, 0.0);
}

)";