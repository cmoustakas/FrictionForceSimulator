#ifndef RENDER_H
#define RENDER_H


#include <GLFW/glfw3.h>
extern GLFWwindow* window;
// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <vector>

#include <unistd.h>
#include <math.h>

#define VERTICALITY 0
#define PARALLEL 0.999
#define SMOOTH_FACTOR 1/45
#define g 10
#define friction_factor 0.016
#define DRIFT 0.01

std::vector<glm::vec3> wheelsWCS = {glm::vec3(2.3629 , 0.77690, 2.5121),
								    glm::vec3(-2.1401, 0.89424, -3.7282),
									glm::vec3(-2.2541, 0.85300, 2.5590),
									glm::vec3(2.27200, 0.85779, -3.74100)
									};

//glm::vec2 centerOfCircle = glm::vec2(-40.744f, -248.78f);
glm::vec2 centerOfCircle = glm::vec2(-40.00f, -250.00f);


class vehicle{
	public:

	glm::vec3 m_car_position     = glm::vec3(0.0f, 0.5f, 0.0f);
	glm::vec3 m_car_velocity     = glm::vec3(0.0f, 0.0f, 0.000001f);
	glm::vec2 m_wheelRotationDegrees = glm::vec2(0.0f, 0.0f);
	
	float m_car_acceleration  = 0.0f;
	float        m_carDegrees = 0.0f;
	vehicle(){};
	void reset();
};

void vehicle::reset(){
	m_car_position     = glm::vec3(0.0f, 0.5f, 0.0f);
	m_car_velocity     = glm::vec3(0.0f, 0.0f, 0.000001f);
	m_wheelRotationDegrees = glm::vec2(0.0f, 0.0f);
	m_car_acceleration  = 0.0f;
	m_carDegrees = 0.0f;
}






enum CarWheelMeshNames{
						BackRight,
						BackRightCarbon,
						FrontLeft,
						FrontLeftCarbon,
						BackLeft,
						BackLeftCarbon,
						FrontRight,
						FrontRightCarbon
					};

const float wheel_ray = 0.5;
const float RayOfCircle = 200;

float theta = 0.0f;

bool exitCicle = false;
bool shift = false;

int test = 0;

void print_vec(glm::vec3 v){
	std::cout << " " << v.x << " " << v.y << " " << v.z << std::endl;
}

float vector_multiplication(glm::vec2 v1, glm::vec2 v2 ){
	return v1.x*v2.x + v1.y*v2.y;
}




bool analyzeFrictionForces(glm::vec3 car_velocity){

	float meter_velocity = glm::length(car_velocity);
	float bound          = sqrt(RayOfCircle * g * friction_factor);
	if(meter_velocity > bound){
		exitCicle = true;
		return true;
	}
	return false;
}

void insideCircle(vehicle *car, float parallelism, glm::vec2 centerVector){
	float meter_velocity = glm::length(car->m_car_velocity);

	if(analyzeFrictionForces(car->m_car_velocity))
		shift = true;
	else{
		theta = acos(parallelism);	
		car->m_car_velocity  = meter_velocity * glm::vec3(cos(theta), 0.0f, sin(theta));
		car->m_car_velocity += car->m_car_acceleration * car->m_car_velocity;
	}	
	car->m_carDegrees   +=  glm::length(glm::cross(car->m_car_velocity, glm::vec3(centerVector.x,0.0f,centerVector.y))) * SMOOTH_FACTOR;
}


void stopCar(vehicle *car){
	glm::length(car->m_car_velocity) > 0.001 ? (car->m_car_velocity -= (0.04f) * car->m_car_velocity) : (car->m_car_velocity  = glm::vec3(0.0f,0.0f,0.0f));
	float maxRotation = 2.5f;
	exitCicle = true;

	if(shift)
		maxRotation = 1.5f;
	
	car->m_carDegrees > maxRotation ? 0 :(car->m_carDegrees+= DRIFT); // Nostalgia of NFS :)
	
}

