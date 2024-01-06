#include "ubo.h"
#include <iostream>

UBO::UBO()
{
	glGenBuffers(1, &this->uboID);
	std::cout << "UBO created with ID: " << this->uboID << "\n";
}

void UBO::initUBO(GLuint _bindingPoint, GLsizeiptr _dataSize)
{
	this->bind();
	glBufferData(GL_UNIFORM_BUFFER, _dataSize, nullptr, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, _bindingPoint, this->uboID);
	this->bindingPoint = _bindingPoint;
	this->dataSize = _dataSize;	
}

void UBO::setData(const void* _data)
{	
	this->bind();
	glBufferSubData(GL_UNIFORM_BUFFER, 0, this->dataSize, _data);	
}

void UBO::setSubData(const void* data, GLintptr offset, GLsizeiptr size)
{
	this->bind();
	glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
}

void UBO::bind()
{
	glBindBuffer(GL_UNIFORM_BUFFER, this->uboID);
}

GLuint UBO::getID()
{
	return this->uboID;
}

GLuint UBO::getBindingPoint()
{
	return bindingPoint;
}

UBO::~UBO()
{
	glDeleteBuffers(1, &this->uboID);
}