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
#include <ae/ui.h>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <ft2build.h>
#include <string>
#include <vector>
#include FT_FREETYPE_H

namespace ae {

// Forward Declarations
class _Program;
class _Texture;

// Contains glyph info
struct _Glyph {
	float Left{0.0f};
	float Top{0.0f};
	float Right{0.0f};
	float Bottom{0.0f};
	float Width{0.0f};
	float Height{0.0f};
	float Advance{0.0f};
	float OffsetX{0.0f};
	float OffsetY{0.0f};
};

// Contains text dimensions
struct _TextBounds {
	int Width{0};
	int AboveBase{0};
	int BelowBase{0};
};

// Classes
class _Font {

	public:

		_Font() { }
		~_Font();

		static void Init(size_t DrawCount);
		static void Close();
		static void Draw();

		void Load(const std::string &ID, const std::string &Path, const _Program *Program, uint32_t FontSize, uint32_t TextureWidth=256);

		float DrawText(const std::string &Text, glm::vec2 Position, const _Alignment &Alignment=LEFT_BASELINE, const glm::vec4 &Color=glm::vec4(1.0f), float Scale=1.0f, bool ForceDraw=true) const;
		void DrawTextFormatted(const std::string &Text, glm::vec2 Position, const _Alignment &Alignment=LEFT_BASELINE, float Alpha=1.0f, float Scale=1.0f) const;
		void SetupProgram() const;
		void GetStringDimensions(const std::string &Text, _TextBounds &TextBounds, bool UseFormatting=false) const;
		void BreakupString(const std::string &Text, float Width, std::vector<std::string> &Strings, bool UseFormatting=false) const;

		// Attributes
		std::string ID;
		float MaxHeight{0.0f};
		float MaxAbove{0.0f};
		float MaxBelow{0.0f};

	private:

		void DeleteFont();
		void CreateFontTexture(std::string SortedCharacters, uint32_t TextureWidth);
		void SortCharacters(FT_Face &Face, const std::string &Characters, std::string &SortedCharacters);
		void AddGlyph(glm::vec2 &Position, char Char, float Scale) const;
		void AdjustPosition(const std::string &Text, glm::vec2 &Position, bool UseFormatting, const _Alignment &Alignment, float Scale) const;

		// Glyphs
		_Glyph Glyphs[256]{{0}};

		// Graphics
		const _Program *Program{nullptr};
		_Texture *Texture{nullptr};

		// Freetype
		FT_Face Face{nullptr};
		FT_Int32 LoadFlags{0};
		bool HasKerning{false};
};

}
