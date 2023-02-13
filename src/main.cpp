// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <filesystem>
#include <cstring>
#include <chrono>
#include <thread>


// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Include imgui
#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_glfw.h"


//Include Assimp
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "setup_openGL.hpp"
#include "setup_camera.hpp"
#include "render.hpp"

#include <common/texture.hpp>
#include <common/objloader.hpp>
#include <common/text2D.hpp>


int main(int argc, char *argv[]){

    //Init my window
    init_openGL_window();
	
	std::filesystem::path cwd 	   = std::filesystem::current_path();
	std::filesystem::path cwd_shdr = std::filesystem::current_path();
	
	cwd_shdr.replace_filename("src");
	cwd.replace_filename("textures_n_objects/");
	
    std::string fragment_shader_path = "/shaders/fragment_shader_gear.glsl";
	std::string vertex_shader_path = "/shaders/vertex_shader_gear.glsl";
	
	std::string map_path = "semi_map.obj";
	std::string car_path = "porsche.obj";
	std::string arrow_path = "arrow.obj";

	// Load Model into main programm
    Model_Mesh *model_map   = new Model_Mesh((cwd.string() + map_path).c_str(), cwd.string());
    Model_Mesh *model_car   = new Model_Mesh((cwd.string() + car_path).c_str(), cwd.string());
	Model_Mesh *model_arrow = new Model_Mesh((cwd.string() + arrow_path).c_str(), cwd.string());
    
	/**
	std::thread model_loader_t[3];
	model_loader_t[0] = std::thread(model_load,model_map,"/home/robin/Desktop/C++DEMOS/GearAssimpDemo/textures_n_objects/semi_map.obj");
	model_loader_t[1] = std::thread(model_load,model_car,"/home/robin/Desktop/C++DEMOS/GearAssimpDemo/textures_n_objects/porsche.obj");
	model_loader_t[2] = std::thread(model_load,model_arrow,"/home/robin/Desktop/C++DEMOS/GearAssimpDemo/textures_n_objects/arrow.obj");
	**/
	
	/**
	*	Make sure all of my threads 
	
	for(unsigned int t=0;t<3;t++)
		model_loader_t[t].join();
	
	std::cout << "All models returned" << std::endl;
	**/

	// Load shaders
    GLuint program_Id = load_shaders((cwd_shdr.string() + fragment_shader_path).c_str(),
                                     (cwd_shdr.string() + vertex_shader_path).c_str());

    glActiveTexture(GL_TEXTURE0); // Reusing the same texture unit for each model mesh.
	unsigned int image_sampler_loc = glGetUniformLocation(program_Id, "image");
	glUniform1i(image_sampler_loc, 0);

	//Bind my uniforms 
	//Link my matrix_Id with the uniform model, model view projection
	GLuint matrix_Id    = glGetUniformLocation(program_Id,"model_mat");
	GLuint view_Id 		= glGetUniformLocation(program_Id,"view_mat");
	GLuint projec_Id    = glGetUniformLocation(program_Id,"projection_mat");
	GLuint rotation_Id  = glGetUniformLocation(program_Id,"rotation_mat");
	

	// Create a camera object 
    camera *camera_obj     = new camera();
	vehicle *car_obj       = new vehicle();

	// Init GUI
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(window,true);
	ImGui_ImplOpenGL3_Init("#version 330");

    bool show_GUI_configure = true;
	bool show_GUI_metrics 	= false;
	bool slowMo				= false;
	bool changeView			= false;

	float acc_slidebar = 0.0f;

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    do 
	{
		//glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
		glUseProgram(program_Id);	
			
		if(show_GUI_configure)
			camera_obj->rotate();
		else if(show_GUI_metrics){
			if(!changeView) camera_obj->follow_car(car_obj->m_car_position);
			else			camera_obj->observe_on_the_turn(car_obj->m_car_position);

			if(slowMo) std::this_thread::sleep_for(std::chrono::milliseconds(50));
		}


		glm::mat4 ModelMatrix   	 = glm::mat4(1.0);
        glm::mat4 ProjectionMatrix 	 = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 1000.0f);
		glm::mat4 ViewMatrix 		 = camera_obj->get_camera_model();
		glm::mat4 rotationMatrix	 = glm::mat4(1.0f);

		
		glUniformMatrix4fv(matrix_Id   , 1, GL_FALSE, &ModelMatrix[0][0]);
		glUniformMatrix4fv(projec_Id   , 1, GL_FALSE, &ProjectionMatrix[0][0]);
		glUniformMatrix4fv(view_Id	   , 1, GL_FALSE, &ViewMatrix[0][0]);
		glUniformMatrix4fv(rotation_Id , 1, GL_FALSE, &rotationMatrix[0][0]);

		//renderMap(model_system_axis); // helper function
		renderMap(model_map);
		renderCar(model_car,car_obj, ViewMatrix, view_Id);
		renderArrow(model_arrow,car_obj,ViewMatrix,view_Id);
		renderGUI(car_obj, camera_obj,show_GUI_configure, show_GUI_metrics, slowMo, changeView, acc_slidebar);


		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );


	glDeleteProgram(program_Id); // This OpenGL function call is talked about in: shader_configure.h
	ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();


	glfwTerminate(); 
	exit(EXIT_SUCCESS); 

}
