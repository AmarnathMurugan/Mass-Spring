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
	assert(VAO != GL_INVALID_INDEX);
	glBindVertexArray(VAO) ;
	glDrawElements(GL_TRIANGLES, faceIndices.size(), GL_UNSIGNED_INT, 0) ;
}


void TriangularMesh::generateBuffers()
{
	glGenVertexArrays(1, &VAO) ;
	glGenBuffers(2, VBO) ;
	glBindVertexArray(VAO) ;

	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, vertexData.position.size() * sizeof(float), vertexData.position.data(), GL_STATIC_DRAW) ;

	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, vertexData.normal.size() * sizeof(float), vertexData.normal.data(), GL_STATIC_DRAW) ;


	glGenBuffers(1, &EBO) ;
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO) ;
	uint32_t test = faceIndices.size();
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * faceIndices.size(), faceIndices.data(), GL_STATIC_DRAW) ;

	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]) ;
	glEnableVertexAttribArray(0) ;
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0) ;

	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]) ;
	glEnableVertexAttribArray(1) ;
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0) ;
}

void TriangularMesh::computeBoundingBox(bool normalizeModel)
{
	this->boundingBoxMin = vertexData.position.colwise().minCoeff();
	this->boundingBoxMax = vertexData.position.colwise().maxCoeff();
	Eigen::Vector3f center = (this->boundingBoxMin + this->boundingBoxMax) / 2;
	Eigen::Vector3f size = boundingBoxMax - boundingBoxMin;
	float scale = 1.0f;
	if (normalizeModel)
	{
		scale = 1.0f / size.maxCoeff();
		vertexData.position = (vertexData.position.rowwise() - center.transpose()) * scale;
	}
}

void TriangularMesh::update()
{
	SceneObject::update();
	if (isDirty)
	{		
		// recalculate face normals and update buffer in parallel
		vertexData.normal.setZero();
		for (int i = 0; i < faceIndices.size(); i++)
		{
			Eigen::Vector3f v1 = vertexData.position.row(faceIndices(i,0));
			Eigen::Vector3f v2 = vertexData.position.row(faceIndices(i,1));
			Eigen::Vector3f v3 = vertexData.position.row(faceIndices(i,2));
			Eigen::Vector3f normal = (v2 - v1).cross(v3 - v1);
			vertexData.normal.row(faceIndices(i, 0)) += normal;
			vertexData.normal.row(faceIndices(i, 1)) += normal;
			vertexData.normal.row(faceIndices(i, 2)) += normal;
		}		
		vertexData.normal.rowwise().normalize();
		isDirty = false;	
		glBindVertexArray(VAO) ;
		glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, vertexData.position.size() * sizeof(float), vertexData.position.data()) ;
		glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, vertexData.normal.size() * sizeof(float), vertexData.normal.data()) ;
	}
}

TriangularMesh::~TriangularMesh()
{
	glDeleteBuffers(2, VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteVertexArrays(1, &VAO);
}