#pragma once

#include "includes.h"
#include "sceneObject.h"

class TetMesh : public SceneObject
{
public:
	TetMesh() = default;
	TetMesh(const TetMesh& other);
	void normalizeModel();
	void setBuffers();
	void update(const EngineState& engineState) override;
	void initNormals();
	void computeNormals();
	void initTetMesh(Eigen::Vector3d offset = Eigen::Vector3d::Zero());
	void offsetVertices(const Eigen::Vector3d& offset);
	void render() override;

public:
	struct TetData
	{
		Eigen::VectorXd vertices;
		MatrixX3fRowMajor normals;
		MatrixX4UIRowMajor tetrahedra;
		MatrixX3UIRowMajor faces;
		Eigen::VectorXi faceInteriorVertexIndices;
		std::unordered_map<int, std::unordered_set<int>> vertAdjacency;
		uint32_t numBdryVertices, maxVertexNeighbors;
	}tetData;
	bool isDirty = false;

private:
	GLuint VAO;
	GLuint VBO[2];
	GLuint EBO;
};