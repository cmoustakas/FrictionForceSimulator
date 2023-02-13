#include <stdio.h>
#include <string.h>

#include <iostream>
#include <fstream>
#include <sstream>

#include <vector>
#include <string>
#include <cmath>

#include <GL/glew.h>

#include <GLFW/glfw3.h>
extern GLFWwindow* window;

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <common/text2D.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "setup_openGL.hpp"



// Print error message
void print_error(const char* err){
	fprintf(stderr,err);
	getchar();
}


int init_openGL_window(){
	if(!glfwInit()){
		print_error("Could not init OpenGL window\n");
		return FAIL;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//window = glfwCreateWindow(WIDTH,HEIGHT, "My OpenGL simulation ", glfwGetPrimaryMonitor() , NULL);
	window = glfwCreateWindow(WIDTH,HEIGHT, "Friction force simulator", NULL , NULL);


	if(window == NULL ){
		print_error("Failed to init Window\n");
		return FAIL;
	}
	glfwMakeContextCurrent(window);

	// Init GLEW to specify Targets attributes
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		print_error("Failed to init Glew\n");
		glfwTerminate();
		return FAIL;
	}

	// Just to make sure the programm can be interrupted by pressing Esc
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // Hide mouse
	glfwPollEvents();
    glfwSetCursorPos(window, WIDTH/2, HEIGHT/2);	
	glClearColor(0.1f, 0.1f, 0.1f, 0.0f);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS); 
	glEnable(GL_CULL_FACE);

	return SUCCESS;

}

int create_bind_VAO(GLuint &vertex_array_ID){	
	glGenVertexArrays(1, &vertex_array_ID);
	glBindVertexArray(vertex_array_ID);
	return SUCCESS;
}
/**
template <typename T> int create_bind_VBO(GLuint &vertex_buffer_ID, std::vector<T> &buffer){
	glGenBuffers(1,&vertex_buffer_ID);
	glBindBuffer(GL_ARRAY_BUFFER,vertex_buffer_ID);
	glBufferData(GL_ARRAY_BUFFER, buffer.size() * sizeof(T), &buffer[0], GL_STATIC_DRAW);
	return SUCCESS;
}
**/
double amplitude_of_vector(glm::vec3 v){
	return sqrt(pow(v.x,2) + pow(v.y,2) + pow(v.z,2));
}



int get_shader_code_and_compile(GLuint &shaderID, const char * path){
	// Read the Vertex Shader code from the file
	std::string shader_code;
	std::ifstream shader_stream(path, std::ios::in);
	if(shader_stream.is_open()){
		std::stringstream sstr;
		sstr << shader_stream.rdbuf();
		shader_code = sstr.str();
		shader_stream.close();
	}else{
		printf("Impossible to open %s !\n", path);
		getchar();
		return FAIL;
	}

	GLint Result = GL_FALSE;
	int info_log_len;

	// Compile Shader
	printf("Compiling shader : %s\n", path);
	char const * shader_ptr = shader_code.c_str();
	glShaderSource(shaderID, 1, &shader_ptr , NULL);
	glCompileShader(shaderID);

	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &info_log_len);
	if ( info_log_len > 0 ){
		std::vector<char> error_message(info_log_len+1);
		glGetShaderInfoLog(shaderID, info_log_len, NULL, &error_message[0]);
		printf("%s\n", &error_message[0]);
		return FAIL;
	}

	return SUCCESS;
}