void analyze_motion(vehicle *car, glm::mat4 ViewMatrix){
	
	glm::vec2 centerVector = glm::normalize(glm::vec2(car->m_car_position.x-centerOfCircle.x, car->m_car_position.z - centerOfCircle.y));
	glm::vec2 motionVector = glm::normalize(glm::vec2(car->m_car_position.x-car->m_car_velocity.x  , car->m_car_position.z - car->m_car_velocity.z));
	
	float parallelism = vector_multiplication(centerVector,motionVector);


	/**
	Update car velocity vector on each ocasion
	**/
	if(parallelism < VERTICALITY ) // Before Circle
		car->m_car_velocity += car->m_car_acceleration*glm::length(car->m_car_velocity)*glm::vec3(0.0f,0.0f,1.0f);
	else if(parallelism > PARALLEL) //After Circle
		stopCar(car);
	else if(!exitCicle)
		insideCircle(car, parallelism, centerVector);
	else{
		stopCar(car);
		theta = 0.0f;
	}

	car->m_car_position -= car->m_car_velocity;

 
}


void rotate_wheels(glm::mat4 &viewWheelMatrix, Model_Mesh *model_car, glm::vec3 wheels_posWCS, glm::vec2 degrees, GLuint view_Id, unsigned int idx){

	glm::mat4 translateWheelWCSback  = glm::translate(glm::mat4(1.0f),-wheels_posWCS);
	glm::mat4 translateWheelWCSforth = glm::translate(glm::mat4(1.0f), glm::vec3(wheels_posWCS.x, wheels_posWCS.y, wheels_posWCS.z));
	glm::mat4 rotationWheelMatrix = glm::rotate(glm::mat4(1.0f), degrees.x , glm::vec3(1.0f,0.0f ,0.0f));
	glm::mat4 rotationWheelAtCircle = glm::mat4(1.0f);
	if(degrees.y != 0)
		rotationWheelAtCircle = glm::rotate(glm::mat4(1.0f), degrees.y , glm::vec3(0.0f,1.0f ,0.0f));

	viewWheelMatrix = viewWheelMatrix * translateWheelWCSforth * rotationWheelAtCircle * rotationWheelMatrix *  translateWheelWCSback  ;

				

	glUniformMatrix4fv(view_Id,1,GL_FALSE,&viewWheelMatrix[0][0]);
	glBindTexture(GL_TEXTURE_2D, model_car->mesh_list[idx].tex_handle); // Bind texture for the current mesh.	
	glBindVertexArray(model_car->mesh_list[idx].VAO);
	glDrawElements(GL_TRIANGLES, (GLsizei)model_car->mesh_list[idx].vert_indices.size(), GL_UNSIGNED_INT, 0);

}


