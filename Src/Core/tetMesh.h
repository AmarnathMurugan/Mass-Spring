#pragma once

#include "includes.h"
#include "sceneObject.h"

class TetMesh : public SceneObject
{
public:
	void normalizeModel();
	void setBuffers();
	void update();
	void computeNormals();
	void initTetMesh();

public:
	struct TetData
	{
		MatrixX3fRowMajor vertices;
		MatrixX3fRowMajor normals;
		MatrixX4UIRowMajor tetrahedra;
		MatrixX3UIRowMajor faces;
		uint32_t numBdryVertices;
	}tetData;
	bool isDirty = false;

private:
	GLuint VAO;
	GLuint VBO[2];
	GLuint EBO;
};