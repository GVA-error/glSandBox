#pragma once

static const char* fragmentShaderSource = R"(
#version 300 es
precision highp float;
precision highp sampler3D;

in vec3 vColor;
in vec2 vUV;

uniform sampler3D colorTexture;
uniform sampler3D proportionTextur;

uniform vec2 WindowSize;

out vec4 color;

void main()
{
	vec3 uvw = vec3(vUV, 0.0);
	vec3 textColor = texture(colorTexture, uvw).rgb;	

    color = vec4(textColor, 1.0);
}

)";