#version 450

layout (location = 0) in vec2 fragOffset;

layout(location = 0) out vec4 o_Color;

struct PointLight {
	vec4 position; //ignore w
	vec4 color; // w is intensity
};

layout(set = 0, binding = 0) uniform GlobalUBO {
	mat4 projectionMatrix;
	mat4 viewMatrix;
	mat4 inverseViewMatrix;
	vec4 ambientLightColor; //4th component is light intensity
	PointLight pointLights[10];
	int numLights;
} ubo;

layout(push_constant) uniform Push {
	vec4 position;
	vec4 color;
	float radius;
} push;

const float M_PI = 3.14159265358979;

void main() {
	float distance = sqrt(dot(fragOffset, fragOffset));
	if(distance > 1.0) {
		discard;
	}
	float cosDis = 0.5 * (cos(distance * M_PI) + 1);
	o_Color = vec4(push.color.xyz + cosDis, cosDis);
}
