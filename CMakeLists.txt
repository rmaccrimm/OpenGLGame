project("OpenGL Game")
cmake_minimum_required(VERSION 3.9)

include_directories(include ../include)
link_directories(lib)

add_library(glad src/glad.c)

add_executable(openglGame src/openglGame.cpp)

target_link_libraries(openglGame opengl32 glfw3 glad assimp-vc140-mt)
add_dependencies(openglGame glad)