void renderCar(Model_Mesh *model_car, vehicle *car, glm::mat4 &ViewMatrix, GLuint view_Id){
	

    analyze_motion(car, ViewMatrix);

	car->m_wheelRotationDegrees -= glm::vec2(glm::length(car->m_car_velocity) * wheel_ray, 0.0f);
	theta != 0.0f ? car->m_wheelRotationDegrees.y = 0.5f : car->m_wheelRotationDegrees.y = 0.0f;

	glm::mat4 viewWheelMatrix;
	
	

	ViewMatrix = glm::translate(ViewMatrix,car->m_car_position);
	ViewMatrix = glm::rotate(ViewMatrix,car->m_carDegrees,glm::vec3(0.0f,1.0f,0.0f));
	

	for(unsigned int i=0; i<model_car->num_meshes; ++i){
		viewWheelMatrix = ViewMatrix;

		switch(i){
			
			case BackRight:
			case BackRightCarbon:
				rotate_wheels(viewWheelMatrix, model_car, wheelsWCS.at(0), glm::vec2(car->m_wheelRotationDegrees.x,0.0f), view_Id, i);
				break;

			case FrontLeft:
			case FrontLeftCarbon:
				rotate_wheels(viewWheelMatrix, model_car, wheelsWCS.at(1), car->m_wheelRotationDegrees, view_Id, i);
				break;

			case BackLeft:
			case BackLeftCarbon:
				rotate_wheels(viewWheelMatrix, model_car, wheelsWCS.at(2), glm::vec2(car->m_wheelRotationDegrees.x,0.0f), view_Id, i);
				break;
				

			case FrontRight:
			case FrontRightCarbon:
				rotate_wheels(viewWheelMatrix, model_car, wheelsWCS.at(3), car->m_wheelRotationDegrees, view_Id, i);
				break;
		}	
	}
	
	
	glUniformMatrix4fv(view_Id, 1, GL_FALSE, &ViewMatrix[0][0]);

	for (unsigned int i = 8; i < model_car->num_meshes; ++i)
	{		
		glBindTexture(GL_TEXTURE_2D, model_car->mesh_list[i].tex_handle); // Bind texture for the current mesh.	
		glBindVertexArray(model_car->mesh_list[i].VAO);
		glDrawElements(GL_TRIANGLES, (GLsizei)model_car->mesh_list[i].vert_indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

}

void renderArrow(Model_Mesh *model_arrow, vehicle *car,glm::mat4 &ViewMatrix, GLuint view_Id){
	glm::mat4 view_arrow = glm::scale(ViewMatrix,glm::vec3(car->m_car_velocity.x/3,1.0f,1.0f));
	 
	glUniformMatrix4fv(view_Id, 1, GL_FALSE, &view_arrow[0][0]);

	for (unsigned int i = 0; i < model_arrow->num_meshes; ++i)
	{		
		glBindTexture(GL_TEXTURE_2D, model_arrow->mesh_list[i].tex_handle); // Bind texture for the current mesh.	
		glBindVertexArray(model_arrow->mesh_list[i].VAO);
		glDrawElements(GL_TRIANGLES, (GLsizei)model_arrow->mesh_list[i].vert_indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}
}

void renderMap(Model_Mesh *model_map){

	for (unsigned int i = 0; i < model_map->num_meshes; ++i)
	{			
		glBindTexture(GL_TEXTURE_2D, model_map->mesh_list[i].tex_handle); // Bind texture for the current mesh.	
		glBindVertexArray(model_map->mesh_list[i].VAO);
		glDrawElements(GL_TRIANGLES, (GLsizei)model_map->mesh_list[i].vert_indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}
		
}


void renderGUI(vehicle *car, camera *cam, bool &show_GUI_configure, bool &show_GUI_metrics, bool &slowMo, bool &changeView, float &acc){
		const ImVec2 size = ImVec2(350.0f,120.0f);

		ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Show my GUI
        if (show_GUI_configure)
        {
			ImGui::SetNextWindowSize(size, show_GUI_metrics);

            ImGui::Begin("Simulator Control Panel Configuration", &show_GUI_configure);   
			ImGui::SliderFloat("Acceleration", &acc , 0.0f, 0.025f);
			
			if (ImGui::Button("Configure [+]")){
				show_GUI_configure = false;
				show_GUI_metrics   = true;
				car->m_car_velocity = glm::vec3(0.0f,0.0f,0.1f);
				car->m_car_acceleration = acc;
			}
            ImGui::End();
        }
		else if(show_GUI_metrics){

			ImGui::SetNextWindowSize(size, show_GUI_metrics);
			
			ImGui::Begin("Speedometer", &show_GUI_configure); 
			ImGui::Text("%d: Velocity [km/h]", (int)(30*glm::length(car->m_car_velocity)) );  
			ImGui::Text("%d: Ray [m]", (int)RayOfCircle );  
			ImGui::Text("%.1f: m_f ", 100*friction_factor/2 );

			if(ImGui::Button("Slow Motion")){
				if(slowMo) slowMo = false ;
				else slowMo = true;
			}

			ImGui::SameLine();

			if(ImGui::Button("Change View")){
				if(changeView) changeView = false;
				else changeView = true;
			}

			ImGui::SameLine();

			if (ImGui::Button("Restart [+]")){
				slowMo = false;
				show_GUI_configure = true;
				show_GUI_metrics   = false;
				exitCicle = false;
				shift     = false;
				acc = 0.0f;
				cam->reset();
				car->reset();
			}
            ImGui::End();			

		}

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());		

}

/*
void model_load(Model_Mesh *m, std::string path){
	m->set_Model(path.c_str());
}
*/
#endif