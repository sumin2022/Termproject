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

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// ���� ���� ���� �� �Լ���
GLchar* vertexSource, * fragmentSource;
GLuint vertexShader, fragmentShader;
GLuint shaderProgramID;
GLuint vao_axes, vao_cube, vao_pyramid;
GLuint vbo_axes, vbo_cube, vbo_pyramid;
GLuint ebo_cube, ebo_pyramid;

int window_width = 1000, window_height = 800;
int currentObject = 1;
float rotationX = 30.0f;
float rotationY = 10.0f;
float rotationSpeedX = 0.0f;
float rotationSpeedY = 0.0f;
float camerax = 50.0f, cameray = 20.0f, cameraz = 2000.0f; //���������� �̷��Ե�
float cameraAngleY = 0.0f; //ī�޶� y�� ȸ��
bool isWireframe = false;
bool cullFace = false;
GLuint cubeTextures[6];

GLfloat floorVertices[] = { //��ǥ�� ������ 
    // Position                  // Color         // TexCoords  // Normal
     0.0f,    0.0f,  2000.0f,   1.0f,0.0f,0.0f,  0.0f,0.0f,   0.0f,1.0f,0.0f,
     2000.0f, 0.0f,  2000.0f,   0.0f,1.0f,0.0f,  1.0f,0.0f,   0.0f,1.0f,0.0f,
     2000.0f, 0.0f,  0.0f,   0.0f,0.0f,1.0f,  1.0f,1.0f,   0.0f,1.0f,0.0f,
     0.0f,    0.0f,  0.0f,   1.0f,1.0f,0.0f,  0.0f,1.0f,   0.0f,1.0f,0.0f
};
GLuint floorIndices[] = {
    0, 1, 2,
    2, 3, 0
};

// ������ ���� (x, y, z, r, g, b, u, v, nx, ny, nz) ���� ����
GLfloat hexVertices[] = {
    // Position         Color            TexCoords Normal
    0.0f,   0.5f, 0.0f,   1.0f,0.0f,0.0f, 0.0f,0.0f,   0.0f,1.0f,0.0f,
    0.43f,  0.25f,0.0f,   1.0f,0.0f,0.0f, 0.5f,0.0f,   0.0f,1.0f,0.0f,
    0.43f, -0.25f,0.0f,   1.0f,0.0f,0.0f, 0.5f,1.0f,   0.0f,1.0f,0.0f,
    0.0f,  -0.5f, 0.0f,   1.0f,0.0f,0.0f, 1.0f,1.0f,   0.0f,1.0f,0.0f,
   -0.43f, -0.25f,0.0f,   1.0f,0.0f,0.0f, 1.0f,0.5f,   0.0f,1.0f,0.0f,
   -0.43f,  0.25f,0.0f,   1.0f,0.0f,0.0f, 0.5f,0.5f,   0.0f,1.0f,0.0f
};

GLuint hexIndices[] = {
    // �������� �����ϴ� 4���� �ﰢ��(�� 6���� �����̹Ƿ� 4���� �ﰢ������ Ŀ��)
    0,1,2,
    0,2,3,
    0,3,4,
    0,4,5
};


// ���̴� ���� �Լ���
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
        fprintf(stderr, "ERROR: vertex shader ������ ����\n%s\n", errorLog);
        exit(EXIT_FAILURE);
    }
    free(vertexSource);
}

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
        fprintf(stderr, "ERROR: fragment shader ������ ����\n%s\n", errorLog);
        exit(EXIT_FAILURE);
    }
    free(fragmentSource);
}

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
        fprintf(stderr, "ERROR: ���̴� ���α׷� ��ũ ����\n%s\n", errorLog);
        exit(EXIT_FAILURE);
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    glUseProgram(shaderProgramID);
}

// �ؽ�ó �ε� �Լ�
GLuint LoadTexture(const char* path) {
    GLuint textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data) {
        GLenum format = GL_RGB;
        if (nrComponents == 1) format = GL_RED;
        else if (nrComponents == 3) format = GL_RGB;
        else if (nrComponents == 4) format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        // �ؽ�ó �Ķ���� ����
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

// ���� �ʱ�ȭ �Լ�
void InitBuffer() {
    // �ٴ�(����ü) VAO
    glGenVertexArrays(1, &vao_cube);
    glBindVertexArray(vao_cube);

    glGenBuffers(1, &vbo_cube);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_cube);
    glBufferData(GL_ARRAY_BUFFER, sizeof(floorVertices), floorVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(8 * sizeof(GLfloat)));
    glEnableVertexAttribArray(3);

    glGenBuffers(1, &ebo_cube);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_cube);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(floorIndices), floorIndices, GL_STATIC_DRAW);

    glBindVertexArray(0);
}

// �� ��ȯ ��� ���� �Լ�
void SetModelTransform(glm::mat4 model) {
    GLuint modelTransformLoc = glGetUniformLocation(shaderProgramID, "modelTransform");
    glUniformMatrix4fv(modelTransformLoc, 1, GL_FALSE, glm::value_ptr(model)); 
}

// ����ü �׸��� �Լ�
void drawCube() {
    glBindVertexArray(vao_cube);
    glUseProgram(shaderProgramID);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, cubeTextures[0]);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (GLvoid*)0);

    glBindVertexArray(0);
}

// -----------------------------
// Car Ŭ���� ����
// -----------------------------
class Car {
public:
    GLuint vao, vbo, ebo;

    void Init() {
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(hexVertices), hexVertices, GL_STATIC_DRAW);

