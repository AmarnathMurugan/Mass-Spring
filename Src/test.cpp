#include <iostream>
#include <Eigen/Core>
#include <Eigen/Dense>

void main()
{
	Eigen::Matrix3f d;
	d.setZero();
	d(0, Eigen::all) = Eigen::Vector3f(1,2,3);
	Eigen::Vector3f temp = d(1, Eigen::all);
	if(temp == Eigen::Vector3f(1, 2, 3))
		std::cout << "error" << std::endl;
	std::cout << d << std::endl;
}
