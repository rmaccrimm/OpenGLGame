#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>
#include <vector>
#include <sstream>
#include <map>
#include <utility>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shader.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "camera.h"
#include "Model.h"

bool setupWindow(GLFWwindow *window);
void framebufferSizeCallback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void configureShader(Shader &shader, bool shadow);
void setupScreenBuffer(GLuint &fbo, GLuint &texture, GLuint &rbo);
Mesh getScreenQuad();

glm::vec3 lightDirection(1.0, -0.5, 0.5);
Camera camera(glm::vec3(-10, -10, -10) * lightDirection);

float SCR_WIDTH = 1920;
float SCR_HEIGHT = 1080;
float SHADOW_WIDTH = 1024;
float SHADOW_HEIGHT = 1024;

float DELTA_TIME = 0;
float LAST_FRAME = 0;
double LAST_X = 400;
double LAST_Y = 300;
bool FIRST_MOUSE = true;

const std::string rootdir = "C:/Users/Roderick/Documents/Projects/OpenGLGame/";
const std::string modelPath = rootdir + "boxguy/export/boxguy.obj";
const std::string shaderdir = rootdir + "src/shaders/";
const std::string basicVertex = shaderdir + "basic_vert.glsl";
const std::string basicFragment = shaderdir + "basic_frag.glsl";
const std::string depthVertex = shaderdir + "depth_vert.glsl";
const std::string emptyFragment = shaderdir + "empty_frag.glsl";
const std::string screenVertex = shaderdir + "screen_vert.glsl";
const std::string screenFragment = shaderdir + "screen_frag.glsl";

int main()
{
	glfwInit();		
	GLFWwindow *window = glfwCreateWindow((int)SCR_WIDTH, (int)SCR_HEIGHT, "OpenGL Game", NULL, NULL);
	if (!setupWindow(window)) {	return -1; }
	
	Model model(modelPath);
	Shader basicShader(basicVertex, basicFragment);
	Shader depthShader(depthVertex, emptyFragment);
	Shader screenShader(screenVertex, screenFragment);
	Mesh screenQuad = getScreenQuad();

	GLuint fbo, texture, depthMap;
	setupScreenBuffer(fbo, texture, depthMap);

	while (!glfwWindowShouldClose(window)) {
		processInput(window);
		float current_frame = glfwGetTime();
		DELTA_TIME = current_frame - LAST_FRAME;
		LAST_FRAME = current_frame;
		glEnable(GL_DEPTH_TEST);
		glClearColor(0.1, 0.1, 0.1, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		configureShader(depthShader, true);

		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glClear(GL_DEPTH_BUFFER_BIT);
		model.draw();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		screenShader.use();
		screenShader.set_int("screenTexture", 0);
		glDisable(GL_DEPTH_TEST);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		screenQuad.draw();

		/*glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		glClearColor(0.3f, 0.3f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		configureShader(basicShader, false);
		model.draw();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT);
		screenShader.use();
		glDisable(GL_DEPTH_TEST);
		glBindTexture(GL_TEXTURE_2D, texture);
		screenQuad.draw();*/

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
	return 0;
}

Mesh getScreenQuad()
{
	float squad[] = {  
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
	     1.0f,  1.0f,  1.0f, 1.0f
	};
	std::vector<Vertex> vertices;
	for (int i = 0; i < 4; i++) {
		vertices.push_back(Vertex(glm::vec3(squad[4*i], squad[4*i + 1], 0.0), 
		 						  glm::vec2(squad[4*i + 2], squad[4*i + 3])));
	}
	std::vector<unsigned int> indices = {0, 1, 2, 2, 3, 0};
	return Mesh(vertices, indices);
}

void setupScreenBuffer(GLuint &fbo, GLuint &texture, GLuint &depthMap)
{
	glGenFramebuffers(1, &fbo);  
	
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);  

	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, 
				 GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);  

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	//glDrawBuffer(GL_NONE);
	//glReadBuffer(GL_NONE);

	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0); 
}

void configureShader(Shader &shader, bool shadows)
{
	shader.use();
	glm::mat4 model(1.0);
	if (shadows) {
		float near_plane = 1.0f, far_plane = 20.0f;
		glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, 
											   far_plane); 
		glm::mat4 lightView = glm::lookAt(glm::vec3(-10, -10, -10) * lightDirection, 
										  glm::vec3( 0.0f, 0.0f,  0.0f), 
										  glm::vec3( 0.0f, 1.0f,  0.0f)); 
		glm::mat4x4 lightSpaceMatrix = lightProjection * lightView;
		shader.set_mat4("lightSpaceMatrix", lightSpaceMatrix);
		shader.set_mat4("model", model);
	}
	else {
		glm::mat4 view = camera.get_view();
		glm::mat4 projection = glm::perspective(glm::radians(camera.fov), SCR_WIDTH / SCR_HEIGHT, 0.1f, 100.0f);
		
		shader.set_mat4("model", model);
		shader.set_mat4("projection", projection);
		shader.set_mat4("view", view);
		shader.set_vec3("viewPos", camera.position);
		shader.set_vec3("material.diffuse", 0.5f, 0.8f, 0.1f);
		shader.set_vec3("material.specular", 0.2, 0.2, 0.2);
		shader.set_float("material.shininess", 32.0f);
		shader.set_vec3("dirLight.ambient", 0.2f, 0.2f, 0.2f);
		shader.set_vec3("dirLight.diffuse", 0.8f, 0.8f, 0.8f);
		shader.set_vec3("dirLight.specular",  1.0f, 1.0f, 1.0f);
		shader.set_vec3("dirLight.direction", lightDirection);
	}
}

bool setupWindow(GLFWwindow* window)
{
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return false;
	}
	glfwMakeContextCurrent(window);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return false;
	}
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);
	return true;
}

void framebufferSizeCallback(GLFWwindow *window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		camera.process_keyboard_input(FORWARD, DELTA_TIME);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		camera.process_keyboard_input(BACKWARD, DELTA_TIME);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		camera.process_keyboard_input(LEFT, DELTA_TIME);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		camera.process_keyboard_input(RIGHT, DELTA_TIME);
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE)) {
		camera.process_keyboard_input(UP, DELTA_TIME);
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT)) {
		camera.process_keyboard_input(DOWN, DELTA_TIME);
	}
}

void mouse_callback(GLFWwindow* window, double x_pos, double y_pos)
{
	if (FIRST_MOUSE) {
		LAST_X = x_pos;
		LAST_Y = y_pos;
		FIRST_MOUSE = false;
	}

	float x_offset = x_pos - LAST_X;
	float y_offset = LAST_Y - y_pos; // positive y is toward bottom of window
	LAST_X = x_pos;
	LAST_Y = y_pos;
	camera.process_mouse_movement(x_offset, y_offset);
}