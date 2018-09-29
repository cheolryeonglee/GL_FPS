/////////////////////////////////////
// FPS Moving
/////////////////////////////////////
using namespace std;

#include "vgl.h"
#include "LoadShaders.h"
#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"

enum VAO_IDs { Triangles, NumVAOs };
enum Buffer_IDs { ArrayBuffer, NumBuffers };
enum Attrib_IDs { vPosition = 0 };

GLuint VAOs[NumVAOs];
GLuint Buffers[NumBuffers];
GLuint location;
GLuint uCameraMatrix;
GLuint uProjectionMatrix;

const GLuint WIDTH = 640;
const GLuint HEIGHT = 640;

const GLuint NumVertices = 4;

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float pitch = 0.0f;
float yaw = 0.0f;

//

float deltaTime = 0.0f;
float lastFrame = 0.0f;

float time = 0.0f;

float cameraSpeed = 0.0f;

void init(void)
{
	ShaderInfo shaders[] = {
		{ GL_VERTEX_SHADER, "triangles.vert" },
		{ GL_FRAGMENT_SHADER, "triangles.frag" },
		{ GL_NONE, NULL }
	};

	GLuint program = LoadShaders(shaders);
	glUseProgram(program);

	GLfloat vertices[NumVertices][3] = {
		{ -0.9, 0.0, -0.9 }, 
		{  0.9, 0.0, -0.9 },
		{  0.9, 0.0,  0.9 },
		{ -0.9, 0.0,  0.9 }
	};

	GLfloat colorData[NumVertices][3] = {
		{ 1,0,0 },
		{ 0,1,0 },
		{ 0,0,1 },
		{ 1,1,1 } 
	};

	glGenBuffers(2, Buffers);
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices),	vertices, GL_STATIC_DRAW);
	glBindAttribLocation(program, 0, "vPosition");
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, Buffers[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colorData), colorData, GL_STATIC_DRAW);
	glBindAttribLocation(program, 1, "vertexColor");
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(1);
	location = glGetUniformLocation(program, "model_matrix");
	uCameraMatrix = glGetUniformLocation(program, "camera_matrix");
	uProjectionMatrix = glGetUniformLocation(program, "projection_matrix");
}
#include <iostream>

void drawScene(void)
{
	glClear(GL_COLOR_BUFFER_BIT);

	glm::mat4 model_view = glm::rotate(glm::mat4(1.0), 0.0f, glm::vec3(0.0f, 0.0f, 1.0f));
	glUniformMatrix4fv(location, 1, GL_FALSE, &model_view[0][0]);
	
	// Use deltaTime because glutGet(GL_ELAPSED_TIME) is too fast
	deltaTime = time - lastFrame;
	lastFrame = time;

	cameraSpeed = 10.0f * deltaTime;

	glm::mat4 camera_matrix = glm::lookAt(
		cameraPos, 
		cameraPos + cameraFront, 
		cameraUp);
	glUniformMatrix4fv(uCameraMatrix, 1, GL_FALSE, &camera_matrix[0][0]);

	glm::mat4 projection_matrix = glm::perspective(glm::radians(45.0f), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
	glUniformMatrix4fv(uProjectionMatrix, 1, GL_FALSE, &projection_matrix[0][0]);

	glDrawArrays(GL_QUADS, 0, NumVertices);
	
	glFlush();
}

void runEveryFrame()
{
	time += 0.001f;
	glutPostRedisplay();
}

void keyboard(GLubyte key, GLint x, GLint y) {
	if (key == 'w')
		cameraPos += cameraSpeed * cameraFront;
	if (key == 's')
		cameraPos -= cameraSpeed * cameraFront;
	if (key == 'a')
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (key == 'd')
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

bool firstMouse = true;
int lastX = WIDTH / 2, lastY = HEIGHT / 2;

void mouse(int x, int y) {

	if (firstMouse) {
		lastX = x;
		lastY = y;
		firstMouse = false;
	}

	int xoffset = x - lastX;
	int yoffset = lastY - y;
	lastX = x;
	lastY = y;

	yaw += xoffset;
	pitch += yoffset;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 direction;

	direction.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
	direction.y = sin(glm::radians(pitch));
	direction.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));

	printf("(%f, %f, %f)\n", direction.x, direction.y, direction.z);

	cameraFront = glm::normalize(direction);
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutCreateWindow("Hello World");
	glewInit();	

	init();

	glutDisplayFunc(drawScene);

	glutKeyboardFunc(keyboard);

	glutPassiveMotionFunc(mouse);

	glutIdleFunc(runEveryFrame);

	glutMainLoop();
}
