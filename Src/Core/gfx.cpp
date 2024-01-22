#include <iostream>
#include "gfx.h"

void GLAPIENTRY openGLErrorMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
	{
		// print message with the right string for type, severity and source
		std::string _source;
		std::string _type;
		std::string _severity;
		switch (source)
		{
		case GL_DEBUG_SOURCE_API:
			_source = "API";
			break;
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
			_source = "WINDOW_SYSTEM";
			break;
		case GL_DEBUG_SOURCE_SHADER_COMPILER:
			_source = "SHADER_COMPILER";
			break;
		case GL_DEBUG_SOURCE_THIRD_PARTY:
			_source = "THIRD_PARTY";
			break;
		case GL_DEBUG_SOURCE_APPLICATION:
			_source = "APPLICATION";
			break;
		case GL_DEBUG_SOURCE_OTHER:
			_source = "OTHER";
			break;
		}


		switch (type)
		{
		case GL_DEBUG_TYPE_ERROR:
			_type = "ERROR";
			break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
			_type = "DEPRECATED_BEHAVIOR";
			break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
			_type = "UNDEFINED_BEHAVIOR";
			break;
		case GL_DEBUG_TYPE_PORTABILITY:
			_type = "PORTABILITY";
			break;
		case GL_DEBUG_TYPE_PERFORMANCE:
			_type = "PERFORMANCE";
			break;
		case GL_DEBUG_TYPE_MARKER:
			_type = "MARKER";
			break;
		case GL_DEBUG_TYPE_PUSH_GROUP:
			_type = "PUSH_GROUP";
			break;
		case GL_DEBUG_TYPE_POP_GROUP:
			_type = "POP_GROUP";
			break;
		case GL_DEBUG_TYPE_OTHER:
			_type = "OTHER";
			break;
		}

		switch (severity)
		{
		case GL_DEBUG_SEVERITY_HIGH:
			_severity = "HIGH";
			break;
		case GL_DEBUG_SEVERITY_MEDIUM:
			_severity = "MEDIUM";
			break;
		case GL_DEBUG_SEVERITY_LOW:
			_severity = "LOW";
			break;
		case GL_DEBUG_SEVERITY_NOTIFICATION:
			_severity = "NOTIFICATION";
			return;
		}
		std::cout << "\n[" << _source << " | " << _severity << " | " << _type << "]" << message;
		assert(_severity != "HIGH" || _type != "ERROR");
	}
}