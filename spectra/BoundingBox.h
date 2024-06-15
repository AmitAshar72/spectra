#pragma once
#include <glm/glm.hpp>
class BoundingBox {
public:
    BoundingBox() {};
    BoundingBox(glm::vec3 center, float width, float height, float depth, float minX, float minY, float minZ, float maxX, float maxY, float maxZ)
        : center(center), width(width), height(height), depth(depth), minX(minX), minY(minY), minZ(minZ), maxX(maxX), maxY(maxY), maxZ(maxZ) {}

    glm::vec3 getCenter() { return center; }
    float getWidth() { return width; }
    float getHeight() { return height; }
    float getDepth() { return depth; }
    float getMinX() { return minX; }
    float getMinY() { return minY; }
    float getMinZ() { return minZ; }
    float getMaxX() { return maxX; }
    float getMaxY() { return maxY; }
    float getMaxZ() { return maxZ; }

private:
    glm::vec3 center;
    float width, height, depth;
    float minX, minY, minZ, maxX, maxY, maxZ;
};