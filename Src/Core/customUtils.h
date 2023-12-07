#pragma once

#include<fstream>
#include<string>
#include<Cy/cyTriMesh.h>
#include "gfx.h"


/// <summary>
/// Linearly Inpolates between a and b by t %
/// </summary>
/// <param name="a">Starting Value</param>
/// <param name="b">Target Value</param>
/// <param name="t">Interpolation Amount</param>
/// <returns></returns>
inline float Lerp(float a, float b, float t)
{
	return a * (1 - t) + b * t;
}

static std::string GetStringFromFile(std::string name)
{
	std::ifstream f(name);
	if (!f.is_open())
		return "\0";

	std::string content;
	std::string str = "";

	while (!f.eof())
	{
		std::getline(f, str);
		content.append(str + "\n");
	}
	f.close();

	return content;
}

static void importObjModel(std::string objName, 
					bool loadMat, 
					MatrixX3fRowMajor& _position,
					MatrixX3fRowMajor& _normal,
					MatrixX3UIRowMajor& _faces,
					std::unordered_map<int, std::vector<int>>& vertAdjacency)
{
	cy::TriMesh model;
	if (!model.LoadFromFileObj(objName.data(), loadMat))
	{
		std::cerr << "\n Unable to load obj " << objName;
		return;
	}

	//// TODO: check the initial value of vertexData.position and vertexData.normal and see if the comparison to deafault constructor is valid
	_position.resize((uint32_t)(model.NV() * 1.1),3);
	_position.setZero();
	_normal = _position;
	int size = model.NV();
	// Iterate through faces
	for (int i = 0; i < model.NF(); i++)
	{		
		// Iterate through vertices and see if there are any of them have same position but different normals, create a duplicate vertex if so
		for (int j = 0; j < 3; j++)
		{
			int vert = model.F(i).v[j];
			int norm = model.FN(i).v[j];
			Eigen::Vector3f curNormal = Eigen::Vector3f(model.VN(norm).x, model.VN(norm).y, model.VN(norm).z);
			//if the vertex is not yet dealt with
			if (Eigen::Vector3f(_position(vert,Eigen::all)) == Eigen::Vector3f::Zero() && Eigen::Vector3f(_normal(vert,Eigen::all)) == Eigen::Vector3f::Zero())
			{
				_position(vert,Eigen::all) = Eigen::Vector3f(model.V(vert).x, model.V(vert).y, model.V(vert).z);
				_normal(vert, Eigen::all) = curNormal;
			}
			// if the vertex is already dealt with but the normal is different (Conversion to Vector3f is done becuase != and == doesn't work for sliced matrix and vector3f)		
			else if (Eigen::Vector3f(_normal(vert, Eigen::all)) != curNormal && size > model.NV())
			{
				bool isDealtWith = false;
				//Check in duplicates
				for (int k = model.NV() - 1; k < size; k++)
				{
					if (_normal(vert, Eigen::all) == _normal(k, Eigen::all) && _position(vert, Eigen::all) == _position(k, Eigen::all))
					{
						// if the vertex is already dealth with, set the index to the duplicate vertex
						model.F(i).v[j] = k;
						isDealtWith = true;
					}
				}

				if(isDealtWith)
					continue;
				
				//Create duplicate vertex if not dealt with
				if (size >= _position.rows())
				{
					_position.conservativeResize((uint32_t)(_position.rows() * 1.1), 3);
					_normal.conservativeResize((uint32_t)(_normal.rows() * 1.1), 3);
				}
				_position(size,Eigen::all) = Eigen::Vector3f(model.V(vert).x, model.V(vert).y, model.V(vert).z);
				_normal(size,Eigen::all) = curNormal;
				model.F(i).v[j] = size;
				size++;
			}
		}
		_faces(i,Eigen::all) << model.F(i).v[0], model.F(i).v[1], model.F(i).v[2];
		vertAdjacency[model.F(i).v[0]].emplace_back(model.F(i).v[1]);
		vertAdjacency[model.F(i).v[0]].emplace_back(model.F(i).v[2]);
		vertAdjacency[model.F(i).v[1]].emplace_back(model.F(i).v[0]);
		vertAdjacency[model.F(i).v[1]].emplace_back(model.F(i).v[2]);
		vertAdjacency[model.F(i).v[2]].emplace_back(model.F(i).v[0]);
		vertAdjacency[model.F(i).v[2]].emplace_back(model.F(i).v[1]);
	}
}


