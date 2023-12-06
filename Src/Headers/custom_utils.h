#ifndef CUSTOM_UTILS_H
#define CUSTOM_UTILS_H

#include<fstream>
#include<string>



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

std::string GetStringFromFile(const char* name)
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

void importObjModel(const char* objName, 
					bool loadMat, 
					std::vector<Eigen::Vector3f>& position, 
					std::vector<Eigen::Vector3f>& normal, 
					std::vector<std::array<uint32_t,3>>& faces,
					std::unordered_map<int, std::vector<int>>& vertAdjacency)
{
	cy::TriMesh model;
	if (!model.LoadFromFileObj(objName, loadMat))
	{
		std::cerr << "\n Unable to load obj " << objName;
		return;
	}

	//// TODO: check the initial value of vertexData.position and vertexData.normal and see if the comparison to deafault constructor is valid
	position.resize((uint32_t)(model.NV() * 1.1));
	normal.resize((uint32_t)(model.NV() * 1.1));
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
			if (position[vert] == Eigen::Vector3f::Zero() && normal[vert] == Eigen::Vector3f::Zero())
			{
				position[vert] = Eigen::Vector3f(model.V(vert).x, model.V(vert).y, model.V(vert).z);
				normal[vert] = curNormal;
			}
			// if the vertex is already dealt with but the normal is different
			else if (normal[vert] != curNormal && size > model.NV())
			{

				//Check in duplicates
				for (int k = model.NV() - 1; k < size; k++)
				{
					if (normal[vert] == normal[k] && position[vert] == position[k])
					{
						// if the vertex is already dealth with, set the index to the duplicate vertex
						model.F(i).v[j] = k;
						continue;
					}
				}

				//Create duplicate vertex if not dealt with

				if (size >= position.size())
				{
					position.resize((uint32_t)(model.NV() * 1.1));
					normal.resize((uint32_t)(model.NV() * 1.1));
				}
				position[size] = Eigen::Vector3f(model.V(vert).x, model.V(vert).y, model.V(vert).z);
				normal[size] = curNormal;
				model.F(i).v[j] = size;
				size++;
			}
		}
		faces.emplace_back(std::array<uint32_t, 3>{model.F(i).v[0], model.F(i).v[1], model.F(i).v[2]});
		vertAdjacency[model.F(i).v[0]].emplace_back(model.F(i).v[1]);
		vertAdjacency[model.F(i).v[0]].emplace_back(model.F(i).v[2]);
		vertAdjacency[model.F(i).v[1]].emplace_back(model.F(i).v[0]);
		vertAdjacency[model.F(i).v[1]].emplace_back(model.F(i).v[2]);
		vertAdjacency[model.F(i).v[2]].emplace_back(model.F(i).v[0]);
		vertAdjacency[model.F(i).v[2]].emplace_back(model.F(i).v[1]);
	}
}

#endif 
