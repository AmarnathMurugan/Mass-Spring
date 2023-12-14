#include "tetMesh.h"

void TetMesh::normalizeModel()
{
	// Compute bounding box
	Eigen::Vector3f min, max;
	min = Eigen::Map<MatrixX3fRowMajor>(this->tetData.vertices.data(), this->tetData.vertices.size() / 3, 3).colwise().minCoeff();
	max = Eigen::Map<MatrixX3fRowMajor>(this->tetData.vertices.data(), this->tetData.vertices.size() / 3, 3).colwise().maxCoeff();
	Eigen::Vector3f center = (min + max) / 2;
	Eigen::Vector3f size = max - min;
	float scale = 1.0f / size.maxCoeff();
	#pragma omp parallel for
	for (int i = 0; i < this->tetData.vertices.size() / 3; i++)
	{	
		this->tetData.vertices(Eigen::seqN(3 * i, 3)) = (this->tetData.vertices(Eigen::seqN(3 * i, 3)) - center) * scale;
	}
}

void TetMesh::setBuffers()
{
	// Set buffers
	glGenVertexArrays(1, &this->VAO);
	glBindVertexArray(this->VAO);

	glGenBuffers(2, this->VBO);

	glBindBuffer(GL_ARRAY_BUFFER, this->VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, this->tetData.vertices.size() * sizeof(float), this->tetData.vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, this->VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, this->tetData.normals.size() * sizeof(float), this->tetData.normals.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &this->EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->tetData.faces.size() * sizeof(uint32_t), this->tetData.faces.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, this->VBO[0]);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, this->VBO[1]);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
}

void TetMesh::update()
{
	SceneObject::update();
	if (this->isDirty)
	{
		computeNormals();
		this->isDirty = false;

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, this->tetData.vertices.size() * sizeof(float), this->tetData.vertices.data());
		glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, this->tetData.normals.size() * sizeof(float), this->tetData.normals.data());
	}
}

void TetMesh::computeNormals()
{
	//CustomUtils::Stopwatch stopwatch("TetMesh::computeNormals()");
	this->tetData.normals.setZero();
	Eigen::Vector<uint32_t,3> max = this->tetData.faces.colwise().maxCoeff();
	#pragma omp parallel for
	for (int i = 0; i < this->tetData.faces.rows(); i++)
	{
		Eigen::Vector3f v1 = this->tetData.vertices(Eigen::seqN(3 * this->tetData.faces(i, 0), 3));
		Eigen::Vector3f v2 = this->tetData.vertices(Eigen::seqN(3 * this->tetData.faces(i, 1), 3));
		Eigen::Vector3f v3 = this->tetData.vertices(Eigen::seqN(3 * this->tetData.faces(i, 2), 3));
		Eigen::Vector3f v4 = this->tetData.vertices(Eigen::seqN(3 * this->tetData.faceInteriorVertexIndices(i),3));
		Eigen::Vector3f normal = (v2 - v1).cross(v3 - v1).normalized();
		if(normal.dot(v4 - v1) > 0)
			normal *= -1;
		for (int j = 0; j < 3; j++)
		{
			#pragma omp atomic
			this->tetData.normals(this->tetData.faces(i, 0),j) += normal(j);
			#pragma omp atomic
			this->tetData.normals(this->tetData.faces(i, 1),j) += normal(j);
			#pragma omp atomic
			this->tetData.normals(this->tetData.faces(i, 2),j) += normal(j);
		}
	}
	this->tetData.normals.rowwise().normalize();
}

void TetMesh::initTetMesh(Eigen::Vector3f offset)
{
	this->normalizeModel();
	if (offset.squaredNorm() > 0.0f)
		this->offsetVertices(offset);
	this->tetData.normals.resize(this->tetData.vertices.size() / 3, 3);
	this->computeNormals();
	this->setBuffers();

	// calculate vertex adjacency
	for (int i = 0; i < this->tetData.tetrahedra.rows(); i++)
	{
		this->tetData.vertAdjacency[this->tetData.tetrahedra(i, 0)].emplace(this->tetData.tetrahedra(i, 1));
		this->tetData.vertAdjacency[this->tetData.tetrahedra(i, 0)].emplace(this->tetData.tetrahedra(i, 2));
		this->tetData.vertAdjacency[this->tetData.tetrahedra(i, 0)].emplace(this->tetData.tetrahedra(i, 3));
		this->tetData.vertAdjacency[this->tetData.tetrahedra(i, 1)].emplace(this->tetData.tetrahedra(i, 0));
		this->tetData.vertAdjacency[this->tetData.tetrahedra(i, 1)].emplace(this->tetData.tetrahedra(i, 2));
		this->tetData.vertAdjacency[this->tetData.tetrahedra(i, 1)].emplace(this->tetData.tetrahedra(i, 3));
		this->tetData.vertAdjacency[this->tetData.tetrahedra(i, 2)].emplace(this->tetData.tetrahedra(i, 0));
		this->tetData.vertAdjacency[this->tetData.tetrahedra(i, 2)].emplace(this->tetData.tetrahedra(i, 1));
		this->tetData.vertAdjacency[this->tetData.tetrahedra(i, 2)].emplace(this->tetData.tetrahedra(i, 3));
		this->tetData.vertAdjacency[this->tetData.tetrahedra(i, 3)].emplace(this->tetData.tetrahedra(i, 0));
		this->tetData.vertAdjacency[this->tetData.tetrahedra(i, 3)].emplace(this->tetData.tetrahedra(i, 1));
		this->tetData.vertAdjacency[this->tetData.tetrahedra(i, 3)].emplace(this->tetData.tetrahedra(i, 2));
	}

}

void TetMesh::offsetVertices(const Eigen::Vector3f& offset)
{
	#pragma omp parallel for
	for (int i = 0; i < this->tetData.vertices.size() / 3; i++)
	{
		this->tetData.vertices(Eigen::seqN(3 * i, 3)) = (this->tetData.vertices(Eigen::seqN(3 * i, 3)) + offset);
	}
}

void TetMesh::render()
{
	assert(VAO != GL_INVALID_INDEX);
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, this->tetData.faces.size(), GL_UNSIGNED_INT, 0);
}
