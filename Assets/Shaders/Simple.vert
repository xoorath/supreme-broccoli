#version 330 core

layout(location = 0) in vec3 vertPosition;
layout(location = 1) in vec3 vertNormal;
layout(location = 2) in vec2 vertUV;

out vec2 UV;
out vec3 Normal;

uniform mat4 MP;

void main(){
	gl_Position =  MP *  vec4(vertPosition, 1.0);
	Normal = vertNormal;
	UV = vertUV;
}
