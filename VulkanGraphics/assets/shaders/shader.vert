#version 450

//Inputs
layout(location = 0) in vec4 position;
layout(location = 1) in vec4 color;
layout(location = 2) in vec4 normal;
layout(location = 3) in vec2 uv;

//Outputs
layout(location = 0) out vec4 o_Color;

layout(push_constant) uniform Push {
	mat4 transform; //projection * view * model
	mat4 normalMatrix;
	//vec4 color;
} push;

const vec3 DIRECTION_TO_LIGHT = normalize(vec3(1.0, -3.0, -1.0));
const float AMBIENT = 0.02;

void main() {
	gl_Position = push.transform * position;

	vec3 normalWorldSpace = normalize((push.normalMatrix * normal).xyz);
	float lightIntensity = max(dot(normalWorldSpace, DIRECTION_TO_LIGHT), AMBIENT);

	o_Color = lightIntensity * color;
}