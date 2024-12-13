
#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec3 VertexColor;
in vec2 TexCoord;

out vec4 FragColor;

uniform bool useTexture;
uniform bool ambientOn;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 ambientColor;
uniform sampler2D texture1;

void main() {
    if(useTexture) {
        // 조명 계산
        vec3 norm = normalize(Normal);
        vec3 lightDir = normalize(lightPos - FragPos);

        // Ambient
        vec3 ambient = ambientOn ? 1.0 * ambientColor : vec3(0.0);

        // Diffuse
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = diff * lightColor;

        // Specular
        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
        vec3 specular = 0.5 * spec * lightColor;

        // 텍스처 색상
        vec3 textureColor = texture(texture1, TexCoord).rgb;

        // 최종 색상
        vec3 result = (ambient + diffuse + specular) * textureColor;
        FragColor = vec4(result, 1.0);
    } else {
        FragColor = vec4(VertexColor, 1.0);
    }
}