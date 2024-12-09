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
#pragma once

// Libraries
#include <ae/opengl.h>
#include <glm/mat4x4.hpp>
#include <SDL_video.h>
#include <string>

struct SDL_Cursor;

namespace ae {

// Forward Declarations
class _Element;
class _Mesh;
class _Program;
class _Texture;
class _TextureArray;
struct _Bounds;

enum VertexBufferType {
	VBO_NONE,
	VBO_LINE,
	VBO_CIRCLE,
	VBO_QUAD,
	VBO_RECT,
	VBO_SPRITE,
	VBO_ATLAS,
	VBO_QUAD_UV,
	VBO_CUBE,
	VBO_COUNT
};

struct _WindowSettings {
	std::string WindowTitle;
	std::string IconPath;
	glm::ivec2 Size{0};
	glm::ivec2 Position{0};
	int MSAA{0};
	bool Fullscreen{false};
	bool Vsync{false};
};

enum CursorType {
	CURSOR_NONE,
	CURSOR_MAIN,
	CURSOR_CROSS,
	CURSOR_COUNT,
};

// Classes
class _Graphics {

	public:

		void Init(const _WindowSettings &WindowSettings);
		void Close();

		GLuint CreateVBO(float *Vertices, GLsizeiptr Size, GLenum Type);
		void DeleteVBO(GLuint BufferID);

		void ResetState();
		void CheckError();
		void Setup2D();
		void Setup3D();

		void FadeScreen(const _Program *Program, float Amount);
		void ClearScreen();
		void Flip(double FrameTime);

		void SetViewport(const glm::ivec2 &Size);
		void SetWindowSize(const glm::ivec2 &Size);
		bool SetFullscreen(bool Fullscreen);
		void SetStaticUniforms();
		bool SetVsync(bool Vsync);
		bool GetVsync();
		void SetCursor(int Type);
		void SetCursor(SDL_Cursor *Cursor);
		void SetVBO(GLuint Type);
		void SetAttribLevel(GLuint AttribLevel);
		void SetColor(const glm::vec4 &Color);
		void SetClearColor(const glm::vec4 &Color);
		void SetTextureID(GLuint TextureID, GLenum Type=GL_TEXTURE_2D);
		void SetVertexBufferID(GLuint VertexBufferID);
		void SetProgram(const _Program *Program);
		void SetDepthTest(bool DepthTest);
		void SetCullFace(bool Value);
		void SetScissor(const _Bounds &Bounds);
		void SetDepthMask(bool Value);
		void SetActiveTexture(int Unit);
		void EnableStencilTest();
		void DisableStencilTest();
		void EnableScissorTest();
		void DisableScissorTest();
		void EnableParticleBlending();
		void DisableParticleBlending();

		void DrawLine(const glm::vec2 &Start, const glm::vec2 &End);
		void DrawRectangle(const _Bounds &Bounds, bool Filled=false);
		void DrawRectangle(const glm::vec2 &Start, const glm::vec2 &End, bool Filled=false);
		void DrawRectangle3D(const glm::vec2 &Start, const glm::vec2 &End, bool Filled);
		void DrawCircle(const glm::vec3 &Position, float Radius);
		void DrawMask(const _Bounds &Bounds);

		void DrawImage(const _Bounds &Bounds, const _Texture *Texture, bool Stretch=true);
		void DrawRotatedImage(const _Bounds &Bounds, const _Texture *Texture, float Rotation=0.0f, bool Stretch=true);
		void DrawScaledImage(const glm::vec2 &Position, const _Texture *Texture, const glm::vec2 &Size, const glm::vec4 &Color=glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
		void DrawSprite(const glm::vec3 &Position, const _Texture *Texture, float Rotation=0.0f, const glm::vec2 &Scale=glm::vec2(1.0f));
		void DrawAnimationFrame(const glm::vec3 &Position, const _Texture *Texture, const glm::vec4 &TextureCoords, float Rotation=0.0f, const glm::vec2 &Scale=glm::vec2(1.0f));
		void DrawAtlasTexture(const _Bounds &Bounds, const _Texture *Texture, const glm::vec4 &TextureCoords);
		void DrawTextureArray(const _Bounds &Bounds, const _TextureArray *Texture, uint32_t Index);
		void DrawRepeatable(const glm::vec3 &Start, const glm::vec3 &End, const _Texture *Texture, float Rotation, float ScaleX);
		void DrawWall(const glm::vec3 &Position, const glm::vec3 &Scale, const _Texture *Texture, int Side);
		void DrawCube(const glm::vec3 &Start, const glm::vec3 &Scale, const _Texture *Texture);
		void DrawWallDecal(const glm::vec3 &Position, const ae::_Texture *Texture, int Side=0, const glm::vec2 &Scale=glm::vec2(1.0f));
		void DrawMesh(const glm::vec3 &Position, const ae::_Mesh *Mesh, const _Texture *Texture, float Rotation=0.0f, const glm::vec3 &Scale=glm::vec3(1.0f));

		// UI
		_Element *Element{nullptr};

		// Window
		glm::ivec2 WindowSize{0};
		glm::ivec2 FullscreenSize{0};
		glm::ivec2 CurrentSize{0};
		glm::ivec2 ViewportSize{0};

		// Graphics
		glm::mat4 Ortho;
		GLfloat MaxAnisotropy{0};
		float AspectRatio{1};
		GLint MaxSamples{0};
		int FramesPerSecond{0};
		int CircleVertices{32};

	private:

		void BuildVertexBuffers();
		void SetupOpenGL();

		// Vertex buffers
		GLuint VertexArrayID{0};
		GLuint VertexBuffer[VBO_COUNT];

		// Data structures
		SDL_Window *Window{nullptr};
		SDL_GLContext Context{nullptr};
		SDL_Cursor *Cursors[CURSOR_COUNT];

		// State changes
		GLuint LastVertexBufferID{(GLuint)-1};
		GLuint LastTextureID{(GLuint)-1};
		GLuint LastAttribLevel{(GLuint)-1};
		const _Program *LastProgram{nullptr};

		// Benchmarking
		double FrameRateTimer{0.0};
		int FrameCount{0};
};

extern _Graphics Graphics;

}