int load_shaders(const char *fragment_shader_path, const char* vertex_shader_path){
	
	GLuint vertex_shader_Id = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragment_shader_Id = glCreateShader(GL_FRAGMENT_SHADER);

	if(	get_shader_code_and_compile(vertex_shader_Id, vertex_shader_path) 	 == FAIL ||
	   	get_shader_code_and_compile(fragment_shader_Id,fragment_shader_path) == FAIL  )
	{
		print_error("Couldn't compile Shaders\n");
		return FAIL;
	}

	// Linking vertex shader and fragment shader to copmlete the pipeline
	GLuint program_Id = glCreateProgram();
	glAttachShader(program_Id, vertex_shader_Id);
	glAttachShader(program_Id, fragment_shader_Id);
	glLinkProgram(program_Id);

	GLint Result;
	int info_log_len;


	// Check the program
	glGetProgramiv(program_Id, GL_LINK_STATUS, &Result);
	glGetProgramiv(program_Id, GL_INFO_LOG_LENGTH, &info_log_len);
	if ( info_log_len > 0 ){
		std::vector<char> error_message(info_log_len+1);
		glGetProgramInfoLog(program_Id, info_log_len, NULL, &error_message[0]);
		printf("%s\n", &error_message[0]);
	}

	
	glDetachShader(program_Id, vertex_shader_Id);
	glDetachShader(program_Id, fragment_shader_Id);
	
	glDeleteShader(vertex_shader_Id);
	glDeleteShader(fragment_shader_Id);

	return program_Id;
	
}


text_render::text_render(unsigned int shader_ID, std::string working_dir){

	initText2D((working_dir+"/textures_n_objects/Holstein.DDS").c_str()
			   ,shader_ID);
}

void text_render::update_metrics_to_screen(std::vector<glm::vec3> metrics){
	/** metrics:
			
			1:B_field:
			2:F_Lorentz:
			3:Velocity:
	**/
	char text[256];
	int index = 1;
	for(std::vector<glm::vec3>::iterator it  = metrics.begin(); 
											   it != metrics.end(); 
											   it++)
	{
		char output[256];
		char line[256];
		char *temp;
		switch(index){
			case 1:
				temp = "(Green) B = ";
				break;
			case 2:
				temp = "(Red)   E = ";
				break;
			case 3:
				temp = "(White) F = ";
				break;
			case 4:
				temp = "(Blue)  u = ";
				break;
		}
		
		strcpy(line,temp);		
		sprintf(output,"[%.3f %.3f %.3f] ", it->x, it->y, it->z );
		strcat(line, output);
		//strcat(text,line);

		printText2D(line,1.30872, 13.119+index*20, 20);

		index++;
	}
	
}

Model_Mesh::Model_Mesh(){}

Model_Mesh::Model_Mesh(const char* model_path, std::string textures_path){
	scene = importer.ReadFile(model_path,aiProcess_JoinIdenticalVertices | aiProcess_Triangulate | aiProcess_FlipUVs);
	load_model(textures_path.c_str());
}

void Model_Mesh::set_Model(const char* model_path, std::string textures_path){
	scene = importer.ReadFile(model_path,aiProcess_JoinIdenticalVertices | aiProcess_Triangulate | aiProcess_FlipUVs);
	load_model(textures_path.c_str());
}

int Model_Mesh::is_image_loaded(std::string file_name){		
		for (unsigned int i = 0; i < texture_list.size(); ++i)	
			if (file_name.compare(texture_list[i].image_name) == 0)
				return texture_list[i].textureID;
		return -1;
}

unsigned int Model_Mesh::load_texture_image(std::string file_name, bool& load_complete)
{
	// stbi_set_flip_vertically_on_load(1); // Call this function if the image is upside-down.		

	std::size_t position = file_name.find_last_of("\\");
	file_name = file_name.substr(position + 1);		
	

	int width, height, num_components;
	unsigned char* image_data = stbi_load(file_name.c_str(), &width, &height, &num_components, 0);

	unsigned int textureID;
	glGenTextures(1, &textureID);		

	if (image_data)
	{
		GLenum format{};

		if (num_components == 1)
			format = GL_RED;
		else if (num_components == 3)
			format = GL_RGB;
		else if (num_components == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1); 

		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, image_data);
		glGenerateMipmap(GL_TEXTURE_2D);
 
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT); // GL_REPEAT... GL_MIRRORED_REPEAT... GL_CLAMP_TO_EDGE... GL_CLAMP_TO_BORDER.
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
 
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // GL_NEAREST or GL_LINEAR.
 
		load_complete = true;
		stbi_image_free(image_data);			
		std::cout << "   Image loaded OK: " << file_name << "\n";
	}
	else
	{
		load_complete = false;
		stbi_image_free(image_data);
		std::cout << "   Image failed to load: " << file_name << "\n";
	}
	return textureID;
}

