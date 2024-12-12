#version 330 core

layout (location = 0) in vec3 vPos; 
layout (location = 1) in vec3 vNormal;

uniform mat4 modelTransform;
uniform mat4 viewTransform;
uniform mat4 projectionTransform;

out vec3 FragPos; // 객체의 위치값을 프래그먼트 쉐이더로 보냄
out vec3 Normal;  // 노멀값을 프래그먼트 쉐이더로 보냄


void main()
{
	gl_Position =  projectionTransform * viewTransform * modelTransform * vec4 (vPos, 1.0f);
	FragPos = vec3(modelTransform * vec4(vPos, 1.0));	//월드 공간에 있는 버텍스 값을 프래그먼트로 보냄
	Normal = vNormal;		 // 노멀값을 프래그먼트 쉐이더로 보냄

}