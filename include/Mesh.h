#ifndef MESH_H
#define MESH_H

#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>

struct Vertex 
{
	glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texcoord;
    Vertex(glm::vec3 p, glm::vec3 n): position{p}, normal{n} {}
    Vertex(glm::vec3 p, glm::vec2 t): position{p}, texcoord{t} {}
    Vertex(glm::vec3 p, glm::vec3 n, glm::vec2 t): position{p}, normal{n}, texcoord{t} {}
};

class Mesh 
{
public:
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	GLuint VAO;
	GLuint VBO;
	GLuint EBO;

	Mesh(std::vector<Vertex> v, std::vector<unsigned int> i): vertices{v}, indices{i}
	{
		setupMesh();
	}

	void setupMesh()
	{
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), indices.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texcoord));
        glEnableVertexAttribArray(2);
		glBindVertexArray(0);
	}

	void draw()
	{
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, (void*)0);
		glBindVertexArray(0);
	}
};

#endif