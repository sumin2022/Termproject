#define _CRT_SECURE_NO_WARNINGS
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <gl/glm/glm.hpp>
#include <gl/glm/ext.hpp>
#include <gl/glm/gtc/matrix_transform.hpp>
#include <iostream>
#include "filetobuf.h"
#include <GL/glu.h>

void make_vertexShaders();
void make_fragmentShaders();
void InitBuffer();
GLuint make_shaderProgram();
GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Timer(int value);


GLint width, height;
GLuint shaderProgramID; //--- 세이더 프로그램 이름
GLuint vertexShader; //--- 버텍스 세이더 객체
GLuint fragmentShader; //--- 프래그먼트 세이더 객체

void ChangeWtoOpenGL(int windowx, int windowy, int windowwidth, int windowheight);
double xopenGL = 0;
double yopenGL = 0;

void UpdateUniforms(const glm::mat4& model, const glm::vec3& color);

GLvoid Keyboard(unsigned char key, int x, int y);



GLuint vao, vbo[2];


float vertices[] = {
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
	0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  1.0f,

	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f, -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f, -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

	0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
};
float Botton[] = {	//바닥 (임시), 자동차 세로 크기:50, 가로:30, 전체 2000x2000
	2000.0f, 0.0f,  2000.0f,   0.0f, 1.0f, 0.0f,  // P1
	2000.0f, 0.0f, -2000.0f,   0.0f, 1.0f, 0.0f,  // P2
   -2000.0f, 0.0f,  2000.0f,   0.0f, 1.0f, 0.0f,  // P3

	2000.0f, 0.0f, -2000.0f,   0.0f, 1.0f, 0.0f,  // P2
   -2000.0f, 0.0f, -2000.0f,   0.0f, 1.0f, 0.0f,  // P4
   -2000.0f, 0.0f,  2000.0f,   0.0f, 1.0f, 0.0f   // P3
};



void main(int argc, char** argv) {

	width = 800;
	height = 800;

	glutInit(&argc, argv); // glut 초기화
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH); // 디스플레이 모드 설정
	glutInitWindowPosition(100, 100); // 윈도우의 위치 지정
	glutInitWindowSize(width, height); // 윈도우의 크기 지정
	glutCreateWindow("Example1"); // 윈도우 생성


	//--- GLEW 초기화하기
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) // glew 초기화
	{
		std::cerr << "Unable to initialize GLEW" << std::endl
			;
		exit(EXIT_FAILURE);
	}
	else
		std::cout << "GLEW Initialized\n";

	srand(time(NULL));


	//--- 세이더 읽어와서 세이더 프로그램 만들기
	make_vertexShaders(); //--- 버텍스 세이더 만들기
	make_fragmentShaders(); //--- 프래그먼트 세이더 만들기
	shaderProgramID = make_shaderProgram();
	//--- 세이더 프로그램 만들기
	make_shaderProgram();
	InitBuffer();


	glutDisplayFunc(drawScene); // 출력 함수의 지정
	glutReshapeFunc(Reshape); // 다시 그리기 함수 지정
	glutMainLoop();
}

GLvoid drawScene() {

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW); //CW:시계방향 ,CCW:반시계방향

	GLfloat rColor, gColor, bColor;
	rColor = gColor = 0.0;
	bColor = 1.0; //--- 배경색을 파랑색으로 설정
	glClearColor(0.2, 0.2, 0.2, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(shaderProgramID);

	glutKeyboardFunc(Keyboard);		//키보드 입력 처리 콜백 함수

	glm::vec3 cameraPos = glm::vec3(0.0f, 2.0f, -30.0f);	//--- 카메라위치
	glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, 0.0f);   //--- 카메라바라보는방향
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);   //--- 카메라위쪽방향
	glm::mat4 view = glm::lookAt(cameraPos, cameraDirection, cameraUp);

	unsigned int viewLocation = glGetUniformLocation(shaderProgramID, "viewTransform");
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);

	glm::mat4 projection = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 50.0f);
	//projection = glm::translate(projection, glm::vec3(0.0, 0.0, -2.0));
	// 투영 행렬을 셰이더로 전달
	unsigned int projectionLocation = glGetUniformLocation(shaderProgramID, "projectionTransform");
	glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);

	glm::mat4 Botton = glm::mat4(1.0f);
	UpdateUniforms(Botton, glm::vec3(0.0f, 1.0f, 0.2f)); //업데이트 연두색

	glBindVertexArray(vao);				//바닥(임시)
	glDrawArrays(GL_TRIANGLES, 0, 6);


	glutSwapBuffers();

}

