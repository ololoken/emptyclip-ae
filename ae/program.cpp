/******************************************************************************
* Copyright (c) 2024 Alan Witkowski
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
#include <ae/light.h>
#include <ae/util.h>
#include <glm/gtc/type_ptr.hpp>

namespace ae {

// Load a program from two shaders
_Program::_Program(const std::string &Name, const _Shader *VertexShader, const _Shader *FragmentShader, GLuint Attribs, size_t MaxLights) :
	Name(Name),
	Attribs(Attribs),
	MaxLights(MaxLights) {

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
		glGetProgramInfoLog(ID, ResultLength, nullptr, (GLchar *)&ErrorMessage[0]);

		throw std::runtime_error(std::string(__func__) + " " + ErrorMessage);
	}

	// Setup lights
	if(MaxLights)
		Lights = new _Light[MaxLights]();

	// Get uniforms
	for(int i = 0; i < SAMPLER_COUNT; i++)
		SamplerIDs[i] = glGetUniformLocation(ID, std::string("sampler" + std::to_string(i)).c_str());

	ViewProjectionTransformID = glGetUniformLocation(ID, "view_projection_transform");
	ModelTransformID = glGetUniformLocation(ID, "model_transform");
	NormalTransformID = glGetUniformLocation(ID, "normal_transform");
	TextureTransformID = glGetUniformLocation(ID, "texture_transform");
	ColorID = glGetUniformLocation(ID, "color");
	AmbientLightID = glGetUniformLocation(ID, "ambient_light");
	FogColorID = glGetUniformLocation(ID, "fog_color");
	LightCountID = glGetUniformLocation(ID, "light_count");

	for(size_t i = 0; i < MaxLights; i++) {
		std::string LightColorName = "lights[" + std::to_string(i) + "].color";
		std::string LightPositionName = "lights[" + std::to_string(i) + "].position";
		std::string LightAttenuationName = "lights[" + std::to_string(i) + "].attenuation";
		Lights[i].ColorID = glGetUniformLocation(ID, LightColorName.c_str());
		Lights[i].PositionID = glGetUniformLocation(ID, LightPositionName.c_str());
		Lights[i].AttenuationID = glGetUniformLocation(ID, LightAttenuationName.c_str());
	}

	// Initialize
	Use();
	ResetTransform(TextureTransformID);
	ResetTransform(NormalTransformID);
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
	for(int i = 0; i < SAMPLER_COUNT; i++) {
		if(SamplerIDs[i] != -1)
			glUniform1i(SamplerIDs[i], i);
	}

	if(AmbientLightID != -1)
		glUniform4fv(AmbientLightID, 1, &AmbientLight[0]);

	if(FogColorID != -1)
		glUniform4fv(FogColorID, 1, &FogColor[0]);

	if(LightCountID != -1)
		glUniform1i(LightCountID, LightCount);

	for(int i = 0; i < LightCount; i++) {
		glUniform4fv(Lights[i].ColorID, 1, &Lights[i].Color[0]);
		glUniform3fv(Lights[i].PositionID, 1, &Lights[i].Position[0]);
		glUniform3fv(Lights[i].AttenuationID, 1, &Lights[i].Attenuation[0]);
	}
}

// Set the value of a float uniform
void _Program::SetUniformFloat(const std::string &Name, float Value) const {
	glUniform1f(glGetUniformLocation(ID, Name.c_str()), Value);
}

// Set the value of a vec2 uniform
void _Program::SetUniformVec2(const std::string &Name, const glm::vec2 &Value) const {
	glUniform2fv(glGetUniformLocation(ID, Name.c_str()), 1, &Value[0]);
}

// Set the value of a vec4 uniform
void _Program::SetUniformVec4(const std::string &Name, const glm::vec4 &Value) const {
	glUniform4fv(glGetUniformLocation(ID, Name.c_str()), 1, &Value[0]);
}

// Set the value of a mat4 uniform
void _Program::SetUniformMat4(const std::string &Name, const glm::mat4 &Value) const {
	glUniformMatrix4fv(glGetUniformLocation(ID, Name.c_str()), 1, GL_FALSE, glm::value_ptr(Value[0]));
}

// Reset texture transform
void _Program::ResetTextureTransform() const {
	if(TextureTransformID)
		glUniformMatrix4fv(TextureTransformID, 1, GL_FALSE, glm::value_ptr(glm::mat4(1)));
}

// Reset transform by id
void _Program::ResetTransform(GLint TransformID) const {
	if(TransformID)
		glUniformMatrix4fv(TransformID, 1, GL_FALSE, glm::value_ptr(glm::mat4(1)));
}

// Loads a shader
_Shader::_Shader(const std::string &Path, GLenum ProgramType) {

	// Load program from file
	const char *ShaderSource = LoadFileIntoMemory(Path.c_str());
	if(!ShaderSource)
		throw std::runtime_error(std::string(__func__) + " error opening '" + Path + "'");

	// Create the shader
	ID = glCreateShader(ProgramType);

	// Compile shader
	glShaderSource(ID, 1, &ShaderSource, nullptr);
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
		glGetShaderInfoLog(ID, ResultLength, nullptr, (GLchar *)&ErrorMessage[0]);

		throw std::runtime_error(std::string(__func__) + " error in " + Path + '\n' + ErrorMessage);
	}
}

// Destructor
_Shader::~_Shader() {
	glDeleteShader(ID);
}

}
