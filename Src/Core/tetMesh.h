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
	void initTetMesh(Eigen::Vector3d offset = Eigen::Vector3d::Zero());
	void offsetVertices(const Eigen::Vector3d& offset);
	virtual void render() override;

public:
	struct TetData
	{
		Eigen::VectorXd vertices;
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