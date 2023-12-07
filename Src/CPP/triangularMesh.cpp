#include "triangularMesh.h"


TriangularMesh::TriangularMesh(const MatrixX3fRowMajor& _position,
	const MatrixX3fRowMajor& _normal,
	const MatrixX3UIRowMajor& _faces,
	std::unordered_map<int, std::vector<int>>& _vertAdjacency) : SceneObject() , vertexData({ _position,_normal }), faceIndices(_faces), vertAdjacency(_vertAdjacency)
{	
	generateBuffers();
}

void TriangularMesh::render()
{
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, faceIndices.size() * 3, GL_UNSIGNED_INT, 0);
}


void TriangularMesh::generateBuffers()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertexData.position.size() * sizeof(float) * 2, vertexData.position.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * faceIndices.size(), faceIndices.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof(float) * vertexData.position.size()));
}

void TriangularMesh::Update()
{
	SceneObject::Update();
	if (isDirty)
	{		
		// recalculate face normals and update buffer in parallel
		#pragma omp parallel for
		for (int i = 0; i < faceIndices.size(); i++)
		{
			Eigen::Vector3f v1 = vertexData.position(faceIndices(i,0),Eigen::all);
			Eigen::Vector3f v2 = vertexData.position(faceIndices(i,1),Eigen::all);
			Eigen::Vector3f v3 = vertexData.position(faceIndices(i,2),Eigen::all);
			Eigen::Vector3f normal = (v2 - v1).cross(v3 - v1);
			vertexData.normal(faceIndices(i, 0), Eigen::all) = normal;
			vertexData.normal(faceIndices(i, 1), Eigen::all) = normal;
			vertexData.normal(faceIndices(i, 2), Eigen::all) = normal;
		}		
		isDirty = false;	
	}
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, vertexData.position.size() * sizeof(float) * 2, vertexData.position.data());
}

TriangularMesh::~TriangularMesh()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}