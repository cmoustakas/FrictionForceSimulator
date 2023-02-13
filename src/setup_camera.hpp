#ifndef CAMERA_SETUP_H
#define CAMERA_SETUP_H

#include <GLFW/glfw3.h>
extern GLFWwindow* window;
// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <unistd.h>

#define PI 3.14
#define QUANT_ANGLE 0.01

const glm::vec3 static_position = glm::vec3(-80.00f,30.0f,-360.00f);


class camera{
	
	glm::vec3 position;
	glm::vec3 up;  
	glm::vec3 direction; 
	glm::mat4 cameraModel;
	float m_angleXZ = 0.0f;
	float m_angleY	= 0.0f;
	unsigned int m_ray = 5.0f;
	float omega = 0.008f;
	unsigned int deltaTime = 0;

	public:
		camera();

		void observe_on_the_turn(glm::vec3 car_pos);
		void follow_car(glm::vec3 car_pos);
		void rotate();
		void reset();

		glm::vec3 get_position();
		glm::vec3 get_up_vector();
		glm::vec3 get_direction();
		glm::mat4 get_camera_model();
};

camera::camera(){
	
	position = glm::vec3(20.00f, 10.0f, 0.0f);
	direction = glm::vec3(0.0f, 0.0f, 0.0f);
	up = glm::vec3(0.0f, 1.0f, 0.0f);
	//direction = -glm::vec3(0.0f,1.0f,0.0f);
	m_ray = glm::length(position);
	cameraModel = glm::lookAt(position, -direction, up);
}

glm::vec3 camera::get_position(){    return position;}
glm::vec3 camera::get_up_vector(){   return up;}
glm::vec3 camera::get_direction(){   return direction;}
glm::mat4 camera::get_camera_model(){return cameraModel;} 

void camera::reset(){
	position = glm::vec3(20.00f, 10.0f, 0.0f);
	direction = glm::vec3(0.0f, 0.0f, 0.0f);
	up = glm::vec3(0.0f, 1.0f, 0.0f);
	//direction = -glm::vec3(0.0f,1.0f,0.0f);
	m_ray = glm::length(position);
	cameraModel = glm::lookAt(position, -direction, up);
}

void camera::rotate(){
	position = glm::vec3( m_ray * cos(omega*deltaTime), position.y , m_ray * sin(omega*deltaTime));
	cameraModel = glm::lookAt(position, -direction, up);
	deltaTime++;
}

void camera::observe_on_the_turn(glm::vec3 car_pos){
	direction = -car_pos;
	position = static_position;
	cameraModel = glm::lookAt(position, -direction, up);
}


void camera::follow_car(glm::vec3 car_pos){



	direction = -car_pos;
	
	if (glfwGetKey( window, GLFW_KEY_RIGHT ) == GLFW_PRESS)
		m_angleXZ += QUANT_ANGLE;
	else if (glfwGetKey( window, GLFW_KEY_LEFT ) == GLFW_PRESS)
		m_angleXZ -= QUANT_ANGLE;
	if (glfwGetKey( window, GLFW_KEY_UP ) == GLFW_PRESS)
		m_angleY += QUANT_ANGLE;	
	if (glfwGetKey( window, GLFW_KEY_DOWN ) == GLFW_PRESS)
		(m_angleY > 0) ?  m_angleY -= QUANT_ANGLE : m_angleY = 0.0f; 
	
	position = glm::vec3(m_ray*sin(m_angleXZ) + car_pos.x,
					     m_ray*sin(m_angleY) + 10.0f,
						 m_ray*cos(m_angleXZ) + car_pos.z);

	cameraModel = glm::lookAt(position, -direction, up);



}

#endif
