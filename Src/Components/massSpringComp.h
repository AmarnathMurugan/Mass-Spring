#pragma once

#include "includes.h"
#include "Component.h"
#include "tetMesh.h"

class MassSpring : public Component
{
public:
	MassSpring(std::shared_ptr<TetMesh> _tetMesh);
	virtual void Start() override;
	virtual void update() override;
	virtual void fixedUpdate(float dt) override;
	~MassSpring();

private:
	std::shared_ptr<TetMesh> tetMesh;

};