        // vertex attribute ���� (������ ����)
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
        glEnableVertexAttribArray(2);

        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(8 * sizeof(GLfloat)));
        glEnableVertexAttribArray(3);

        glGenBuffers(1, &ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(hexIndices), hexIndices, GL_STATIC_DRAW);

        glBindVertexArray(0);
    }


    void Draw(const glm::vec3& cameraPos) {
        glUseProgram(shaderProgramID);

        // �ؽ�ó ��� X (�ܻ�)
        GLint useTextureLoc = glGetUniformLocation(shaderProgramID, "useTexture");
        glUniform1i(useTextureLoc, 0);

        // ������Ʈ �÷� (����)
        GLint objectColorLoc = glGetUniformLocation(shaderProgramID, "objectColor");
        if (objectColorLoc != -1) {
            glUniform3f(objectColorLoc, 1.0f, 0.0f, 0.0f);
        }

        // �ڵ��� �� ��ȯ : ī�޶� (0,2,-30)���� (0,0,0)�� ���Ƿ�
        // �ڵ����� (0,0,-20)�� ��ġ��Ű�� ī�޶� ���� 10���� �� ������ �ְ� �˴ϴ�.
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 2.0f, -20.0f));
        // �ڵ��� ũ�� Ű���
        model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));

        SetModelTransform(model);

        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

};

// ���������� Car ��ü ����
Car carObj;

// ��� �׸��� �Լ�
void drawScene() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(shaderProgramID);

    glm::vec3 cameraPos = glm::vec3(camerax, cameray, cameraz);
    glm::vec3 cameraDirection = glm::vec3(camerax, 0.0f, 0.0f);
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

    glm::mat4 view = glm::lookAt(cameraPos, cameraDirection, cameraUp);
    unsigned int viewLocation = glGetUniformLocation(shaderProgramID, "viewTransform");
    glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);

    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)window_width / window_height, 0.1f, 2000.0f);
    unsigned int projectionLocation = glGetUniformLocation(shaderProgramID, "projectionTransform");
    glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);

    rotationX += rotationSpeedX;
    rotationY += rotationSpeedY;

    // ��(�ٴ�) ������ (�ؽ�ó ���)
    GLint useTextureLoc = glGetUniformLocation(shaderProgramID, "useTexture");
    glUniform1i(useTextureLoc, 1);

    glm::mat4 model_floor = glm::mat4(1.0f);
    //model_floor = glm::rotate(model_floor, glm::radians(rotationX), glm::vec3(1, 0, 0));
    //model_floor = glm::rotate(model_floor, glm::radians(rotationY), glm::vec3(0, 1, 0));
    SetModelTransform(model_floor);
    drawCube();

    // �ڵ��� ������ (ī�޶� ���̿� ���� y �̵�, �ܻ�)
    carObj.Draw(cameraPos);

    glutSwapBuffers();
}

// Ű���� �ݹ�
void Keyboard(unsigned char key, int x, int y) {
    if (key == 'q') {

    }
    switch (key) { //ī�޶� �������� �������� �ٲ� ����
    case 'y':
        cameraAngleY += glm::radians(5.0f);
        break;
   }
    //glutLeaveMainLoop();
    glutPostRedisplay();
}

// Ư�� Ű
void SpecialKeys(int key, int x, int y) {
    switch (key) {
    case GLUT_KEY_LEFT:
        camerax -= 5.0f;
        break;
    case GLUT_KEY_RIGHT:
        camerax += 5.0f;
        break;
    case GLUT_KEY_UP:
        cameraz -= 5.0f;
        break;
    case GLUT_KEY_DOWN:
        cameraz += 5.0f;
        break;
    default:
        break;
    }

    glutPostRedisplay();
}

// Ÿ�̸� �Լ�
void Timer(int value) {
    glutPostRedisplay();
    glutTimerFunc(16, Timer, 0);
}

// ��������
void Reshape(int w, int h) {
    window_width = w;
    window_height = h;
    glViewport(0, 0, w, h);
}

// �ʱ�ȭ �Լ�
void Initialize() {
    make_shaderProgram();
    InitBuffer();

    // �ؽ�ó �ε� (�� ����)
    GLuint commonTexture = LoadTexture("map.png");
    for (int i = 0; i < 6; i++) {
        cubeTextures[i] = commonTexture;
    }

    // ������ ����
    glUseProgram(shaderProgramID);
    glUniform1i(glGetUniformLocation(shaderProgramID, "texture1"), 0);

    GLint ambientOnLoc = glGetUniformLocation(shaderProgramID, "ambientOn");
    GLint lightColorLoc = glGetUniformLocation(shaderProgramID, "lightColor");
    GLint lightPosLoc = glGetUniformLocation(shaderProgramID, "lightPos");
    GLint viewPosLoc = glGetUniformLocation(shaderProgramID, "viewPos");
    GLint ambientColorLoc = glGetUniformLocation(shaderProgramID, "ambientColor");

    glUniform1i(ambientOnLoc, 1);
    glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f);
    glUniform3f(lightPosLoc, 5.0f, 5.0f, 5.0f);
    glUniform3f(viewPosLoc, 0.0f, 0.0f, 7.0f);
    glUniform3f(ambientColorLoc, 1.0f, 1.0f, 1.0f);

    // Car ��ü �ʱ�ȭ
    carObj.Init();

    glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
    glEnable(GL_DEPTH_TEST);
}

int main(int argc, char** argv) {
    srand((unsigned int)time(NULL));

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(window_width, window_height);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Car Example");

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "GLEW �ʱ�ȭ ����\n");
        return EXIT_FAILURE;
    }

    Initialize();

    glutDisplayFunc(drawScene);
    glutReshapeFunc(Reshape);
    glutKeyboardFunc(Keyboard);
    glutSpecialFunc(SpecialKeys);
    glutTimerFunc(0, Timer, 0);

    glutMainLoop();

    return 0;
}