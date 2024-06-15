#ifndef MODEL_CLASS_H
#define MODEL_CLASS_H

#include "Mesh.h"
#include "BoundingBox.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <GLFW/glfw3.h>

class Model
{
public:
    glm::vec3 ModelScale = glm::vec3(1.0f);
    glm::vec3 ModelRotation = glm::vec3(1.0f);
    glm::vec3 ModelPosition = glm::vec3(0.0f);
    Model(std::string const& path, glm::vec3& initPos);
    virtual void Draw(Shader& shader, Camera& cam);
    
    // Function to smoothly lerp a value towards zero
    float lerpToZero(float value, float lerpRate, float deltaTime) {
        if (value > 0.0f) {
            value = glm::max(0.0f, value - lerpRate * deltaTime);
        }
        else if (value < 0.0f) {
            value = glm::min(0.0f, value + lerpRate * deltaTime);
        }
        return value;
    }

    // Function to smoothly lerp a value towards a target value
    float lerpToTarget(float value, float target, float lerpRate, float deltaTime) {
        value = glm::mix(value, target, lerpRate * deltaTime);
        return value;
    }

    void UpdateModelBounds() 
    {
        for (int i = 0; i < meshes.size(); ++i) 
        {
            BoundingBox mbb = meshes[i].GetMeshBoundingBox();
            minX = std::min(minX, mbb.getMinX());
            maxX = std::max(maxX, mbb.getMaxX());
            minY = std::min(minY, mbb.getMinY());
            maxY = std::max(maxY, mbb.getMaxY());
            minZ = std::min(minZ, mbb.getMinZ());
            maxZ = std::max(maxZ, mbb.getMaxZ());
        }
        UpdateBoundBoxAsPerScale();
    }

    void SetScale(glm::vec3& scale) 
    {
        ModelScale = scale;
        UpdateBoundBoxAsPerScale();
    }

    void SetRotation(glm::vec3& rotation) 
    {
        ModelRotation = rotation;
    }

    void UpdateBoundBoxAsPerScale() 
    {
        minX *= ModelScale.x;
        maxX *= ModelScale.x;
        minY *= ModelScale.y;
        maxY *= ModelScale.y;
        minZ *= ModelScale.z;
        maxZ *= ModelScale.z;
    }

    float getMinX() { return minX; }
    float getMinY() { return minY; }
    float getMinZ() { return minZ; }
    float getMaxX() { return maxX; }
    float getMaxY() { return maxY; }
    float getMaxZ() { return maxZ; }

private:
    // model data
    std::vector<Texture> textures_loaded;
    std::vector<Mesh> meshes;
    std::string directory;    
    
    BoundingBox boundingBox;

    // Initialize minimum and maximum extents
    float minX = std::numeric_limits<float>::max();
    float maxX = std::numeric_limits<float>::lowest();
    float minY = std::numeric_limits<float>::max();
    float maxY = std::numeric_limits<float>::lowest();
    float minZ = std::numeric_limits<float>::max();
    float maxZ = std::numeric_limits<float>::lowest();

    // Calculate bounding box dimensions
    float width = maxX - minX;
    float height = maxY - minY;
    float depth = maxZ - minZ;

    // Center the bounding box at the center of the vehicle
    float centerX = (minX + maxX) / 2.0f;
    float centerY = (minY + maxY) / 2.0f;
    float centerZ = (minZ + maxZ) / 2.0f;

    void loadModel(std::string const& path);
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string textureType);
};
#endif