
#include <string.h>
#include <common/text2D.hpp>

#define SUCCESS 1
#define FAIL    0
#define WIDTH   640
#define HEIGHT  480
#define LAYOUT0 0
#define LAYOUT1 1

// Print error message
void print_error(const char* err);

// Initialize an opengl window 
int init_openGL_window();

// Binds VAO
int create_bind_VAO(GLuint &vertex_array_ID);


//Binds VBO
template <typename T> int create_bind_VBO(GLuint &vertex_buffer_ID, std::vector<T> &buffer){
	glGenBuffers(1,&vertex_buffer_ID);
	glBindBuffer(GL_ARRAY_BUFFER,vertex_buffer_ID);
	glBufferData(GL_ARRAY_BUFFER, buffer.size() * sizeof(T), &buffer[0], GL_STATIC_DRAW);
	return SUCCESS;
}

int get_shader_code_and_compile(GLuint &shaderID, const char *path);



//Loads a fragment shader and a vertex shader
int load_shaders(const char* fragment_shader_path,
                 const char* vertex_shader_path);

double amplitude_of_vector(glm::vec3 v);

#ifndef TEXT_RENDER_H
#define TEXT_RENDER_H

class text_render{

	public:
		text_render(unsigned int shader_ID, std::string working_dir);
		~text_render(){cleanupText2D();}
		void update_metrics_to_screen(std::vector<glm::vec3> metrics);
};

#endif


#ifndef MODEL_MESH_H
#define MODEL_MESH_H

class Model_Mesh{
	
	Assimp::Importer importer;
	const aiScene *scene = NULL;
	aiNode *root_node = NULL;

	struct Mesh{
		GLuint VAO;
		GLuint VBO1;
		GLuint VBO2;
		GLuint VBO3;
		GLuint EBO;
		std::vector<glm::vec3> vert_positions;
		std::vector<glm::vec3> vert_normals;
		std::vector<glm::vec2> tex_coords;
		std::vector<unsigned int> vert_indices;
		unsigned int tex_handle; 
		std::string mName;
	};

	struct Texture{
		GLuint textureID;
		std::string image_name;
	};

	


	public:
		unsigned int num_meshes;
		std::vector<Mesh> mesh_list;
		std::vector<Texture> texture_list;
		Model_Mesh();
		Model_Mesh(const char* model_path, std::string texture_path);
		void set_Model(const char* model_path, std::string texture_path);
	private:
		void load_model(const char* textures_path);
		void set_buffer_data(unsigned int index);
		int  is_image_loaded(std::string file_name);
		unsigned int load_texture_image(std::string file_name, bool& load_complete);


};

#endif