#include "massSpringComp.h"

MassSpring::MassSpring(std::shared_ptr<TetMesh> _tetMesh)
{
	tetMesh = _tetMesh;
}

void MassSpring::Start()
{
	std::cout << "\nMassSpring Start" << std::endl;
}

void MassSpring::update()
{
}

void MassSpring::fixedUpdate(float dt)
{

}

MassSpring::~MassSpring()
{
}