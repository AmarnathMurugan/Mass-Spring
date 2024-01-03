#pragma once

#include<fstream>
#include<string>
#include<Cy/cyTriMesh.h>
#include "gfx.h"
#include <chrono>

namespace CustomUtils
{

	struct Stopwatch {
		std::chrono::time_point<std::chrono::high_resolution_clock> start;
		std::string funcName;
		Stopwatch(std::string _funcName = "")
		{
			start = std::chrono::high_resolution_clock::now();
			funcName = _funcName;
		};

		~Stopwatch()
		{
			auto end = std::chrono::high_resolution_clock::now();
			std::chrono::duration<float> duration = end - start;
			std::cout << "\nTime taken by "<< funcName << " :" << duration.count() << "s";
		}
	};

	struct PairHash
	{
		template<class T1, class T2>
		std::size_t operator()(const std::pair<T1, T2>& v) const
		{
			return std::hash<T1>()(v.first) ^ std::hash<T2>()(v.second);
		}
	};

	template<typename T>
	inline T lerp(T a, T b, T t)
	{
		return a * (1 - t) + b * t;
	}

	static std::string getStringFromFile(std::string name)
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
		_position.resize((uint32_t)(model.NV() * 1.1), 3);
		_position.setZero();
		_normal = _position;
		_faces.resize((uint32_t)model.NF(), 3);
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
				if (Eigen::Vector3f(_position(vert, Eigen::all)) == Eigen::Vector3f::Zero() && Eigen::Vector3f(_normal(vert, Eigen::all)) == Eigen::Vector3f::Zero())
				{
					_position(vert, Eigen::all) = Eigen::Vector3f(model.V(vert).x, model.V(vert).y, model.V(vert).z);
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

					if (isDealtWith)
						continue;

					//Create duplicate vertex if not dealt with
					if (size >= _position.rows())
					{
						_position.conservativeResize((uint32_t)(_position.rows() * 1.1), 3);
						_normal.conservativeResize((uint32_t)(_normal.rows() * 1.1), 3);
					}
					_position(size, Eigen::all) = Eigen::Vector3f(model.V(vert).x, model.V(vert).y, model.V(vert).z);
					_normal(size, Eigen::all) = curNormal;
					model.F(i).v[j] = size;
					size++;
				}
			}
			_faces(i, 0) = model.F(i).v[0];
			_faces(i, 1) = model.F(i).v[1];
			_faces(i, 2) = model.F(i).v[2];
			vertAdjacency[model.F(i).v[0]].emplace_back(model.F(i).v[1]);
			vertAdjacency[model.F(i).v[0]].emplace_back(model.F(i).v[2]);
			vertAdjacency[model.F(i).v[1]].emplace_back(model.F(i).v[0]);
			vertAdjacency[model.F(i).v[1]].emplace_back(model.F(i).v[2]);
			vertAdjacency[model.F(i).v[2]].emplace_back(model.F(i).v[0]);
			vertAdjacency[model.F(i).v[2]].emplace_back(model.F(i).v[1]);
		}
	}

	template<typename T>
	inline T radians(T degree)
	{
		return degree * (T)PI_F / (T)180;
	}

	template<typename T>
	inline T degrees(T radian)
	{
		return radian * (T)180 / (T)PI_F;
	}

	template<typename T>
	inline T clamp(T val, T min, T max)
	{
		if (val < min)
			return min;
		else if (val > max)
			return max;
		else
			return val;
	}

	template<typename T>
	inline bool epsEqual(T a, T b, T eps = 0.0001)
	{
		return std::abs(a - b) < eps;
	};

	static Eigen::Vector3f spherePoint(float theta, float phi)
	{
		return {
			std::cos(theta) * std::sin(phi),
			std::sin(theta),
			std::cos(theta) * std::cos(phi),
		};
	}

	static Eigen::Vector2f pointSphere(const Eigen::Vector3f& p)
	{
		return { std::atan2(p.y(), std::sqrt(p.x() * p.x() + p.z() * p.z())),
				 std::atan2(p.x(), p.z()) + PI_F * 0.5f };
	}

	static Eigen::Quaternionf eulerToQuaternion(Eigen::Vector3f euler)
	{
		return Eigen::AngleAxisf(euler.x(), Eigen::Vector3f::UnitX()) *
			Eigen::AngleAxisf(euler.y(), Eigen::Vector3f::UnitY()) *
			Eigen::AngleAxisf(euler.z(), Eigen::Vector3f::UnitZ());
	}
}