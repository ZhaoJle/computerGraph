#pragma once
#ifndef  BEZIER_CURVE_H
#define BEZIER_CURVE_H

#include <GLFW\glfw3.h>
#include <GL/gl3w.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "imgui.h"
#include "imgui_impl_glfw_gl3.h"

#include <vector>
#include <cmath>
#include <iostream>


const char* vertexShaderSource = "#version 330 core\n"

"layout (location = 0) in vec3 aPos;\n"

"layout (location = 1) in vec3 aColor;\n"

"out vec3 ourColor;\n"

"void main() {\n"

"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"

"	ourColor = aColor;\n"

"}\0";

// 片段着色器

const char* fragmentShaderSource = "#version 330 core\n"

"out vec4 FragColor;\n"

"in vec3 ourColor;\n"

"void main() {\n"

"   FragColor = vec4(ourColor, 1.0f);\n"

"}\n\0";


std::vector<glm::vec2> pointSet;

// settings

class BezierCurve {

private:
	float delta = 0.0005;
	unsigned int shaderProgram;

	ImVec4 line_color;

	ImVec4 point_color;

	ImVec4 tangent_color;

	void init();

	void drawLine(float x1, float y1, float x2, float y2, ImVec4 color);

	void drawPoint(float x, float y, float pointSize, ImVec4 color);

	void drawBezierCurve(ImVec4 color);

	void drawTangent(std::vector<glm::vec2> points, float t, ImVec4 color);

public:

	BezierCurve(ImVec4 line_color, ImVec4 point_color, ImVec4 tangent_color):line_color(line_color), point_color(point_color), tangent_color(tangent_color)
	{
		init();
	}

	void draw();
};

void BezierCurve::init()
{
	gl3wInit();

	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);

	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);

	glCompileShader(vertexShader);

	int success;

	char infoLog[512];

	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

	if (!success)

	{

		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);

		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;

	}

	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);

	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

	if (!success)

	{

		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);

		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;

	}

	shaderProgram = glCreateProgram(); // 创建一个着色器程序对象

	glAttachShader(shaderProgram, vertexShader);

	glAttachShader(shaderProgram, fragmentShader);

	glLinkProgram(shaderProgram);


	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);

	if (!success) {

		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);

		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;

	}

	glDeleteShader(vertexShader);

	glDeleteShader(fragmentShader);



	glUseProgram(shaderProgram);

}

void BezierCurve::drawLine(float x1, float y1, float x2, float y2, ImVec4 color)
{
	float vertice[12] = {

	x1, y1, 0, color.x,  color.y, color.z,

	x2, y2, 0, color.x,  color.y, color.z

	};

	unsigned int VBO;

	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertice), vertice, GL_STATIC_DRAW);

	unsigned int VAO;

	glGenVertexArrays(1, &VAO);

	glBindVertexArray(VAO);


	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);

	glEnableVertexAttribArray(0);


	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

	glEnableVertexAttribArray(1);



	//-----------------------------------

	glUseProgram(shaderProgram);

	glBindVertexArray(VAO);

	glPointSize(1.0f);

	glDrawArrays(GL_LINE_STRIP, 0, 2);


	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);



	glDeleteVertexArrays(1, &VAO);

	glDeleteBuffers(1, &VBO);
}

void BezierCurve::drawPoint(float x, float y, float pointSize, ImVec4 color)
{
	float vertice[6] = { x, y, 0, color.x,  color.y, color.z };



	unsigned int VBO;

	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertice), vertice, GL_STATIC_DRAW);

	unsigned int VAO;

	glGenVertexArrays(1, &VAO);

	glBindVertexArray(VAO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);

	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

	glEnableVertexAttribArray(1);



	//-----------------------------------

	glUseProgram(shaderProgram);

	glBindVertexArray(VAO);

	glPointSize(pointSize);

	glDrawArrays(GL_POINTS, 0, 1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);



	glDeleteVertexArrays(1, &VAO);

	glDeleteBuffers(1, &VBO);
}

void BezierCurve::drawBezierCurve(ImVec4 color)
{
	int size = pointSet.size();

	int n = size - 1;

	float* B = new float[size];

	for (int i = 0; i < size; i++) {

		int k = i, x = 1;

		float c = 1.0;

		if (k > n - k)

			k = n - k;

		for (int j = n; j > n - k; j--, x++)

			c *= (float)j / x;



		B[i] = c;

	}



	for (float t = 0.0; t < 1.0; t += delta) {

		float x = 0.0, y = 0.0, tmp;

		for (int i = 0; i <= n; i++) {

			tmp = B[i] * pow(t, i) * pow(1 - t, n - i);

			x += tmp * pointSet[i].x;

			y += tmp * pointSet[i].y;

		}

		drawPoint(x, y, 2.0f, color);

	}
}

void BezierCurve::drawTangent(std::vector<glm::vec2> points, float t, ImVec4 color)
{
	std::vector<glm::vec2> new_points;

	float x1, y1, x2, y2;

	glm::vec2 point;

	if (points.size() < 2)

		return;

	x2 = points[0].x + t * (points[1].x - points[0].x);

	y2 = points[0].y + t * (points[1].y - points[0].y);

	point.x = x2;
	point.y = y2;

	new_points.push_back(point);

	drawPoint(x2, y2, 2.0f, color);

	for (int i = 2; i <= points.size() - 1; i++) {

		x1 = x2;

		y1 = y2;

		x2 = points[i - 1].x + t * (points[i].x - points[i - 1].x);

		y2 = points[i - 1].y + t * (points[i].y - points[i - 1].y);

		point.x = x2;
		point.y = y2;

		new_points.push_back(point);

		drawPoint(x2, y2, 2.0f, color);



		drawLine(x1, y1, x2, y2, color);

	}

	drawTangent(new_points, t, color);

}

void BezierCurve::draw()
{
	for (int i = 0; i < pointSet.size(); i++) {
		drawPoint(pointSet[i].x, pointSet[i].y, 5.0f, point_color);
		if (i > 0)
			drawLine(pointSet[i - 1].x, pointSet[i - 1].y, pointSet[i].x, pointSet[i].y, point_color);

	}


	if (pointSet.size() > 1)
		drawBezierCurve(line_color);
	float t = (float)((int)(glfwGetTime() * 10) % 16) / 16;

	if (pointSet.size() > 1)
		drawTangent(pointSet, t, tangent_color);
}


#endif