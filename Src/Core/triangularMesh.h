#pragma once

#include "includes.h"
#include<material.h>
#include<sceneObject.h>


struct Vertdata
{
	MatrixX3fRowMajor position;
	MatrixX3fRowMajor normal;
};


class TriangularMesh : public SceneObject
{
public:
	Vertdata vertexData;
	MatrixX3UIRowMajor faceIndices;
	std::unordered_map<int,std::vector<int>> vertAdjacency;
	bool isDirty = false;
	Eigen::Vector3f boundingBoxMin, boundingBoxMax;

private:
	GLuint VAO = GL_INVALID_INDEX, EBO = GL_INVALID_INDEX;
	GLuint VBO[2] = { GL_INVALID_INDEX, GL_INVALID_INDEX };

public:
	TriangularMesh():SceneObject(){};
	TriangularMesh(const MatrixX3fRowMajor& _position,
					const MatrixX3fRowMajor& _normal,
					const MatrixX3UIRowMajor& _faces,
					std::unordered_map<int, std::vector<int>>& _vertAdjacency);
	void generateBuffers();
	void computeBoundingBox(bool normalizeModel = false);
	void update();
	virtual void render() override;
	~TriangularMesh();
};
