/**/#define _CRT_SECURE_NO_WARNINGS 
#define GLM_FORCE_RADIANS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <gl/glm/glm.hpp>
#include <gl/glm/ext.hpp>
#include <gl/glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <ctime>
#include "filetobuf.h"

// stb_image 포함
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// 셰이더 소스 코드
GLchar* vertexSource, * fragmentSource; // 셰이더 소스 코드
GLuint vertexShader, fragmentShader;    // 셰이더 객체
GLuint shaderProgramID;                 // 셰이더 프로그램
GLuint vao_axes, vao_cube, vao_pyramid; // VAO들
GLuint vbo_axes, vbo_cube, vbo_pyramid; // VBO들
GLuint ebo_cube, ebo_pyramid;           // EBO들

int window_width = 1000, window_height = 800;

// 현재 선택된 객체 (1: 육면체, 2: 사각뿔)
int currentObject = 1;

// 회전 각도 (초기값)
float rotationX = 30.0f;
float rotationY = -30.0f;

// 회전 애니메이션 속도
float rotationSpeedX = 0.0f;
float rotationSpeedY = 0.0f;

// 와이어프레임 모드 여부
bool isWireframe = false;

// 은면 제거 여부
bool cullFace = false;

// 텍스처 ID
GLuint cubeTextures[6];

GLfloat floorVertices[] = {
    // Position                  // Color          // TexCoords         // Normal
    -1000.0f, 0.0f,  1000.0f,   1.0f, 0.0f, 0.0f,   0.0f, 0.0f,  0.0f,  1.0f,  0.0f, // 정점 0
     1000.0f, 0.0f,  1000.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,  0.0f,  1.0f,  0.0f, // 정점 1
     1000.0f, 0.0f, -1000.0f,   0.0f, 0.0f, 1.0f,   1.0f, 1.0f,  0.0f,  1.0f,  0.0f, // 정점 2
    -1000.0f, 0.0f, -1000.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f,  0.0f,  1.0f,  0.0f  // 정점 3
};
GLuint floorIndices[] = {
    0, 1, 2,
    2, 3, 0
};

// 버텍스 셰이더 컴파일 함수
void make_vertexShaders() {
    vertexSource = filetobuf("vertex.glsl");
    if (!vertexSource) exit(EXIT_FAILURE);
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, (const GLchar**)&vertexSource, 0);
    glCompileShader(vertexShader);
    GLint result;
    GLchar errorLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
    if (!result) {
        glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
        fprintf(stderr, "ERROR: vertex shader 컴파일 실패\n%s\n", errorLog);
        exit(EXIT_FAILURE);
    }
    free(vertexSource);
}

// 프래그먼트 셰이더 컴파일 함수
void make_fragmentShaders() {
    fragmentSource = filetobuf("fragment.glsl");
    if (!fragmentSource) exit(EXIT_FAILURE);
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, (const GLchar**)&fragmentSource, 0);
    glCompileShader(fragmentShader);
    GLint result;
    GLchar errorLog[512];
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
    if (!result) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
        fprintf(stderr, "ERROR: fragment shader 컴파일 실패\n%s\n", errorLog);
        exit(EXIT_FAILURE);
    }
    free(fragmentSource);
}

// 셰이더 프로그램 생성 함수
void make_shaderProgram() {
    make_vertexShaders();
    make_fragmentShaders();

    shaderProgramID = glCreateProgram();
    glAttachShader(shaderProgramID, vertexShader);
    glAttachShader(shaderProgramID, fragmentShader);
    glLinkProgram(shaderProgramID);

    GLint result;
    GLchar errorLog[512];
    glGetProgramiv(shaderProgramID, GL_LINK_STATUS, &result);
    if (!result) {
        glGetProgramInfoLog(shaderProgramID, 512, NULL, errorLog);
        fprintf(stderr, "ERROR: 셰이더 프로그램 링크 실패\n%s\n", errorLog);
        exit(EXIT_FAILURE);
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    glUseProgram(shaderProgramID);
}

// 텍스처 로딩 함수
GLuint LoadTexture(const char* path) {
    GLuint textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    // 이미지 로드
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data) {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        // 텍스처 바인딩 및 설정
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        // 텍스처 파라미터 설정
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

// 버퍼 초기화 함수
void InitBuffer() {

    // Cube VAO
    glGenVertexArrays(1, &vao_cube);
    glBindVertexArray(vao_cube);

    // VBO for cube
    glGenBuffers(1, &vbo_cube);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_cube);
    glBufferData(GL_ARRAY_BUFFER, sizeof(floorVertices), floorVertices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    // Color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    // TexCoord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    // Normal attribute
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(8 * sizeof(GLfloat)));
    glEnableVertexAttribArray(3);

    // EBO for cube
    glGenBuffers(1, &ebo_cube);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_cube);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(floorIndices), floorIndices, GL_STATIC_DRAW);

    glBindVertexArray(0);
}

// 객체의 모델 변환 행렬을 설정하는 함수
void SetModelTransform(glm::mat4 model) {
    glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 7.0f), // 카메라 위치
        glm::vec3(0.0f, 0.0f, 0.0f), // 카메라 타겟
        glm::vec3(0.0f, 1.0f, 0.0f)); // 업 벡터
    glm::mat4 projection = glm::perspective(glm::radians(45.0f),
        (float)window_width / window_height,
        0.1f, 100.0f);
    glm::mat4 modelTransform = projection * view * model;

    GLuint modelTransformLoc = glGetUniformLocation(shaderProgramID, "modelTransform");
    glUniformMatrix4fv(modelTransformLoc, 1, GL_FALSE, glm::value_ptr(modelTransform));
}

