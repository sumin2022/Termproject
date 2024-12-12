#version 330 core

in vec3 FragPos;	// 위치값
in vec3 Normal;		//버텍스세이더에서받은노멀값

out vec4 FragColor;  // 최종객체의색저장

uniform vec3 objectColor; // 색상 정보
uniform vec3 lightColor;  //조명 색상
uniform vec3 lightPos;   //조명 위치


void main ()
{

	float ambientLight = 0.3;	//조명 계수
	vec3 ambient = ambientLight * lightColor; 	//주변 조명값

	vec3 normalVector = normalize (Normal);

	vec3 lightDir = normalize(lightPos - FragPos); //표면과조명의위치로조명의방향을결정한다

	float diffuseLight = max(dot(normalVector, lightDir), 0.0);
	vec3 diffuse = diffuseLight * lightColor;


	vec3 result =  (ambient + diffuse) * objectColor;	//최종 객체 색상 설정


	FragColor = vec4 (result, 1.0);

}
