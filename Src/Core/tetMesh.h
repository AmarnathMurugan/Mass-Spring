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
	void initTetMesh(Eigen::Vector3f offset = Eigen::Vector3f::Zero());
	void offsetVertices(const Eigen::Vector3f& offset);
	virtual void render() override;

public:
	struct TetData
	{
		MatrixX3fRowMajor vertices;
		MatrixX3fRowMajor normals;
		MatrixX4UIRowMajor tetrahedra;
		MatrixX3UIRowMajor faces;
		Eigen::VectorXi faceInteriorVertexIndices;
		uint32_t numBdryVertices;
	}tetData;
	bool isDirty = false;

private:
	GLuint VAO;
	GLuint VBO[2];
	GLuint EBO;
};