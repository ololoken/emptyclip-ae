/******************************************************************************
* Copyright (c) 2019 Alan Witkowski
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
#pragma once

#include <ae/opengl.h>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <string>

namespace ae {

class _Shader;
struct _Light;

// Program
class _Program {

	static const int SAMPLER_COUNT = 4;

	public:

		_Program(const std::string &Name, const _Shader *VertexShader, const _Shader *FragmentShader, GLuint Attribs, int MaxLights);
		~_Program();

		void Use() const;
		void SetUniformFloat(const std::string &Name, float Value) const;
		void SetUniformVec2(const std::string &Name, const glm::vec2 &Value) const;
		void SetUniformVec4(const std::string &Name, const glm::vec4 &Value) const;
		void SetUniformMat4(const std::string &Name, const glm::mat4 &Value) const;

		std::string Name;

		GLuint ID;
		GLint ViewProjectionTransformID;
		GLint ModelTransformID;
		GLint TextureTransformID;
		GLint ColorID;
		GLint AmbientLightID;
		GLint LightCountID;
		GLuint Attribs;

		int MaxLights;
		int LightCount;
		_Light *Lights;
		glm::vec4 AmbientLight;

	private:

		GLint SamplerIDs[SAMPLER_COUNT];

};

// Shader
class _Shader {

	public:

		_Shader(const std::string &Path, GLenum ProgramType);
		~_Shader();

		GLuint ID;

	private:

};

}
