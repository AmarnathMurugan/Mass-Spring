#ifndef INCLUDES_H
#define INCLUDES_H

#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<vector>
#include<unordered_set>
#include<unordered_map>
#include<Eigen/Core>
#include<Eigen/Dense>
#include<Eigen/Geometry>
#include "custom_utils.h"
#include "graphicsInfo.h"

const float  PI_F = 3.14159265358979f;
Eigen::Vector3f ORIGIN = Eigen::Vector3f::Zero();
Eigen::Vector3f UP = Eigen::Vector3f(0, 1, 0);
Eigen::Vector3f RIGHT = Eigen::Vector3f(1, 0, 0);
Eigen::Vector3f FORWARD = Eigen::Vector3f(0, 0, 1);
Eigen::Vector3f LEFT = Eigen::Vector3f(-1, 0, 0);
Eigen::Vector3f DOWN = Eigen::Vector3f(0, -1, 0);
Eigen::Vector3f BACK= Eigen::Vector3f(0, 0, -1);


typedef Eigen::Transform<float, 3, Eigen::Affine> Transform;

#endif // !INCLUDES_H

