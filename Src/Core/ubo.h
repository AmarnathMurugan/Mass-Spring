#pragma once

#include <glad/glad.h>

class UBO
{
	public:
		UBO();
		void initUBO(GLuint _bindingPoint, GLsizeiptr dataSize);
		void setData(const void* data);
		void setSubData(const void* data, GLintptr offset, GLsizeiptr size);
		void bind();
		GLuint getID();
		GLuint getBindingPoint();
		~UBO();

	private:
		GLuint uboID = GL_INVALID_INDEX, bindingPoint = GL_INVALID_INDEX;
		GLsizeiptr dataSize = 0;

};

