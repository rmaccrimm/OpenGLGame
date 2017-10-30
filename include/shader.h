#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader
{
public:
    unsigned int ID;
    Shader(const std::string vertex_path, const std::string fragment_path);
    void use();
    void set_bool(const std::string &name, bool value) const;
    void set_int(const std::string &name, int value) const;
    void set_float(const std::string &name, float value) const;
    void set_mat4(const std::string &name, glm::mat4 value) const;
    void set_vec3(const std::string &name, glm::vec3 value) const;
    void set_vec3(const std::string &name, float x, float y, float z) const;
private:
    void check_compile_errors(unsigned int shader, std::string type);
};

#endif
    
Shader::Shader(const std::string vertex_path, const std::string fragment_path)
{
    std::string vertex_code, fragment_code;
    std::ifstream vshader_file, fshader_file;
    // ensure ifstream objects can throw exceptions
    vshader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fshader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
		vshader_file.open(vertex_path);
		fshader_file.open(fragment_path);
		std::stringstream vshader_stream, fshader_stream;
		vshader_stream << vshader_file.rdbuf();
		fshader_stream << fshader_file.rdbuf();
		vshader_file.close();
		fshader_file.close();
		vertex_code = vshader_stream.str();
		fragment_code = fshader_stream.str();
    } catch(std::ifstream::failure e) {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
        std::cout << "FRAGMENT: " << fragment_path << std::endl;
        std::cout  << "VERTEX: " << vertex_path << std::endl;
    }
    const char* vshader_code = vertex_code.c_str();
    const char* fshader_code = fragment_code.c_str();
    unsigned int vertex, fragment;
    
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vshader_code, NULL);
    glCompileShader(vertex);
    check_compile_errors(vertex, "VERTEX");
    
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fshader_code, NULL);
    glCompileShader(fragment);
    check_compile_errors(fragment, "FRAGMENT");

    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
    check_compile_errors(ID, "PROGRAM");
}

void Shader::use()
{
    glUseProgram(ID);
}

void Shader::set_bool(const std::string &name, bool value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value); 
}

void Shader::set_int(const std::string &name, int value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);     
}

void Shader::set_float(const std::string &name, float value) const
{
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);     
}

void Shader::set_mat4(const std::string &name, glm::mat4 value) const
{
	auto location = glGetUniformLocation(ID, name.c_str());
    assert(location != -1);
    if (location == -1) {
        std::cout << "ERROR::SHADER::UNIFORM_NOT_FOUND " << name.c_str() << std::endl;
    }
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::set_vec3(const std::string &name, glm::vec3 value) const
{
    glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(value));
}

void Shader::set_vec3(const std::string &name, float x, float y, float z) const
{
    glm::vec3 v(x, y, z);
    glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(v));
}
    
void Shader::check_compile_errors(unsigned int shader, std::string type)
{
    int success;
    char info_log[512];
    if (type == "PROGRAM") {
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(shader, 512, NULL, info_log);
			std::cout << "ERROR::SHADER::" << type << "::LINKING_FAILED" << info_log
				  << std::endl;
		}
    } else {
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if(!success) {
			glGetShaderInfoLog(shader, 512, NULL, info_log);
			std::cout << "ERROR::SHADER::" << type << "::COMPILATION_FAILED" << info_log
				  << std::endl;
		}
    }
}
