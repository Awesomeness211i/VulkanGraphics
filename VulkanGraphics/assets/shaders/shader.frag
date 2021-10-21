#version 450

//Inputs
layout(location = 0) in vec4 color;

//Outputs
layout(location = 0) out vec4 o_Color;

layout(push_constant) uniform Push {
	mat4 modelMatrix;
	mat4 normalMatrix;
} push;

void main() {
	o_Color = color;
}