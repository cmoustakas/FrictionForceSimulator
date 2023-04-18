# FrictionForceSimulator
&nbsp;&nbsp;&nbsp;The current software is attempting to simulate the vehicle's potential slide under constant acceleration on a turn. It is important to underline that the aforementioned software is just a fun, non-optimized project and as a consequence the reader can feel free to use, modify, improve and play with it. It is notable to mention that this software was developed in C++ using relatively low level APIs and Libraries such as modern OpenGL, custom fragment-vertex shaders through GLSL, Dear ImGUI, openAssimp and CMake, while the "handmade" Objects-meshes were constructed through Blender. For brief explanation of physics of the project checkout **README.pdf**.
In order to build the software successfully make sure you have installed cmake 3.x gcc, Assimp, stbimage and OpenGL to your machine. Make a directory /build in the projects parent folder and cmake it:
  
```bash
$ mkdir build && cd build   
$ cmake ..  
$ make  
$ ./CarFrctionForceSimulator  
```

Enjoy :smiling_face_with_three_hearts:


https://user-images.githubusercontent.com/26367117/232768002-c19e880d-6bbf-4bf1-9470-fdd2274eb0d7.mp4

