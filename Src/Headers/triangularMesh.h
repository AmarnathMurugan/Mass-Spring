#ifndef TRIANGULARMESH_H
#define TRIANGULARMESH_H

#include "includes.h"
#include<Cy/cyTriMesh.h>
#include<material.h>
#include<sceneObject.h>


struct Vertdata
{
	std::vector<Eigen::Vector3f> position;
	std::vector<Eigen::Vector3f> normal;
};


class TraingularMesh : public SceneObject
{
public:
	Vertdata vertexData;
	std::vector<std::array<uint32_t,3>> faceIndices;
	std::unordered_map<int,std::vector<int>> vertAdjacency;

public:
	TraingularMesh(std::vector<Eigen::Vector3f> _position,
					std::vector<Eigen::Vector3f> _normal,
					std::vector<std::array<uint32_t, 3>>& _faces,
					std::unordered_map<int, std::vector<int>>& _vertAdjacency);
	void generateBuffers();
	virtual void render();
	~TraingularMesh();
};



#endif 
