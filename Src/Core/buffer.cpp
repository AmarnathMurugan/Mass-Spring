#include "buffer.h"
#include <iostream>
#include <cassert>

Buffer::Buffer(GLenum _bufferType, GLenum _usage, bool _isIndexedBuffer) : bufferType(_bufferType), usage(_usage), isIndexedBuffer(_isIndexedBuffer)
{
	glGenBuffers(1, &this->bufferID);
	std::cout << this->getBufferTypeString() << " created with ID: " << this->bufferID << "\n";
}

void Buffer::initBuffer(GLuint _bindingPoint, GLsizeiptr _dataSize)
{
	this->bind();
	glBufferData(this->bufferType, _dataSize, nullptr, GL_DYNAMIC_DRAW);
	glBindBufferBase(this->bufferType, _bindingPoint, this->bufferID);
	this->bindingPoint = _bindingPoint;
	this->dataSize = _dataSize;	
}

void Buffer::initBuffer(GLsizeiptr dataSize, const void* data)
{
	this->bind();
	glBufferData(this->bufferType, dataSize, data, this->usage);
	this->dataSize = dataSize;
}

void Buffer::setData(const void* _data)
{	
	this->bind();
	glBufferSubData(this->bufferType, 0, this->dataSize, _data);	
}

void Buffer::setSubData(const void* data, GLintptr offset, GLsizeiptr size)
{
	this->bind();
	glBufferSubData(this->bufferType, offset, size, data);
}

void Buffer::bind()
{
	assert(this->bufferID != GL_INVALID_INDEX);
	glBindBuffer(this->bufferType, this->bufferID);
}

GLuint Buffer::getID()
{
	return this->bufferID;
}

GLuint Buffer::getBindingPoint()
{
	return bindingPoint;
}

std::string Buffer::getBufferTypeString()
{
	switch (this->bufferType)
	{
		case GL_UNIFORM_BUFFER:
			return "Uniform buffer";
		case GL_SHADER_STORAGE_BUFFER:
			return "Shader storage buffer";
		default:
			return "Unknown buffer type";
	}
}

Buffer::~Buffer()
{
	glDeleteBuffers(1, &this->bufferID);
}