// 육면체 그리기 함수
void drawCube() {
    glBindVertexArray(vao_cube);
    glUseProgram(shaderProgramID);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, cubeTextures[0]);

    // 인덱스 오프셋 계산 (각 면마다 6개의 인덱스)
    GLsizei offset = 0 * 6 * sizeof(GLuint);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (GLvoid*)offset);

    glBindVertexArray(0);
}

// 장면 그리기 함수
void drawScene() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shaderProgramID);


    // 뷰와 투영 행렬 설정
    glm::vec3 cameraPos = glm::vec3(0.0f, 2.0f, -30.0f); // 카메라 위치
    glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, 0.0f); // 카메라 방향
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f); // 카메라 위쪽

    glm::mat4 view = glm::lookAt(cameraPos, cameraDirection, cameraUp);
    unsigned int viewLocation = glGetUniformLocation(shaderProgramID, "viewTransform");
    glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);

    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 50.0f);
    unsigned int projectionLocation = glGetUniformLocation(shaderProgramID, "projectionTransform");
    glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);

    // 객체 회전 업데이트
    rotationX += rotationSpeedX;
    rotationY += rotationSpeedY;

    // 텍스처 사용 안 함
    GLint useTextureLoc = glGetUniformLocation(shaderProgramID, "useTexture");
    glUniform1i(useTextureLoc, 0);

    glBindVertexArray(0);

    // 모델 행렬 설정
    glm::mat4 model_cube = glm::mat4(1.0f);
    model_cube = glm::rotate(model_cube, glm::radians(rotationX), glm::vec3(1, 0, 0));
    model_cube = glm::rotate(model_cube, glm::radians(rotationY), glm::vec3(0, 1, 0));
    SetModelTransform(model_cube);

    // 텍스처 사용
    glUniform1i(useTextureLoc, 1);

    // 육면체 그리기
    drawCube();

    glutSwapBuffers();
}

// 키보드 입력 콜백 함수
void Keyboard(unsigned char key, int x, int y) {
    switch (key) {
    default:
        break;
    }
    glutPostRedisplay();
}

// 특수 키 입력 콜백 함수 (화살표 키)
void SpecialKeys(int key, int x, int y) {
    switch (key) {
    default:
        break;
    }
    glutPostRedisplay();
}

// 타이머 함수 (애니메이션 업데이트)
void Timer(int value) {
    // 객체 회전 업데이트는 이미 drawScene에서 처리
    glutPostRedisplay();
    glutTimerFunc(16, Timer, 0); // 약 60fps
}

// 리사이즈 콜백 함수
void Reshape(int w, int h) {
    window_width = w;
    window_height = h;
    glViewport(0, 0, w, h);
}

// 초기화 함수
void Initialize() {
    // 셰이더 프로그램 생성
    make_shaderProgram();

    // 버퍼 초기화
    InitBuffer();

    // 텍스처 로드
    // 육면체 텍스처 로드 (6개)
    cubeTextures[0] = LoadTexture("map.png");   // Front face
    cubeTextures[1] = LoadTexture("map.png"); // Back face
    cubeTextures[2] = LoadTexture("map.png"); // Left face
    cubeTextures[3] = LoadTexture("map.png"); // Right face
    cubeTextures[4] = LoadTexture("map.png"); // Top face
    cubeTextures[5] = LoadTexture("map.png"); // Bottom face

    // 텍스처 유니폼 설정 (단일 텍스처 샘플러 사용)
    glUseProgram(shaderProgramID);
    glUniform1i(glGetUniformLocation(shaderProgramID, "texture1"), 0); // 텍스처 유닛 0에 바인딩

    // 조명 유니폼 설정
    GLint ambientOnLoc = glGetUniformLocation(shaderProgramID, "ambientOn");
    GLint lightColorLoc = glGetUniformLocation(shaderProgramID, "lightColor");
    GLint lightPosLoc = glGetUniformLocation(shaderProgramID, "lightPos");
    GLint viewPosLoc = glGetUniformLocation(shaderProgramID, "viewPos");
    GLint ambientColorLoc = glGetUniformLocation(shaderProgramID, "ambientColor");

    if (ambientOnLoc == -1 || lightColorLoc == -1 || lightPosLoc == -1 || viewPosLoc == -1 || ambientColorLoc == -1) {
        std::cerr << "ERROR: 셰이더에서 필요한 유니폼 변수를 찾을 수 없습니다." << std::endl;
    }

    glUniform1i(ambientOnLoc, 1); // 주변광 활성화
    glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f); // 흰색 조명
    glUniform3f(lightPosLoc, 5.0f, 5.0f, 5.0f); // 조명 위치
    glUniform3f(viewPosLoc, 0.0f, 0.0f, 7.0f); // 카메라 위치
    glUniform3f(ambientColorLoc, 1.0f, 1.0f, 1.0f); // 주변광 색상

    // 배경색 설정
    glClearColor(0.8f, 0.8f, 0.8f, 1.0f); // 회색 배경

    // 깊이 테스트 활성화
    glEnable(GL_DEPTH_TEST);
}

// 메인 함수
int main(int argc, char** argv) {
    srand((unsigned int)time(NULL));

    // GLUT 초기화
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(window_width, window_height);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Example 29");

    // GLEW 초기화
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "GLEW 초기화 실패\n");
        return EXIT_FAILURE;
    }

    // 초기화 함수 호출
    Initialize();

    // 콜백 함수 등록
    glutDisplayFunc(drawScene);
    glutReshapeFunc(Reshape);
    glutKeyboardFunc(Keyboard);
    glutSpecialFunc(SpecialKeys);
    glutTimerFunc(0, Timer, 0);

    // 메인 루프 실행
    glutMainLoop();

    return 0;
}
/**/