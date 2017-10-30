#ifndef MODEL_H
#define MODEL_H

#include <iostream>
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Mesh.h"

class Model 
{
public:
    Model(std::string filepath)
    {
        Assimp::Importer importer;
	    const aiScene *scene = importer.ReadFile(filepath, aiProcess_Triangulate);
	    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode){
    		std::cout << importer.GetErrorString() << std::endl;
        }
    	printNodeNames(scene->mRootNode, 0);
	    process_node(scene, scene->mRootNode, aiMatrix4x4());
    }

    void draw()
    {
        for (Mesh m: meshes) 
            m.draw();
    }

private:
    std::vector<Mesh> meshes;

    void printNodeNames(aiNode *root, unsigned int lvl)
    {
        std::cout << root->mName.C_Str() << " " << root->mNumMeshes << " meshes" << std::endl;	
        for (unsigned int i = 0; i < root->mNumChildren; i++)
        {
            for (unsigned int i = 0; i <= lvl; i++)
                std::cout << '\t';
            printNodeNames(root->mChildren[i], lvl + 1);
        }
    }

    void process_node(const aiScene * scene, const aiNode *node, aiMatrix4x4 parentTransform)
    {
        aiMatrix4x4 transform = parentTransform * node->mTransformation;
        aiMatrix4x4 ntransform = transform.Inverse().Transpose();
        for (unsigned int i = 0; i < node->mNumMeshes; i++) {
            std::cout << node->mName.C_Str() << std::endl;
            std::vector<Vertex> vertices;
            std::vector<unsigned int> indices;
            aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
            for (unsigned int j = 0; j < mesh->mNumVertices; j++) {
                aiVector3D position = ntransform * mesh->mVertices[j];
                aiVector3D normal = ntransform * mesh->mNormals[j];			
                glm::vec3 pos(position.x, position.y, position.z);
                glm::vec3 norm(normal.x, normal.y, normal.z);
                vertices.push_back(Vertex(pos, norm));
            }
            for (unsigned int j = 0; j < mesh->mNumFaces; j++) {
                aiFace face = mesh->mFaces[j];
                for (unsigned int k = 0; k < face.mNumIndices; k++) {
                    indices.push_back(face.mIndices[k]);
                }
            }
            meshes.push_back(Mesh(vertices, indices));
        }
        for (unsigned int i = 0; i < node->mNumChildren; i++) {
            process_node(scene, node->mChildren[i], transform);		
        }
    }   
};

#endif