#include "triangularMesh.h"


TraingularMesh::TraingularMesh(std::vector<Eigen::Vector3f> _position,
	std::vector<Eigen::Vector3f> _normal,
	std::vector<std::array<uint32_t, 3>>& _faces,
	std::unordered_map<int, std::vector<int>>& _vertAdjacency): SceneObject() , vertexData({ _position,_normal }), faceIndices(_faces), vertAdjacency(_vertAdjacency)
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
	glBufferData(GL_ARRAY_BUFFER, sizeof(Eigen::Vector3f) * vertexData.position.size() * 2, NULL, GL_STATIC_DRAW);

	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * faceIndices.size() * 3, &faceIndices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof(Eigen::Vector3f) * vertexData.position.size()));
}

TraingularMesh::~TraingularMesh()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}