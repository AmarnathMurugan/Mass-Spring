#include <iostream>
#include "gfx.h"

void glCatchError(const char* file, int line)
{
	GLenum err;
	std::string error;
	do 
	{
		err = glGetError();
        switch (err)
        {
            case GL_INVALID_ENUM:
                error = "GL_INVALID_ENUM";
                break;
            case GL_INVALID_VALUE:
                error = "GL_INVALID_VALUE";
                break;
            case GL_INVALID_OPERATION:
                error = "GL_INVALID_OPERATION";
                break;
            case GL_STACK_OVERFLOW:
                error = "GL_STACK_OVERFLOW";
                break;
            case GL_STACK_UNDERFLOW:
                error = "GL_STACK_UNDERFLOW";
                break;
            case GL_OUT_OF_MEMORY:
                error = "GL_OUT_OF_MEMORY";
                break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                error = "GL_INVALID_FRAMEBUFFER_OPERATION";
                break;
            default:
                error = "unknown error";
                break;
        }
		if (err != GL_NO_ERROR) 
			std::cout<<"GL error: " << file << line << error;			
		
	}while(err  != GL_NO_ERROR);

}