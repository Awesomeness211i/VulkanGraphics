#version 450

//Inputs
layout(location = 0) in vec4 color;
layout(location = 1) in vec4 worldPosition;
layout(location = 2) in vec4 worldNormal;

//Outputs
layout(location = 0) out vec4 o_Color;

layout(set = 0, binding = 0) uniform GlobalUBO {
	mat4 projectionViewMatrix;
	vec4 ambientLightColor; //4th component is light intensity
	vec4 lightPosition; //ignore 4th component
	vec4 lightColor; //4th component is light intensity
} ubo;

layout(push_constant) uniform Push {
	mat4 modelMatrix;
	mat4 normalMatrix;
} push;

void main() {
	vec3 directionToLight = (ubo.lightPosition - worldPosition).xyz;
	float attenuation = 1 / dot(directionToLight, directionToLight);

	vec3 lightColor = ubo.lightColor.xyz * ubo.lightColor.w * attenuation;
	vec3 ambientLight = ubo.ambientLightColor.xyz * ubo.ambientLightColor.w;
	vec3 diffuseLight = lightColor * max(dot(normalize(worldNormal.xyz), normalize(directionToLight)), 0);

	o_Color = vec4(diffuseLight + ambientLight, 1.0) * color;
}