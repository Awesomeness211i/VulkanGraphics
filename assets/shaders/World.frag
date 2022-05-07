#version 450

//Inputs
layout(location = 0) in vec4 color;
layout(location = 1) in vec4 worldPosition;
layout(location = 2) in vec4 worldNormal;

//Outputs
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
	mat4 modelMatrix;
	mat4 normalMatrix;
} push;

void main() {
	vec3 diffuseLight = ubo.ambientLightColor.xyz * ubo.ambientLightColor.w;
	vec3 specularLight = vec3(0.0);
	vec3 surfaceNormal = normalize(worldNormal.xyz);

	vec3 cameraPosWorld = ubo.inverseViewMatrix[3].xyz;
	vec3 viewDirection = normalize(cameraPosWorld - worldPosition.xyz);

	for (int i = 0; i < ubo.numLights; i++) {
		PointLight light = ubo.pointLights[i];
		vec3 directionToLight = (light.position - worldPosition).xyz;
		float attenuation = 1 / dot(directionToLight, directionToLight);
		directionToLight = normalize(directionToLight);

		float cosAngleIncidence = max(dot(surfaceNormal, directionToLight), 0);
		vec3 intensity = light.color.xyz * light.color.w * attenuation;

		diffuseLight += intensity * cosAngleIncidence;
		//specular lighting
		vec3 halfAngle = normalize(directionToLight + viewDirection);
		float blinnTerm = dot(surfaceNormal, halfAngle);
		blinnTerm = clamp(blinnTerm, 0, 1);
		blinnTerm = pow(blinnTerm, 32.0); //High values = sharp highlight
		specularLight += intensity * blinnTerm;
	}
	o_Color = vec4(diffuseLight * color.xyz + specularLight * color.xyz, 1.0);
}
