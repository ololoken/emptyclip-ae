/******************************************************************************
* Copyright (c) 2025 Alan Witkowski
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
#include <ae/graphics.h>
#include <ae/assets.h>
#include <ae/mesh.h>
#include <ae/program.h>
#include <ae/texture_array.h>
#include <ae/texture.h>
#include <ae/bounds.h>
#include <ae/ui.h>
#include <SDL_image.h>
#include <glm/gtc/type_ptr.hpp>
#include <stdexcept>

namespace ae {

_Graphics Graphics;

// Initialize
void _Graphics::Init(const _WindowSettings &WindowSettings) {

	// Set sizes
	SDL_DisplayMode DisplayMode;
	WindowSize = WindowSettings.Size;
	if(SDL_GetDesktopDisplayMode(0, &DisplayMode) == 0)
		FullscreenSize = glm::ivec2(DisplayMode.w, DisplayMode.h);

	// Save position
	WindowPosition = WindowSettings.Position;

	// Set video flags
	Uint32 VideoFlags = SDL_WINDOW_OPENGL;
	if(WindowSettings.Fullscreen) {
		VideoFlags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
		CurrentSize = FullscreenSize;
	}
	else
		CurrentSize = WindowSize;

	// Set opengl attributes
#ifndef __EMSCRIPTEN__
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 1);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	if(WindowSettings.MSAA > 0) {
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, WindowSettings.MSAA);
	}
#endif
	// Load cursors
	Cursors[CURSOR_NONE] = nullptr;
	Cursors[CURSOR_MAIN] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
	Cursors[CURSOR_CROSS] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_CROSSHAIR);
	SDL_SetCursor(Cursors[CURSOR_MAIN]);

	// Create window
	Window = SDL_CreateWindow(WindowSettings.WindowTitle.c_str(), WindowSettings.Position.x, WindowSettings.Position.y, CurrentSize.x, CurrentSize.y, VideoFlags);
	if(Window == nullptr)
		throw std::runtime_error(std::string(__func__) + " SDL_CreateWindow failed");

	// Attach icon
	if(WindowSettings.IconPath.size()) {
		SDL_Surface *IconSurface = IMG_Load(WindowSettings.IconPath.c_str());
		SDL_SetWindowIcon(Window, IconSurface);
		SDL_FreeSurface(IconSurface);
	}

	// Set up opengl context
	Context = SDL_GL_CreateContext(Window);
	if(Context == nullptr)
		throw std::runtime_error(std::string(__func__) + " SDL_GL_CreateContext failed");

	InitGLFunctions();

	int MajorVersion, MinorVersion;
	SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &MajorVersion);
	SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &MinorVersion);

	// Set vsync
	SDL_GL_SetSwapInterval(WindowSettings.Vsync);

	// Set up OpenGL
	SetupOpenGL();

	// Setup viewport
	SetViewport(CurrentSize);
}

// Sets up OpenGL
void _Graphics::SetupOpenGL() {

	// Anisotropic filtering
	if(SDL_GL_ExtensionSupported("GL_EXT_texture_filter_anisotropic"))
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &MaxAnisotropy);

	// Get MSAA limit
	glGetIntegerv(GL_MAX_SAMPLES, &MaxSamples);

	// Default state
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Create vertex array
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);
	glEnableVertexAttribArray(0);

	// Set ortho matrix
	Ortho = glm::ortho(0.0f, (float)CurrentSize.x, (float)CurrentSize.y, 0.0f, -1.0f, 1.0f);

	// Build vertex buffers
	BuildVertexBuffers();

	// Reset internal state
	ResetState();

	// Clear screen
	ClearScreen();
	Flip(0);
}

// Shutdown
void _Graphics::Close() {
	delete Element;
	Element = nullptr;

	// Close opengl context
	if(Context) {
		for(int i = 1; i < VBO_COUNT; i++)
			glDeleteBuffers(1, &VertexBuffer[i]);

		glDeleteVertexArrays(1, &VertexArrayID);

		SDL_GL_DeleteContext(Context);
		Context = nullptr;
	}

	// Close cursors
	for(int i = 1; i < CURSOR_COUNT; i++)
		SDL_FreeCursor(Cursors[i]);

	// Close window
	if(Window) {
		SDL_DestroyWindow(Window);
		Window = nullptr;
	}
}

// Builds the vertex buffer objects
void _Graphics::BuildVertexBuffers() {
	VertexBuffer[VBO_NONE] = 0;

	// Line
	{
		float Vertices[] = {
			0.0f, 0.0f,
			1.0f, 1.0f,
		};

		VertexBuffer[VBO_LINE] = CreateVBO(Vertices, sizeof(Vertices), GL_STATIC_DRAW);
	}

	// Circle
	{
		float *Vertices = new float[CircleVertices * 2];

		// Get vertices
		for(int i = 0; i < CircleVertices; i++) {
			float Radians = ((float)i / CircleVertices) * (glm::pi<float>() * 2.0f);
			Vertices[i * 2] = std::cos(Radians);
			Vertices[i * 2 + 1] = std::sin(Radians);
		}

		VertexBuffer[VBO_CIRCLE] = CreateVBO(Vertices, sizeof(float) * (unsigned)CircleVertices * 2, GL_STATIC_DRAW);
		delete[] Vertices;
	}

	// Quad
	{
		float Vertices[] = {
			1.0f, 0.0f,
			0.0f, 0.0f,
			1.0f, 1.0f,
			0.0f, 1.0f,
		};

		VertexBuffer[VBO_QUAD] = CreateVBO(Vertices, sizeof(Vertices), GL_STATIC_DRAW);
	}

	// Rectangle
	{
		float Vertices[] = {
			0.0f, 0.0f,
			1.0f, 0.0f,
			1.0f, 1.0f,
			0.0f, 1.0f,
		};

		VertexBuffer[VBO_RECT] = CreateVBO(Vertices, sizeof(Vertices), GL_STATIC_DRAW);
	}

	// Centered textured quad
	{
		float Vertices[] = {
			-0.5f,  0.5f,
			 0.5f,  0.5f,
			-0.5f, -0.5f,
			 0.5f, -0.5f,
			 0.0f,  1.0f,
			 1.0f,  1.0f,
			 0.0f,  0.0f,
			 1.0f,  0.0f,
		};

		VertexBuffer[VBO_SPRITE] = CreateVBO(Vertices, sizeof(Vertices), GL_STATIC_DRAW);
	}

	// Dynamic vbo for drawing animations
	{
		float Vertices[] = {
			-0.5f,  0.5f,
			 0.5f,  0.5f,
			-0.5f, -0.5f,
			 0.5f, -0.5f,
			 0.0f,  1.0f,
			 1.0f,  1.0f,
			 0.0f,  0.0f,
			 1.0f,  0.0f,
		};

		VertexBuffer[VBO_ATLAS] = CreateVBO(Vertices, sizeof(Vertices), GL_DYNAMIC_DRAW);
	}

	// Textured quad
	{
		float Vertices[] = {
			1.0f, 0.0f,
			0.0f, 0.0f,
			1.0f, 1.0f,
			0.0f, 1.0f,
			1.0f, 0.0f,
			0.0f, 0.0f,
			1.0f, 1.0f,
			0.0f, 1.0f,
		};

		VertexBuffer[VBO_QUAD_UV] = CreateVBO(Vertices, sizeof(Vertices), GL_STATIC_DRAW);
	}

	// Cube
	{
		float Vertices[] = {

			// Top
			1.0f,  0.0f,  1.0f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
			0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,  1.0f,
			1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f,
			0.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,

			// Front
			1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
			0.0f,  1.0f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,  0.0f,
			1.0f,  1.0f,  0.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f,
			0.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,

			// Left
			0.0f,  1.0f,  1.0f,  1.0f,  0.0f, -1.0f,  0.0f,  0.0f,
			0.0f,  0.0f,  1.0f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
			0.0f,  1.0f,  0.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f,
			0.0f,  0.0f,  0.0f,  0.0f,  1.0f, -1.0f,  0.0f,  0.0f,

			// Back
			0.0f,  0.0f,  1.0f,  1.0f,  0.0f,  0.0f, -1.0f,  0.0f,
			1.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f, -1.0f,  0.0f,
			0.0f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f, -1.0f,  0.0f,
			1.0f,  0.0f,  0.0f,  0.0f,  1.0f,  0.0f, -1.0f,  0.0f,

			// Right
			1.0f,  0.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f,  0.0f,
			1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
			1.0f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f,
			1.0f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,  0.0f,
		};

		VertexBuffer[VBO_CUBE] = CreateVBO(Vertices, sizeof(Vertices), GL_STATIC_DRAW);
	}
}

// Create vertex buffer and return id
GLuint _Graphics::CreateVBO(float *Vertices, GLsizeiptr Size, GLenum Type) {

	// Create buffer
	GLuint BufferID;
	glGenBuffers(1, &BufferID);
	glBindBuffer(GL_ARRAY_BUFFER, BufferID);
	glBufferData(GL_ARRAY_BUFFER, Size, Vertices, Type);

	return BufferID;
}

// Delete vertex buffer
void _Graphics::DeleteVBO(GLuint BufferID) {
	glDeleteBuffers(1, &BufferID);
}

// Resets all the last used variables
void _Graphics::ResetState() {
	SetAttribLevel(0);
	glUseProgram(0);
	glActiveTexture(GL_TEXTURE0);
	LastVertexBufferID = (GLuint)-1;
	LastTextureID = (GLuint)-1;
	LastAttribLevel = (GLuint)-1;
	LastProgram = nullptr;
}

// Throw opengl error
void _Graphics::CheckError() {
	GLenum Error = glGetError();
	if(Error)
		throw std::runtime_error(std::string(__func__) + " glGetError returned " + std::to_string(Error));
}

// Sets up the projection matrix for drawing 2D objects
void _Graphics::Setup2D() {
	glViewport(0, 0, CurrentSize.x, CurrentSize.y);
}

// Set up modelview matrix
void _Graphics::Setup3D() {
	glViewport(0, CurrentSize.y - ViewportSize.y, ViewportSize.x, ViewportSize.y);
}

// Fade the screen
void _Graphics::FadeScreen(const _Program *Program, float Amount) {
	Graphics.SetProgram(Program);
	Graphics.SetColor(glm::vec4(0.0f, 0.0f, 0.0f, Amount));
	DrawRectangle(glm::vec2(0, 0), CurrentSize, true);
}

// Clears the screen
void _Graphics::ClearScreen() {
	SetDepthMask(true);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

// Draws the frame
void _Graphics::Flip(double FrameTime) {

	// Swap buffers
	SDL_GL_SwapWindow(Window);

	// Clear screen
	ClearScreen();

	// Update frame counter
	FrameCount++;
	FrameRateTimer += FrameTime;
	if(FrameRateTimer >= 1.0) {
		FramesPerSecond = FrameCount;
		FrameCount = 0;
		FrameRateTimer -= 1.0;
	}

	// Check for errors
	#ifndef NDEBUG
		CheckError();
	#endif
}

// Change the viewport
void _Graphics::SetViewport(const glm::ivec2 &Size) {
	ViewportSize = Size;

	// Calculate aspect ratio
	AspectRatio = (float)ViewportSize.x / ViewportSize.y;
}

// Change window and viewport size
void _Graphics::SetWindowSize(const glm::ivec2 &Size) {

	// Keep viewport difference the same
	glm::ivec2 ViewportDifference = CurrentSize - ViewportSize;

	// Change viewport size
	CurrentSize = Size;
	SetViewport(Size - ViewportDifference);

	// Update shaders
	Ortho = glm::ortho(0.0f, (float)CurrentSize.x, (float)CurrentSize.y, 0.0f, -1.0f, 1.0f);
	SetStaticUniforms();

	// Update UI size
	Element->Size = Size;

	// Update actual window
	SDL_SetWindowSize(Window, Size.x, Size.y);
}

// Toggle fullscreen
bool _Graphics::SetFullscreen(bool Fullscreen) {
	if(FullscreenSize == glm::ivec2(0))
		return false;

	// Save old window position
	if(Fullscreen)
		SDL_GetWindowPosition(Window, &WindowPosition.x, &WindowPosition.y);

	// Toggle fullscreen
	if(SDL_SetWindowFullscreen(Window, Fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0) != 0)
		return false;

	// Set window size
	if(!Fullscreen) {
		Graphics.SetWindowSize(WindowSize);
		SDL_SetWindowPosition(Window, WindowPosition.x, WindowPosition.y);
	}
	else
		Graphics.SetWindowSize(FullscreenSize);

	return true;
}

// Assign uniform values in program
void _Graphics::SetStaticUniforms() {
	SetProgram(Assets.Programs["ortho_pos"]);
	glUniformMatrix4fv(Assets.Programs["ortho_pos"]->ViewProjectionTransformID, 1, GL_FALSE, glm::value_ptr(Ortho));
	SetProgram(Assets.Programs["ortho_pos_uv"]);
	glUniformMatrix4fv(Assets.Programs["ortho_pos_uv"]->ViewProjectionTransformID, 1, GL_FALSE, glm::value_ptr(Ortho));
	SetProgram(Assets.Programs["text"]);
	glUniformMatrix4fv(Assets.Programs["text"]->ViewProjectionTransformID, 1, GL_FALSE, glm::value_ptr(Ortho));
}

// Set Vsync
bool _Graphics::SetVsync(bool Vsync) {
	return SDL_GL_SetSwapInterval(Vsync) == 0;
}

// Get Vsync value
bool _Graphics::GetVsync() {
	return SDL_GL_GetSwapInterval();
}

// Set mouse cursor icon
void _Graphics::SetCursor(int Type) {
	if(Type == CURSOR_NONE) {
		SDL_ShowCursor(false);
	}
	else {
		SDL_ShowCursor(true);
		SDL_SetCursor(Cursors[Type]);
	}
}

// Set custom mouse cursor
void _Graphics::SetCursor(SDL_Cursor *Cursor) {
	SDL_ShowCursor(true);
	SDL_SetCursor(Cursor);
}

// Enable state for VBO
void _Graphics::SetVBO(GLuint VBO) {
	if(LastVertexBufferID == VBO)
		return;

	glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer[VBO]);

	switch(VBO) {
		case VBO_CUBE:
			SetAttribLevel(3);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, nullptr);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (GLvoid *)(sizeof(glm::vec3)));
			glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (GLvoid *)(sizeof(glm::vec3) + sizeof(glm::vec2)));
		break;
		case VBO_SPRITE:
		case VBO_ATLAS:
		case VBO_QUAD_UV:
			SetAttribLevel(2);
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, nullptr);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (GLvoid *)(sizeof(float) * 8));
		break;
		case VBO_LINE:
		case VBO_RECT:
		case VBO_QUAD:
		case VBO_CIRCLE:
			SetAttribLevel(1);
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, nullptr);
		break;
	}

	LastVertexBufferID = VBO;
}

// Enable vertex attrib arrays
void _Graphics::SetAttribLevel(GLuint AttribLevel) {
	if(AttribLevel == LastAttribLevel)
		return;

	if(AttribLevel < LastAttribLevel && LastAttribLevel != (GLuint)-1) {
		for(GLuint i = 1; i < LastAttribLevel; i++)
			glDisableVertexAttribArray(i);
	}

	for(GLuint i = 1; i < AttribLevel; i++)
		glEnableVertexAttribArray(i);

	LastAttribLevel = AttribLevel;
}

// Set opengl color
void _Graphics::SetColor(const glm::vec4 &Color) {
	glUniform4fv(LastProgram->ColorID, 1, &Color[0]);
}

// Set opengl clear color
void _Graphics::SetClearColor(const glm::vec4 &Color) {
	glClearColor(Color.r, Color.g, Color.b, Color.a);
}

// Set texture id
void _Graphics::SetTextureID(GLuint TextureID, GLenum Type) {
	if(TextureID == LastTextureID)
		return;

	glBindTexture(Type, TextureID);

	LastTextureID = TextureID;
}

// Set vertex buffer id
void _Graphics::SetVertexBufferID(GLuint VertexBufferID) {
	if(VertexBufferID == LastVertexBufferID)
		return;

	glBindBuffer(GL_ARRAY_BUFFER, VertexBufferID);
	LastVertexBufferID = VertexBufferID;
}

// Enable a program
void _Graphics::SetProgram(const _Program *Program) {
	if(Program == LastProgram)
		return;

	SetAttribLevel(Program->Attribs);
	Program->Use();
	LastProgram = Program;
}

// Enable/disable depth test
void _Graphics::SetDepthTest(bool DepthTest) {
	if(DepthTest)
		glEnable(GL_DEPTH_TEST);
	else
		glDisable(GL_DEPTH_TEST);
}

// Set cull face
void _Graphics::SetCullFace(bool Value) {
	if(Value)
		glEnable(GL_CULL_FACE);
	else
		glDisable(GL_CULL_FACE);
}

// Set scissor region
void _Graphics::SetScissor(const _Bounds &Bounds) {
	glScissor((GLint)Bounds.Start.x, (GLint)(CurrentSize.y - Bounds.End.y), (GLsizei)(Bounds.End.x - Bounds.Start.x), (GLsizei)(Bounds.End.y - Bounds.Start.y));
}

// Set depth mask
void _Graphics::SetDepthMask(bool Value) {
	glDepthMask(Value);
}

// Set active texture
void _Graphics::SetActiveTexture(int Unit) {
	glActiveTexture(GL_TEXTURE0 + Unit);
}

// Enable stencil test
void _Graphics::EnableStencilTest() {
	glEnable(GL_STENCIL_TEST);
}

// Disable stencil tests
void _Graphics::DisableStencilTest() {
	glDisable(GL_STENCIL_TEST);
}

// Enable scissor test
void _Graphics::EnableScissorTest() {
	glEnable(GL_SCISSOR_TEST);
}

// Disable scissor test
void _Graphics::DisableScissorTest() {
	glDisable(GL_SCISSOR_TEST);
}

// Enable blending mode for particles
void _Graphics::EnableParticleBlending() {
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
}

// Disable blending mode for particles
void _Graphics::DisableParticleBlending() {
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

// Draw line
void _Graphics::DrawLine(const glm::vec2 &Start, const glm::vec2 &End) {
	SetVBO(VBO_LINE);
	glm::vec2 Size = End - Start;

	glm::mat4 Transform(1.0f);
	Transform[3][0] = Start.x;
	Transform[3][1] = Start.y;
	Transform[0][0] = Size.x;
	Transform[1][1] = Size.y;
	glUniformMatrix4fv(LastProgram->ModelTransformID, 1, GL_FALSE, glm::value_ptr(Transform));
	glDrawArrays(GL_LINES, 0, 2);
}

// Draw rectangle in screen space
void _Graphics::DrawRectangle(const _Bounds &Bounds, bool Filled) {
	DrawRectangle(glm::vec2(Bounds.Start.x, Bounds.Start.y), glm::vec2(Bounds.End.x, Bounds.End.y), Filled);
}

// Draw rectangle
void _Graphics::DrawRectangle(const glm::vec2 &Start, const glm::vec2 &End, bool Filled) {

	// Get transform
	glm::mat4 Transform(1.0f);
	if(Filled) {
		SetVBO(VBO_QUAD);
		Transform = glm::translate(Transform, glm::vec3(Start, 0.0f));
		Transform = glm::scale(Transform, glm::vec3(End - Start, 0.0f));
	}
	else {
		SetVBO(VBO_RECT);
		Transform = glm::translate(Transform, glm::vec3(Start.x + 0.5f, Start.y + 0.5f, 0.0f));
		Transform = glm::scale(Transform, glm::vec3(End - Start - glm::vec2(1.0f), 0.0f));
	}

	glUniformMatrix4fv(LastProgram->ModelTransformID, 1, GL_FALSE, glm::value_ptr(Transform));
	if(Filled)
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	else
		glDrawArrays(GL_LINE_LOOP, 0, 4);
}

// Draw rectangle in 3D space
void _Graphics::DrawRectangle3D(const glm::vec2 &Start, const glm::vec2 &End, bool Filled) {

	// Get transform
	glm::mat4 Transform(1.0f);
	if(Filled) {
		SetVBO(VBO_QUAD);
		Transform = glm::translate(Transform, glm::vec3(Start, 0.0f));
		Transform = glm::scale(Transform, glm::vec3(End - Start, 0.0f));
	}
	else {
		SetVBO(VBO_RECT);
		Transform = glm::translate(Transform, glm::vec3(Start.x, Start.y, 0.0f));
		Transform = glm::scale(Transform, glm::vec3(End - Start, 0.0f));
	}

	glUniformMatrix4fv(LastProgram->ModelTransformID, 1, GL_FALSE, glm::value_ptr(Transform));
	if(Filled)
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	else
		glDrawArrays(GL_LINE_LOOP, 0, 4);
}

// Draw circle
void _Graphics::DrawCircle(const glm::vec3 &Position, float Radius) {
	Graphics.SetVBO(VBO_CIRCLE);

	glm::mat4 ModelTransform;
	ModelTransform = glm::translate(glm::mat4(1.0f), Position);
	ModelTransform = glm::scale(ModelTransform, glm::vec3(Radius, Radius, 0.0f));
	glUniformMatrix4fv(LastProgram->ModelTransformID, 1, GL_FALSE, glm::value_ptr(ModelTransform));

	glDrawArrays(GL_LINE_LOOP, 0, (GLsizei)CircleVertices);
}

// Draw stencil mask
void _Graphics::DrawMask(const _Bounds &Bounds) {

	// Enable stencil
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glStencilMask(0x01);

	// Write 1 to stencil buffer
	glStencilFunc(GL_ALWAYS, 0x01, 0x01);
	glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);

	// Draw region
	DrawRectangle(Bounds.Start, Bounds.End, true);

	// Then draw element only where stencil is 1
	glStencilFunc(GL_EQUAL, 0x01, 0x01);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glStencilMask(0x00);
}

// Draw image in screen space
void _Graphics::DrawImage(const _Bounds &Bounds, const _Texture *Texture, bool Stretch) {
	SetVBO(VBO_QUAD_UV);
	SetTextureID(Texture->ID);

	// Get size
	glm::vec2 Size = Bounds.End - Bounds.Start;

	// Model transform
	glm::mat4 Transform(1.0f);
	Transform[3][0] = Bounds.Start.x;
	Transform[3][1] = Bounds.Start.y;
	Transform[0][0] = Size.x;
	Transform[1][1] = Size.y;
	glUniformMatrix4fv(LastProgram->ModelTransformID, 1, GL_FALSE, glm::value_ptr(Transform));

	// Get texture coordinates
	float S = Stretch ? 1.0f : (Bounds.End.x - Bounds.Start.x) / (float)(Texture->Size.x);
	float T = Stretch ? 1.0f : (Bounds.End.y - Bounds.Start.y) / (float)(Texture->Size.y);

	// Texture transform
	glm::mat4 TextureTransform(1.0f);
	TextureTransform[0][0] = S;
	TextureTransform[1][1] = T;
	glUniformMatrix4fv(LastProgram->TextureTransformID, 1, GL_FALSE, glm::value_ptr(TextureTransform));
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

// Draw rotated image in screen space
void _Graphics::DrawRotatedImage(const _Bounds &Bounds, const _Texture *Texture, float Rotation, bool Stretch) {
	SetVBO(VBO_QUAD_UV);
	SetTextureID(Texture->ID);

	// Get size
	glm::vec2 Size = Bounds.End - Bounds.Start;

	// Translate to position
	glm::mat4 ModelTransform = glm::translate(glm::mat4(1.0f), glm::vec3(Bounds.GetCenter(), 0.0f));

	// Rotate
	ModelTransform = glm::rotate(ModelTransform, glm::radians(Rotation), glm::vec3(0, 0, 1));

	// Translate to origin
	ModelTransform = glm::translate(ModelTransform, glm::vec3(-Size * 0.5f, 0.0f));

	// Scale
	ModelTransform = glm::scale(ModelTransform, glm::vec3(Size, 0.0f));

	// Set model transform
	glUniformMatrix4fv(LastProgram->ModelTransformID, 1, GL_FALSE, glm::value_ptr(ModelTransform));

	// Get texture coordinates
	float S = Stretch ? 1.0f : (Bounds.End.x - Bounds.Start.x) / (float)(Texture->Size.x);
	float T = Stretch ? 1.0f : (Bounds.End.y - Bounds.Start.y) / (float)(Texture->Size.y);

	// Set texture transform
	glm::mat4 TextureTransform(1.0f);
	TextureTransform[0][0] = S;
	TextureTransform[1][1] = T;
	glUniformMatrix4fv(LastProgram->TextureTransformID, 1, GL_FALSE, glm::value_ptr(TextureTransform));
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

// Draw image centered given a size
void _Graphics::DrawScaledImage(const glm::vec2 &Position, const _Texture *Texture, const glm::vec2 &Size, const glm::vec4 &Color) {
	Graphics.SetColor(Color);

	// Scale texture by UI scale
	glm::vec2 TextureSize = Size * 0.5f * ae::_Element::GetUIScale();

	// Draw image
	_Bounds Bounds(Position - TextureSize, Position + TextureSize);
	DrawImage(Bounds, Texture, true);
}

// Draw 3d sprite
void _Graphics::DrawSprite(const glm::vec3 &Position, const _Texture *Texture, float Rotation, const glm::vec2 &Scale) {
	SetVBO(VBO_SPRITE);
	SetTextureID(Texture->ID);

	Rotation = glm::radians(Rotation);

	glm::mat4 ModelTransform;
	ModelTransform = glm::translate(glm::mat4(1.0f), Position);
	if(Rotation != 0.0f)
		ModelTransform = glm::rotate(ModelTransform, Rotation, glm::vec3(0, 0, 1));

	ModelTransform = glm::scale(ModelTransform, glm::vec3(Scale, 0.0f));

	glUniformMatrix4fv(LastProgram->ModelTransformID, 1, GL_FALSE, glm::value_ptr(ModelTransform));

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

// Draw frame from an animation
void _Graphics::DrawAnimationFrame(const glm::vec3 &Position, const _Texture *Texture, const glm::vec4 &TextureCoords, float Rotation, const glm::vec2 &Scale) {
	SetVBO(VBO_ATLAS);
	SetTextureID(Texture->ID);

	// Set transform
	glm::mat4 ModelTransform;
	ModelTransform = glm::translate(glm::mat4(1.0f), Position);
	Rotation = glm::radians(Rotation);
	if(Rotation != 0.0f)
		ModelTransform = glm::rotate(ModelTransform, Rotation, glm::vec3(0, 0, 1));
	ModelTransform = glm::scale(ModelTransform, glm::vec3(Scale, 0.0f));
	glUniformMatrix4fv(LastProgram->ModelTransformID, 1, GL_FALSE, glm::value_ptr(ModelTransform));

	// Texture transform
	glm::mat4 TextureTransform(1.0f);
	TextureTransform[3][0] = TextureCoords[0];
	TextureTransform[3][1] = TextureCoords[1];
	TextureTransform[0][0] = TextureCoords[2] - TextureCoords[0];
	TextureTransform[1][1] = TextureCoords[3] - TextureCoords[1];
	glUniformMatrix4fv(LastProgram->TextureTransformID, 1, GL_FALSE, glm::value_ptr(TextureTransform));

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

// Draw quad with repeated textures
void _Graphics::DrawRepeatable(const glm::vec3 &Start, const glm::vec3 &End, const ae::_Texture *Texture, float Rotation, float ScaleX) {
	SetVBO(VBO_QUAD_UV);
	SetTextureID(Texture->ID);

	// Get size
	glm::vec3 Size = End - Start;

	// Model transform
	glm::mat4 Transform(1.0f);
	Transform[3][0] = Start.x;
	Transform[3][1] = Start.y;
	Transform[3][2] = Start.z;
	Transform[0][0] = Size.x;
	Transform[1][1] = Size.y;
	Transform[2][2] = Size.z;
	glUniformMatrix4fv(LastProgram->ModelTransformID, 1, GL_FALSE, glm::value_ptr(Transform));

	// Texture transform
	glm::mat4 TextureTransform(1.0f);
	if(Rotation != 0.0f)
		TextureTransform = glm::rotate(TextureTransform, glm::radians(Rotation), glm::vec3(0, 0, -1));
	TextureTransform = glm::scale(TextureTransform, glm::vec3(Size.x * ScaleX, Size.y, 1.0f));

	glUniformMatrix4fv(LastProgram->TextureTransformID, 1, GL_FALSE, glm::value_ptr(TextureTransform));
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

// Draw image from a texture atlas
void _Graphics::DrawAtlasTexture(const _Bounds &Bounds, const _Texture *Texture, const glm::vec4 &TextureCoords) {
	SetVBO(VBO_QUAD_UV);
	SetTextureID(Texture->ID);

	// Get size
	glm::vec2 Size = Bounds.End - Bounds.Start;

	// Model transform
	glm::mat4 Transform(1.0f);
	Transform[3][0] = Bounds.Start.x;
	Transform[3][1] = Bounds.Start.y;
	Transform[0][0] = Size.x;
	Transform[1][1] = Size.y;
	glUniformMatrix4fv(LastProgram->ModelTransformID, 1, GL_FALSE, glm::value_ptr(Transform));

	// Texture transform
	glm::mat4 TextureTransform(1.0f);
	TextureTransform[3][0] = TextureCoords[0];
	TextureTransform[3][1] = TextureCoords[1];
	TextureTransform[0][0] = TextureCoords[2] - TextureCoords[0];
	TextureTransform[1][1] = TextureCoords[3] - TextureCoords[1];
	glUniformMatrix4fv(LastProgram->TextureTransformID, 1, GL_FALSE, glm::value_ptr(TextureTransform));
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

// Draw image from texture array
void _Graphics::DrawTextureArray(const _Bounds &Bounds, const _TextureArray *Texture, uint32_t Index) {
	SetVBO(VBO_QUAD_UV);
	SetTextureID(Texture->ID, GL_TEXTURE_2D_ARRAY);
	LastProgram->SetUniformFloat("texture_index", Index);

	// Get size
	glm::vec2 Size = Bounds.End - Bounds.Start;

	// Model transform
	glm::mat4 Transform(1.0f);
	Transform[3][0] = Bounds.Start.x;
	Transform[3][1] = Bounds.Start.y;
	Transform[0][0] = Size.x;
	Transform[1][1] = Size.y;
	glUniformMatrix4fv(LastProgram->ModelTransformID, 1, GL_FALSE, glm::value_ptr(Transform));
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

// Draw double-sided flat wall
void _Graphics::DrawWall(const glm::vec3 &Position, const glm::vec3 &Scale, const ae::_Texture *Texture, int Side) {
	SetVBO(VBO_CUBE);
	SetTextureID(Texture->ID);

	glm::mat4 ModelTransform(1.0f);
	glm::mat4 TextureTransform(1.0f);
	ModelTransform = glm::translate(ModelTransform, glm::vec3(Position.x, Position.y, Position.z));
	if(Side == 1 || Side == 3) {
		TextureTransform[0][0] = Scale.x;
		TextureTransform[1][1] = Scale.z;
	}
	else {
		TextureTransform[0][0] = Scale.y;
		TextureTransform[1][1] = Scale.z;
	}
	ModelTransform = glm::scale(ModelTransform, Scale);

	glUniformMatrix4fv(LastProgram->ModelTransformID, 1, GL_FALSE, glm::value_ptr(ModelTransform));
	glUniformMatrix4fv(LastProgram->TextureTransformID, 1, GL_FALSE, glm::value_ptr(TextureTransform));

	glDrawArrays(GL_TRIANGLE_STRIP, Side * 4, 4);
}

// Draw 3d wall
void _Graphics::DrawCube(const glm::vec3 &Start, const glm::vec3 &Scale, const _Texture *Texture) {
	SetVBO(VBO_CUBE);
	SetTextureID(Texture->ID);

	glm::mat4 ModelTransform(1.0f);
	ModelTransform = glm::translate(ModelTransform, Start);
	ModelTransform = glm::scale(ModelTransform, Scale);
	glUniformMatrix4fv(LastProgram->ModelTransformID, 1, GL_FALSE, glm::value_ptr(ModelTransform));

	glm::mat4 TextureTransform(1.0f);

	// Draw top
	TextureTransform[0][0] = Scale.x;
	TextureTransform[1][1] = Scale.y;
	glUniformMatrix4fv(LastProgram->TextureTransformID, 1, GL_FALSE, glm::value_ptr(TextureTransform));
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	// Draw front
	TextureTransform[0][0] = Scale.x;
	TextureTransform[1][1] = Scale.z;
	glUniformMatrix4fv(LastProgram->TextureTransformID, 1, GL_FALSE, glm::value_ptr(TextureTransform));
	glDrawArrays(GL_TRIANGLE_STRIP, 4, 4);

	// Draw left
	TextureTransform[0][0] = Scale.y;
	TextureTransform[1][1] = Scale.z;
	glUniformMatrix4fv(LastProgram->TextureTransformID, 1, GL_FALSE, glm::value_ptr(TextureTransform));
	glDrawArrays(GL_TRIANGLE_STRIP, 8, 4);

	// Draw back
	TextureTransform[0][0] = Scale.x;
	TextureTransform[1][1] = Scale.z;
	glUniformMatrix4fv(LastProgram->TextureTransformID, 1, GL_FALSE, glm::value_ptr(TextureTransform));
	glDrawArrays(GL_TRIANGLE_STRIP, 12, 4);

	// Draw right
	TextureTransform[0][0] = Scale.y;
	TextureTransform[1][1] = Scale.z;
	glUniformMatrix4fv(LastProgram->TextureTransformID, 1, GL_FALSE, glm::value_ptr(TextureTransform));
	glDrawArrays(GL_TRIANGLE_STRIP, 16, 4);
}

// Vertical texture
void _Graphics::DrawWallDecal(const glm::vec3 &Position, const _Texture *Texture, int Side, const glm::vec2 &Scale) {
	SetVBO(VBO_CUBE);
	SetTextureID(Texture->ID);

	glm::mat4 ModelTransform;

	// Set scale
	switch(Side) {
		case 1:
		case 3:
			ModelTransform[0] = glm::vec4(Scale.x, 0.0f, 0.0f, 0.0f);
			ModelTransform[1] = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
			ModelTransform[2] = glm::vec4(0.0f, 0.0f, Scale.y, 0.0f);
		break;
		case 2:
		case 4:
			ModelTransform[0] = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
			ModelTransform[1] = glm::vec4(0.0f, Scale.x, 0.0f, 0.0f);
			ModelTransform[2] = glm::vec4(0.0f, 0.0f, Scale.y, 0.0f);
		break;
	}

	// Set position
	switch(Side) {
		case 1:
			ModelTransform[3] = glm::vec4(Position.x - 0.5f * Scale.x, Position.y - 1.0f, Position.z - 0.5f * Scale.y, 1.0f);
		break;
		case 2:
			ModelTransform[3] = glm::vec4(Position.x, Position.y - 0.5f * Scale.x, Position.z - 0.5f * Scale.y, 1.0f);
		break;
		case 3:
			ModelTransform[3] = glm::vec4(Position.x - 0.5f * Scale.x, Position.y, Position.z - 0.5f * Scale.y, 1.0f);
		break;
		case 4:
			ModelTransform[3] = glm::vec4(Position.x - 1.0f, Position.y - 0.5f * Scale.x, Position.z - 0.5f * Scale.y, 1.0f);
		break;
	}

	// Draw
	glUniformMatrix4fv(LastProgram->ModelTransformID, 1, GL_FALSE, glm::value_ptr(ModelTransform));
	glDrawArrays(GL_TRIANGLE_STRIP, Side * 4, 4);
}

// Draw 3d mesh
void _Graphics::DrawMesh(const glm::vec3 &Position, const _Mesh *Mesh, const _Texture *Texture, float Rotation, const glm::vec3 &Scale) {
	ae::Graphics.SetTextureID(Texture->ID);
	ae::Graphics.SetVertexBufferID(Mesh->VertexBufferID);
	ae::Graphics.SetAttribLevel(3);

	// Initialize transforms
	glm::mat4 ModelTransform(1.0f);
	glm::mat4 NormalTransform(1.0f);

	// Translate;
	ModelTransform = glm::translate(ModelTransform, Position);

	// Rotate;
	if(Rotation != 0.0f) {
		NormalTransform = glm::rotate(NormalTransform, glm::radians(Rotation), glm::vec3(0, 0, 1));
		ModelTransform *= NormalTransform;
	}

	// Scale
	ModelTransform = glm::scale(ModelTransform, Scale);

	// Set transforms
	glUniformMatrix4fv(LastProgram->ModelTransformID, 1, GL_FALSE, glm::value_ptr(ModelTransform));
	glUniformMatrix4fv(LastProgram->NormalTransformID, 1, GL_FALSE, glm::value_ptr(NormalTransform));

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ae::_PackedVertex), ae::_PackedVertex::GetPositionOffset());
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(ae::_PackedVertex), ae::_PackedVertex::GetUVOffset());
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(ae::_PackedVertex), ae::_PackedVertex::GetNormalOffset());
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Mesh->ElementBufferID);

	glDrawElements(GL_TRIANGLES, (GLsizei)Mesh->IndexCount, GL_UNSIGNED_INT, nullptr);
}

}
