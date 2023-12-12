#pragma once

#include "includes.h"
#include "sceneObject.h"

class TetMesh : public SceneObject
{
public:
	TetMesh();
	~TetMesh();

private:
	struct TetData
	{
		MatrixX3fRowMajor vertices;

	};
};

TetMesh::TetMesh()
{
}

TetMesh::~TetMesh()
{
}