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
	glBindVertexArray(VAO) $GL_CATCH_ERROR;
	glDrawElements(GL_TRIANGLES, faceIndices.size() * 3, GL_UNSIGNED_INT, 0) $GL_CATCH_ERROR;
}


void TriangularMesh::generateBuffers()
{
	glGenVertexArrays(1, &VAO) $GL_CATCH_ERROR;
	glGenBuffers(1, &VBO) $GL_CATCH_ERROR;
	glBindVertexArray(VAO) $GL_CATCH_ERROR;

	glBindBuffer(GL_ARRAY_BUFFER, VBO) $GL_CATCH_ERROR;
	glBufferData(GL_ARRAY_BUFFER, vertexData.position.size() * sizeof(float) * 2, vertexData.position.data(), GL_STATIC_DRAW) $GL_CATCH_ERROR;

	glGenBuffers(1, &EBO) $GL_CATCH_ERROR;
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO) $GL_CATCH_ERROR;
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * faceIndices.size(), faceIndices.data(), GL_STATIC_DRAW) $GL_CATCH_ERROR;
	glEnableVertexAttribArray(0) $GL_CATCH_ERROR;
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0) $GL_CATCH_ERROR;

	glEnableVertexAttribArray(1) $GL_CATCH_ERROR;
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof(float) * vertexData.position.size())) $GL_CATCH_ERROR;
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
	glBindVertexArray(VAO) $GL_CATCH_ERROR;
	glBindBuffer(GL_ARRAY_BUFFER, VBO) $GL_CATCH_ERROR;
	glBufferSubData(GL_ARRAY_BUFFER, 0, vertexData.position.size() * sizeof(float) * 2, vertexData.position.data()) $GL_CATCH_ERROR;
}

TriangularMesh::~TriangularMesh()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}