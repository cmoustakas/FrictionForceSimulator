# CMake entry point
cmake_minimum_required (VERSION 3.0)
project (Friction LANGUAGES   CXX C )
#project (Friction LANGUAGES  CUDA CXX C )

#enable_language(CUDA)
#include(CheckLanguage)
#check_language(CUDA)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)


find_package(OpenGL REQUIRED)

#ASSIMP
find_package(ASSIMP REQUIRED)
if(ASSIMP_FOUND)
    include_directories(${ASSIMP_INCLUDE_DIR})
endif() 



if( CMAKE_BINARY_DIR STREQUAL CMAKE_SOURCE_DIR )
    message( FATAL_ERROR "Please select another Build Directory ! (and give it a clever name, like bin_Visual2012_64bits/)" )
endif()
if( CMAKE_SOURCE_DIR MATCHES " " )
	message( "Your Source Directory contains spaces. If you experience problems when compiling, this can be the cause." )
endif()
if( CMAKE_BINARY_DIR MATCHES " " )
	message( "Your Build Directory contains spaces. If you experience problems when compiling, this can be the cause." )
endif()



# Compile external dependencies 
add_subdirectory (external)

# On Visual 2005 and above, this module can set the debug working directory
cmake_policy(SET CMP0026 OLD)


if(INCLUDE_DISTRIB)
	add_subdirectory(distrib)
endif(INCLUDE_DISTRIB)



include_directories(
	
	external/glfw-3.1.2/include/
	external/glm-0.9.7.1/
	external/glew-1.13.0/include/
	.
)

set(CMAKE_BUILD_TYPE Debug)


set(ALL_LIBS
	${OPENGL_LIBRARY}
	glfw
	GLEW_1130
)

add_definitions(
	-DTW_STATIC
	-DTW_NO_LIB_PRAGMA
	-DTW_NO_DIRECT3D
	-DGLEW_STATIC
	-D_CRT_SECURE_NO_WARNINGS
	-w
)




# Executable
add_executable(CarFrictionForceSimulator 
	src/main.cpp
	src/render.hpp
	src/setup_openGL.cpp
	src/setup_openGL.hpp
	src/setup_camera.hpp
	src/shaders/vertex_shader_gear.glsl
	src/shaders/fragment_shader_gear.glsl
	
	common/texture.cpp
	common/texture.hpp
	common/objloader.cpp
	common/objloader.hpp
	common/text2D.hpp
	common/text2D.cpp

	
	
	src/imgui/imconfig.h
	src/imgui/imgui.cpp
	src/imgui/imgui.h
	src/imgui/imgui_demo.cpp
	src/imgui/imgui_draw.cpp
	src/imgui/imgui_impl_glfw.cpp
	src/imgui/imgui_impl_glfw.h
	src/imgui/imgui_internal.h
	src/imgui/imgui_tables.cpp
	src/imgui/imgui_widgets.cpp
	src/imgui/imstb_rectpack.h
	src/imgui/imstb_textedit.h
	src/imgui/imstb_truetype.h
	src/imgui/imgui_impl_opengl3.h
	src/imgui/imgui_impl_opengl3.cpp
	src/imgui/imgui_impl_opengl3_loader.h

)

target_link_libraries(CarFrictionForceSimulator 
	${ASSIMP_LIBRARIES}
)


target_link_libraries(CarFrictionForceSimulator
	${ALL_LIBS}
)
# Xcode and Visual working directories
set_target_properties(CarFrictionForceSimulator PROPERTIES XCODE_ATTRIBUTE_CONFIGURATION_BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/CarFrictionForceSimulator/")


