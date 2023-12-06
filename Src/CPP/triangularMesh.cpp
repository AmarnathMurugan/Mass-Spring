#include "triangularMesh.h"


TraingularMesh::TraingularMesh(const char* objName, bool loadMat)
{	
	generateBuffers();
}

void TraingularMesh::render()
{
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, faceIndices.size() * 3, GL_UNSIGNED_INT, 0);
}


void TraingularMesh::generateBuffers()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertdata) * vertexData.size(), &vertexData[0], GL_STATIC_DRAW);

	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cy::TriMesh::TriFace) * model.NF(), &model.F(0), GL_STATIC_DRAW);

	glEnableVertexAttribArray(material->positionLocation);
	glVertexAttribPointer(material->positionLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Eigen::Vector3f) * 2, (void*)0);

	glEnableVertexAttribArray(material->normalLocation);
	glVertexAttribPointer(material->normalLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Eigen::Vector3f) * 2, (void*)offsetof(Vertdata, normal));
}

TraingularMesh::~TraingularMesh()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}