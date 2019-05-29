

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include "BezierCurve.h"


using namespace std;

const unsigned int SCR_WIDTH = 1000;

const unsigned int SCR_HEIGHT = 1000;



void framebuffer_size_callback(GLFWwindow* window, int width, int height);

void processInput(GLFWwindow* window);



void mouse_move_callback(GLFWwindow* window, double xpos, double ypos);

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);



// 存储点坐标的类



float xCur, yCur;	// 记录鼠标实时位置


int main()

{

	// glfw初始化和配置

	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);



	// 创建窗口

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "DrawBezier", NULL, NULL);

	if (window == NULL)

	{

		cout << "Failed to create GLFW window" << endl;

		glfwTerminate();

		return 0;

	}

	glfwMakeContextCurrent(window);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	glfwSetCursorPosCallback(window, mouse_move_callback);		//鼠标移动回调函数

	glfwSetMouseButtonCallback(window, mouse_button_callback);	//鼠标点击回调函数




	//gl3wInit();

	ImVec4 line_color = ImVec4(0.8f, 0.0f, 0.0f, 1.00f);

	ImVec4 point_color = ImVec4(0.0f, 0.8f, 0.0f, 1.00f);

	ImVec4 tangent_color = ImVec4(0.0f, 0.0f, 0.8f, 1.00f);

	BezierCurve* bezierCurve = new BezierCurve(line_color, point_color, tangent_color);

	while (!glfwWindowShouldClose(window))

	{

		processInput(window);



		glClear(GL_COLOR_BUFFER_BIT);


		bezierCurve->draw();



		glfwSwapBuffers(window);

		glfwPollEvents();

	}

	glfwTerminate();

	return 0;

}



// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly

// ---------------------------------------------------------------------------------------------------------

void processInput(GLFWwindow * window)

{

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)

		glfwSetWindowShouldClose(window, true);

}




void mouse_move_callback(GLFWwindow * window, double xpos, double ypos) {


	xCur = (xpos - SCR_WIDTH / 2) / SCR_WIDTH * 2;

	yCur = 0 - (ypos - SCR_HEIGHT / 2) / SCR_HEIGHT * 2;

};





void mouse_button_callback(GLFWwindow * window, int button, int action, int mods)

{

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {

		glm::vec2 point;

		point.x = xCur;
		point.y = yCur;

		pointSet.push_back(point);

	}


	if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_RIGHT) {

		if (!pointSet.empty())

			pointSet.pop_back();

	}

}



// glfw: whenever the window size changed (by OS or user resize) this callback function executes

// ---------------------------------------------------------------------------------------------

void framebuffer_size_callback(GLFWwindow * window, int width, int height)

{

	glViewport(0, 0, width, height);

}


