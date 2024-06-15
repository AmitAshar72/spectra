#include "Mesh.h"
#include "Mesh.h"

Mesh::Mesh(std::vector <Vertex>& vertices, std::vector <GLuint>& indices, std::vector <Texture>& textures)
{
	Mesh::vertices = vertices;
	Mesh::indices = indices;
	Mesh::textures = textures;

	VAO.Bind();
	// Generates Vertex Buffer Object and links it to vertices
	VBO VBO(vertices);
	// Generates Element Buffer Object and links it to indices
	EBO EBO(indices);

	//Position attribute
	VAO.LinkAttrib(VBO, 0, 3, GL_FLOAT, sizeof(Vertex), (void*)0);
	//Normal attribute
	VAO.LinkAttrib(VBO, 1, 3, GL_FLOAT, sizeof(Vertex), (void*)(3 * sizeof(float)));
	//Color attribute
	VAO.LinkAttrib(VBO, 2, 3, GL_FLOAT, sizeof(Vertex), (void*)(6 * sizeof(float)));
	//Texture attribute
	VAO.LinkAttrib(VBO, 3, 2, GL_FLOAT, sizeof(Vertex), (void*)(9 * sizeof(float)));

	// Unbind all to prevent accidentally modifying them
	VAO.Unbind();
	VBO.Unbind();
	EBO.Unbind();

	calculateBoundingBox(this);
}


void Mesh::Draw(Shader& shader)
{
	// Bind shader to be able to access uniforms
	shader.Activate();
	VAO.Bind();

	// Keep track of how many of each type of textures we have
	unsigned int numDiffuse = 0;
	unsigned int numSpecular = 0;

	for (unsigned int i = 0; i < textures.size(); i++)
	{
		std::string num;
		std::string type = textures[i].type;
		if (type == "diffuse")
		{
			num = std::to_string(numDiffuse++);
		}
		else if (type == "specular")
		{
			num = std::to_string(numSpecular++);
		}

		//New Texture Unit
		textures[i].TextureUnit(shader, (type + num).c_str(), i);
		textures[i].Bind();
	}

	// Draw the actual mesh
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}

void Mesh::SetMeshProperties(Shader& shader, Camera& cam, glm::vec3& position, glm::vec3& rotation, glm::vec3& scale)
{
	//Activate Shader
	shader.Activate();
	shader.setVec3("viewPos", cam.Position);

	cam.UpdateCameraMatrix(shader);

	glm::mat4 meshMat = glm::mat4(1.0f);
	Position = position;
	meshMat = glm::translate(meshMat, position);	
	meshMat = glm::scale(meshMat, scale);

	shader.setMat4("model", meshMat);
}
