#include "gfx.h"
#include <GL/glu.h>

void glCatchError(const char* file, int line)
{
	GLenum err;
	do 
	{
		err = glGetError();
		if (err != GL_NO_ERROR) 
			printf("GL error: %s:%d: %s\n", file, line, gluErrorString(err));			
		
	}while(err  != GL_NO_ERROR);

}