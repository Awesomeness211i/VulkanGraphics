#version 450

//Inputs
layout(location = 0) in vec4 color;

//Outputs
layout(location = 0) out vec4 o_Color;

layout(push_constant) uniform Push {
	mat4 transform; //projection * view * model
	mat4 normalMatrix;
	//vec4 color;
} push;

void main() {
	o_Color = color;
}