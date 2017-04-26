#version 330 core

in vec2 UV;
in vec3 Normal;

out vec4 color;

uniform vec3 LightDirection;
uniform vec4 Tint;
uniform sampler2D Sampler;

void main() {
	vec3 texel = texture( Sampler, UV ).rgb * Tint.rgb;
	float theta = clamp(dot( Normal, LightDirection ), 0.1, 0.9);
	color = vec4(texel * theta , Tint.a);
}