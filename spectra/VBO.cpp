#include "VBO.h"

VBO::VBO(std::vector<Vertex>& vertices)
{
	glGenBuffers(1, &ID);
	//Bind newly generated buffer
	glBindBuffer(GL_ARRAY_BUFFER, ID);
	//Copies previously defined vertices into buffer's memory
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
}

void VBO::Bind()
{
	//Bind newly generated buffer
	glBindBuffer(GL_ARRAY_BUFFER, ID);
}

void VBO::Unbind()
{
	//Unbind generated buffer
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VBO::Delete()
{
	glDeleteBuffers(1, &ID);
}
