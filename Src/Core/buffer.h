#pragma once

#include <glad/glad.h>
#include <string>

class Buffer
{
	public:
		Buffer(GLenum _bufferType,GLenum _usage = GL_DYNAMIC_DRAW, bool _isIndexedBuffer = false);
		void initBuffer(GLuint _bindingPoint, GLsizeiptr dataSize);
		void initBuffer(GLsizeiptr dataSize, const void* data);
		void setData(const void* data);
		void setSubData(const void* data, GLintptr offset, GLsizeiptr size);
		void bind();
		GLuint getID();
		GLuint getBindingPoint();
		std::string getBufferTypeString();
		~Buffer();

	public:
		GLenum bufferType = GL_INVALID_ENUM;
		GLenum usage = GL_INVALID_ENUM;
		bool isIndexedBuffer = false;

	private:
		GLuint bufferID = GL_INVALID_INDEX, bindingPoint = GL_INVALID_INDEX;
		GLsizeiptr dataSize = 0;

};

