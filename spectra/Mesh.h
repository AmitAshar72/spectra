#ifndef MESH_CLASS_H
#define MESH_CLASS_H

#include<string>

#include "VAO.h"
#include "EBO.h"
#include "Camera.h"
#include "Texture.h"
#include "BoundingBox.h"

class Mesh
{
public:
	std::vector <Vertex> vertices;
	std::vector <GLuint> indices;
	std::vector <Texture> textures;
    glm::vec3 Position;
	// Store VAO in public so it can be used in the Draw function
	VAO VAO;

	// Initializes the mesh
	Mesh(std::vector <Vertex>& vertices, std::vector <GLuint>& indices, std::vector <Texture>& textures);

    void calculateBoundingBox(Mesh* mesh) {
        // Calculate the minimum and maximum extents of the model
        // ...

        for (unsigned int i = 0; i < mesh->vertices.size(); i++)
        {
            minX = std::min(minX, mesh->vertices[i].position.x);
            maxX = std::max(maxX, mesh->vertices[i].position.x);
            minY = std::min(minY, mesh->vertices[i].position.y);
            maxY = std::max(maxY, mesh->vertices[i].position.y);
            minZ = std::min(minZ, mesh->vertices[i].position.z);
            maxZ = std::max(maxZ, mesh->vertices[i].position.z);
        }

        std::cout << mesh << std::endl;
        std::cout << "X: (" << minX << ", " << maxX << ") " 
                  << "Y: (" << minY << ", " << maxY << ") " 
                  << "Z: (" << minZ << ", " << maxZ << ") " << std::endl;

        // Calculate the center, width, height, and depth of the bounding box
        boundingBox = BoundingBox(
            glm::vec3((minX + maxX) / 2.0f, (minY + maxY) / 2.0f, (minZ + maxZ) / 2.0f),
            maxX - minX, maxY - minY, maxZ - minZ, minX, minY, minZ, maxX, maxY, maxZ
        );
    }

    void UpdateBoundingBoxScale(glm::vec3& scale) 
    {
        minX *= scale.x;
        maxX *= scale.x;
        minY *= scale.y;
        maxY *= scale.y;
        minZ *= scale.z;
        maxZ *= scale.z;
        
        std::cout << "X: (" << minX << ", " << maxX << ") "
            << "Y: (" << minY << ", " << maxY << ") "
            << "Z: (" << minZ << ", " << maxZ << ") " << std::endl;

        // Calculate the center, width, height, and depth of the bounding box
        boundingBox = BoundingBox(
            glm::vec3((minX + maxX) / 2.0f, (minY + maxY) / 2.0f, (minZ + maxZ) / 2.0f),
            maxX - minX, maxY - minY, maxZ - minZ, minX, minY, minZ, maxX, maxY, maxZ
        );
    }

	// Draws the mesh
	void Draw(Shader& shader);

    BoundingBox GetMeshBoundingBox()
    {
        return boundingBox;
    }

    void SetMeshProperties(Shader& shader, Camera& cam, glm::vec3& position, glm::vec3& rotation, glm::vec3& scale);

private:

    BoundingBox boundingBox;

    // Initialize minimum and maximum extents
    float minX = std::numeric_limits<float>::max();
    float maxX = std::numeric_limits<float>::lowest();
    float minY = std::numeric_limits<float>::max();
    float maxY = std::numeric_limits<float>::lowest();
    float minZ = std::numeric_limits<float>::max();
    float maxZ = std::numeric_limits<float>::lowest();
};
#endif