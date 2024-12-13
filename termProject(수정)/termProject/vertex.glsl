#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec2 texCoord;
layout(location = 3) in vec3 normal; // 법선 벡터 입력

out vec3 FragPos;
out vec3 Normal;
out vec3 VertexColor;
out vec2 TexCoord;

uniform mat4 modelTransform;
uniform mat4 viewTransform;
uniform mat4 projectionTransform;

void main()
{
    gl_Position =  projectionTransform * viewTransform * modelTransform * vec4 (position, 1.0f);
    FragPos = vec3(modelTransform * vec4(position, 1.0));
    Normal = mat3(transpose(inverse(modelTransform))) * normal; // 변환된 법선 벡터
    VertexColor = color;
    TexCoord = texCoord;
}