GLvoid Reshape(int w, int h) {
	glViewport(0, 0, w, h);
}

void UpdateUniforms(const glm::mat4& model, const glm::vec3& color) { //model값과 objectcolor값 전달함수
	// 변환 행렬 업데이트
	unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "modelTransform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

	// 색상 업데이트
	unsigned int colorLocation = glGetUniformLocation(shaderProgramID, "objectColor");
	glUniform3fv(colorLocation, 1, glm::value_ptr(color));
}

void make_vertexShaders()
{
	GLchar* vertexSource;
	//--- 버텍스 세이더 읽어 저장하고 컴파일 하기
	//--- filetobuf: 사용자정의 함수로 텍스트를 읽어서 문자열에 저장하는 함수
	vertexSource = filetobuf("vertex.glsl");
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	glCompileShader(vertexShader);
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
		std::cerr << "ERROR: vertex shader 컴파일 실패\n" << errorLog << std::endl;
		return;
	}
}

void make_fragmentShaders()
{
	GLchar* fragmentSource;
	//--- 프래그먼트 세이더 읽어 저장하고 컴파일하기
	fragmentSource = filetobuf("fragment.glsl"); // 프래그세이더 읽어오기
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader);
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
		std::cerr << "ERROR: frag_shader 컴파일 실패\n" << errorLog << std::endl;
		return;
	}
}

GLuint make_shaderProgram()
{
	GLuint shaderID;
	shaderID = glCreateProgram(); //--- 세이더 프로그램 만들기
	glAttachShader(shaderID, vertexShader); //--- 세이더 프로그램에 버텍스 세이더 붙이기
	glAttachShader(shaderID, fragmentShader); //--- 세이더 프로그램에 프래그먼트 세이더 붙이기
	glLinkProgram(shaderID); //--- 세이더 프로그램 링크하기
	glDeleteShader(vertexShader); //--- 세이더 객체를 세이더 프로그램에 링크했음으로, 세이더 객체 자체는 삭제 가능
	glDeleteShader(fragmentShader);
	GLint result;
	GLchar errorLog[512];
	glGetProgramiv(shaderID, GL_LINK_STATUS, &result); // ---세이더가 잘 연결되었는지 체크하기
	if (!result) {
		glGetProgramInfoLog(shaderID, 512, NULL, errorLog);
		std::cerr << "ERROR: shader program 연결 실패\n" << errorLog << std::endl;
		return false;
	}
	glUseProgram(shaderID); //--- 만들어진 세이더 프로그램 사용하기
	//--- 여러 개의 세이더프로그램 만들 수 있고, 그 중 한개의 프로그램을 사용하려면
	//--- glUseProgram 함수를 호출하여 사용 할 특정 프로그램을 지정한다.
	//--- 사용하기 직전에 호출할 수 있다.
	return shaderID;
}




void InitBuffer()
{
	glGenVertexArrays(1, &vao); // VAO 생성 및 바인딩
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo[0]);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Botton), Botton, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);


	glUseProgram(shaderProgramID);
	unsigned int lightPosLocation = glGetUniformLocation(shaderProgramID, "lightPos");
	glUniform3f(lightPosLocation, 0.0f, 20.0f, 0.0f);
	int lightColorLocation = glGetUniformLocation(shaderProgramID, "lightColor");
	glUniform3f(lightColorLocation, 1.0, 1.0, 1.0);



}



GLvoid Keyboard(unsigned char key, int x, int y) {

	glutPostRedisplay();	//누를 때마다 변화하게 출력하는 콜백함수
}



GLvoid Timer(int value) {	//타이머 함수에서 램덤으로 색깔을 출력해줌

	glutTimerFunc(16, Timer, 0);
	glutPostRedisplay();


}


void ChangeWtoOpenGL(int windowx, int windowy, int windowwidth, int windowheight) { //좌표 변환(일단 넣어둠)
	xopenGL = (2.0f * windowx / windowwidth) - 1.0f;
	yopenGL = 1.0f - (2.0f * windowy / windowheight);
	printf("%0.4f, %0.4f\n", xopenGL, yopenGL);

}
