#pragma once

#include "includes.h"
#include "sceneObject.h"

class TetMesh : public SceneObject
{
public:
	void normalizeModel();
	void setBuffers();
	virtual void update() override;
	void computeNormals();
	void initTetMesh(Eigen::Vector3f offset = Eigen::Vector3f::Zero());
	void offsetVertices(const Eigen::Vector3f& offset);
	virtual void render() override;

public:
	struct TetData
	{
		Eigen::VectorXf vertices;
		MatrixX3fRowMajor normals;
		MatrixX4UIRowMajor tetrahedra;
		MatrixX3UIRowMajor faces;
		Eigen::VectorXi faceInteriorVertexIndices;
		std::unordered_map<int, std::unordered_set<int>> vertAdjacency;
		uint32_t numBdryVertices;
	}tetData;
	bool isDirty = false;

private:
	GLuint VAO;
	GLuint VBO[2];
	GLuint EBO;
};