#ifndef TRIANGULARMESH_H
#define TRIANGULARMESH_H

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

public:
	TriangularMesh():SceneObject(){};
	TriangularMesh(const MatrixX3fRowMajor& _position,
					const MatrixX3fRowMajor& _normal,
					const MatrixX3UIRowMajor& _faces,
					std::unordered_map<int, std::vector<int>>& _vertAdjacency);
	void generateBuffers();
	void Update();
	virtual void render();
	~TriangularMesh();
};



#endif 
