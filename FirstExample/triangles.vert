#version 430 core
layout(location = 0) in vec4 vPosition;
layout(location = 1) in vec4 vertexColor;
uniform mat4 model_matrix;
uniform mat4 camera_matrix;
uniform mat4 projection_matrix;

out vec4 myColor;

void main()
{
	myColor = vertexColor;
	gl_Position = projection_matrix * camera_matrix * model_matrix * vPosition;

}
