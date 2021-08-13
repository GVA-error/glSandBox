#pragma once

static const char* fragmentShaderSource = R"(
#version 300 es
precision highp float;
precision highp sampler3D;

in vec3 vColor;
in vec2 vUV;

uniform sampler3D colorTexture;
uniform sampler3D proportionTexture;

uniform vec2 WindowSize;

out vec4 color;

void main()
{
	float U = vUV.x;
	float V = vUV.y;
	float correctedU = texture(proportionTexture, vec3(U, 0.0, 0.0)).r;
	
	vec3 uvw = vec3(correctedU, V, 0.0);
	vec3 textColor = texture(colorTexture, uvw).rgb;	

    color = vec4(textColor, 1.0);
}

)";