void Model_Mesh::set_buffer_data(unsigned int index)
{
	
	if(create_bind_VAO(mesh_list[index].VAO) == FAIL) std::cout << "Error in VAO\n";

	if(create_bind_VBO(mesh_list[index].VBO1,mesh_list[index].vert_positions) == FAIL) std::cout << "Error in VBO1\n";
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0,3,GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);

	if(create_bind_VBO(mesh_list[index].VBO2,mesh_list[index].vert_normals)   == FAIL) std::cout << "Error in VBO2\n";
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1,3,GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);

	if(create_bind_VBO(mesh_list[index].VBO3,mesh_list[index].tex_coords)	  == FAIL) std::cout << "Error in VBO3\n";
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2,2,GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*)0);

	glGenBuffers(1, &mesh_list[index].EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh_list[index].EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * mesh_list[index].vert_indices.size(), &mesh_list[index].vert_indices[0], GL_STATIC_DRAW);
	
	glBindVertexArray(0); 

}

void Model_Mesh::load_model(const char* textures_path)
{
	if (!scene || !scene->mRootNode || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)
		std::cout << "Assimp impocreate_create_bind_VBO(mesh_list.VBO1);bind_VBO(mesh_list.VBO1);rter.ReadFile (Error) -- " << importer.GetErrorString() << "\n";
	else
	{
		num_meshes = scene->mNumMeshes;
		mesh_list.resize(num_meshes);

		aiMesh* mesh{};		
		int indices_offset = 0; 

		for (unsigned int i = 0; i < num_meshes; ++i)
		{
			mesh = scene->mMeshes[i];
			mesh_list[i].mName = mesh->mName.C_Str();
			

			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex]; 
			for (unsigned int tex_count = 0; tex_count < material->GetTextureCount(aiTextureType_DIFFUSE); ++tex_count) // Also, only using: aiTextureType_DIFFUSE.
			{
				aiString string_postfix;	
				material->GetTexture(aiTextureType_DIFFUSE, tex_count, &string_postfix); 
				

				aiString string(textures_path);
				string.Append(string_postfix.C_Str());
				
				int already_loaded = is_image_loaded(string.C_Str()); 
				

				if (already_loaded == -1) 
				{
					bool load_success = false;
					unsigned int texture_handle = load_texture_image(string.C_Str(), load_success);
	 				if (load_success) 
					{
						Texture texture;
						texture.image_name = string.C_Str();
						texture.textureID = texture_handle;

						texture_list.push_back(texture);
						mesh_list[i].tex_handle = texture_handle;
					}
				}
				else	
					mesh_list[i].tex_handle = already_loaded;
				
			}

			for (unsigned int i2 = 0; i2 < mesh->mNumVertices; ++i2)
			{
					glm::vec3 position{};
					position.x = mesh->mVertices[i2].x;
					position.y = mesh->mVertices[i2].y;
					position.z = mesh->mVertices[i2].z;
					mesh_list[i].vert_positions.push_back(position);					
 
					if (mesh->HasNormals())
					{
						glm::vec3 normal{};
						normal.x = mesh->mNormals[i2].x;
						normal.y = mesh->mNormals[i2].y;
						normal.z = mesh->mNormals[i2].z;
						mesh_list[i].vert_normals.push_back(normal);
					}
					else
						mesh_list[i].vert_normals.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
 					if (mesh->HasTextureCoords(0)) 
					{
						glm::vec2 tex_coords{};
						tex_coords.x = mesh->mTextureCoords[0][i2].x;
						tex_coords.y = mesh->mTextureCoords[0][i2].y;							
						mesh_list[i].tex_coords.push_back(tex_coords);							
					}
					else
						mesh_list[i].tex_coords.push_back(glm::vec2(0.0f, 0.0f));
			}
				

			for (unsigned int i3 = 0; i3 < mesh->mNumFaces; ++i3)
				for (unsigned int i4 = 0; i4 < mesh->mFaces[i3].mNumIndices; ++i4)										
					mesh_list[i].vert_indices.push_back(mesh->mFaces[i3].mIndices[i4] + indices_offset);									
			
			
			set_buffer_data(i); 
		}
		delete mesh;			
	}
}	

