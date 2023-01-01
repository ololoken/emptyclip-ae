/******************************************************************************
* Copyright (c) 2023 Alan Witkowski
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

	static const int SAMPLER_COUNT = 16;

	public:

		_Program(const std::string &Name, const _Shader *VertexShader, const _Shader *FragmentShader, GLuint Attribs, size_t MaxLights);
		~_Program();

		void Use() const;
		void SetUniformFloat(const std::string &Name, float Value) const;
		void SetUniformVec2(const std::string &Name, const glm::vec2 &Value) const;
		void SetUniformVec4(const std::string &Name, const glm::vec4 &Value) const;
		void SetUniformMat4(const std::string &Name, const glm::mat4 &Value) const;
		void ResetTextureTransform() const;
		void ResetTransform(GLint TransformID) const;

		std::string Name;

		GLuint ID;
		GLint ViewProjectionTransformID{-1};
		GLint ModelTransformID{-1};
		GLint NormalTransformID{-1};
		GLint TextureTransformID{-1};
		GLint ColorID{-1};
		GLint AmbientLightID{-1};
		GLint LightCountID{-1};
		GLuint Attribs{0};

		size_t MaxLights{0};
		int LightCount{0};
		_Light *Lights{nullptr};
		glm::vec4 AmbientLight{1.0f};

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
