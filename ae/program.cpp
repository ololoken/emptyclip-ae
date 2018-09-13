/******************************************************************************
* Copyright (c) 2017 Alan Witkowski
*
* This software is provided 'as-is', without any express or implied
* warranty. In no event will the authors be held liable for any damages
* arising from the use of this software.
*
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
*
* 1. The origin of this software must not be misrepresented; you must not
*    claim that you wrote the original software. If you use this software
*    in a product, an acknowledgment in the product documentation would be
*    appreciated but is not required.
* 2. Altered source versions must be plainly marked as such, and must not be
*    misrepresented as being the original software.
* 3. This notice may not be removed or altered from any source distribution.
*******************************************************************************/
#include <ae/program.h>
#include <ae/graphics.h>
#include <ae/light.h>
#include <ae/util.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stdexcept>

namespace ae {

// Load a program from two shaders
_Program::_Program(const std::string &Name, const _Shader *VertexShader, const _Shader *FragmentShader, int Attribs, int MaxLights) :
	Name(Name),
	ViewProjectionTransformID(-1),
	ModelTransformID(-1),
	AmbientLightID(-1),
	LightCountID(-1),
	Attribs(Attribs),
	MaxLights(MaxLights),
	LightCount(0),
	Lights(nullptr),
	AmbientLight(1.0f) {

	// Create program
	ID = glCreateProgram();
	glAttachShader(ID, VertexShader->ID);
	glAttachShader(ID, FragmentShader->ID);
	glLinkProgram(ID);

	// Check the program
	GLint Result = GL_FALSE;
	glGetProgramiv(ID, GL_LINK_STATUS, &Result);
	if(!Result) {

		// Get error message length
		GLint ResultLength;
		glGetProgramiv(ID, GL_INFO_LOG_LENGTH, &ResultLength);

		// Get message
		std::string ErrorMessage((size_t)ResultLength, 0);
		glGetProgramInfoLog(ID, ResultLength, NULL, (GLchar *)ErrorMessage.data());

		throw std::runtime_error(ErrorMessage);
	}

	// Setup lights
	if(MaxLights)
		Lights = new _Light[MaxLights]();

	// Get uniforms
	SamplerIDs[0] = glGetUniformLocation(ID, "sampler0");
	glBindAttribLocation(ID, 0, "vertex_pos");
	glBindAttribLocation(ID, 1, "vertex_uv");
	glBindAttribLocation(ID, 2, "vertex_norm");

	ViewProjectionTransformID = glGetUniformLocation(ID, "view_projection_transform");
	ModelTransformID = glGetUniformLocation(ID, "model_transform");
	AmbientLightID = glGetUniformLocation(ID, "ambient_light");
	LightCountID = glGetUniformLocation(ID, "light_count");

	for(int i = 0; i < MaxLights; i++) {
		std::string LightPositionName = "lights[" + std::to_string(i) + "].position";
		std::string LightColorName = "lights[" + std::to_string(i) + "].color";
		std::string LightRadiusName = "lights[" + std::to_string(i) + "].radius";
		Lights[i].PositionID = glGetUniformLocation(ID, LightPositionName.c_str());
		Lights[i].ColorID = glGetUniformLocation(ID, LightColorName.c_str());
		Lights[i].RadiusID = glGetUniformLocation(ID, LightRadiusName.c_str());
	}
}

// Destructor
_Program::~_Program() {
	delete[] Lights;
	glDeleteProgram(ID);
}

// Enable the program
void _Program::Use() const {
	glUseProgram(ID);

	// Set uniforms
	if(SamplerIDs[0] != -1)
		glUniform1i(SamplerIDs[0], 0);

	if(AmbientLightID != -1)
		glUniform4fv(AmbientLightID, 1, &AmbientLight[0]);

	if(LightCountID != -1)
		glUniform1i(LightCountID, LightCount);

	for(int i = 0; i < LightCount; i++) {
		glUniform3fv(Lights[i].PositionID, 1, &Lights[i].Position[0]);
		glUniform4fv(Lights[i].ColorID, 1, &Lights[i].Color[0]);
		glUniform1fv(Lights[i].RadiusID, 1, &Lights[i].Radius);
	}
}

// Loads a shader
_Shader::_Shader(const std::string &Path, GLenum ProgramType) {

	// Load program from file
	const char *ShaderSource = LoadFileIntoMemory(Path.c_str());
	if(!ShaderSource)
		throw std::runtime_error("Failed to load shader file: " + Path);

	// Create the shader
	ID = glCreateShader(ProgramType);

	// Compile shader
	glShaderSource(ID, 1, &ShaderSource, NULL);
	delete[] ShaderSource;

	glCompileShader(ID);

	// Check for errors
	GLint Result = GL_FALSE;
	glGetShaderiv(ID, GL_COMPILE_STATUS, &Result);
	if(!Result) {

		// Get error message length
		GLint ResultLength;
		glGetShaderiv(ID, GL_INFO_LOG_LENGTH, &ResultLength);

		// Get message
		std::string ErrorMessage((size_t)ResultLength, 0);
		glGetShaderInfoLog(ID, ResultLength, NULL, (GLchar *)ErrorMessage.data());

		throw std::runtime_error("Error in " + Path + '\n' + ErrorMessage);
	}
}

// Destructor
_Shader::~_Shader() {
	glDeleteShader(ID);
}

}
