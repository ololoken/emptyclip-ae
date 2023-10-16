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

// Libraries
#include <ae/bounds.h>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <string>
#include <vector>

// Forward Declarations
namespace tinyxml2 {
	class XMLDocument;
	class XMLElement;
}

namespace ae {

// Forward Declarations
class _Font;
class _Texture;
class _TextureArray;
class _Atlas;
class _Program;
class _Sound;
struct _KeyEvent;

// Alignment struct
struct _Alignment {

	enum HorizontalAlignment {
		LEFT,
		CENTER,
		RIGHT,
	};

	enum VerticalAlignment {
		TOP,
		MIDDLE,
		BOTTOM,
		BASELINE,
	};

	_Alignment() : Horizontal(CENTER), Vertical(MIDDLE) {}
	_Alignment(int Horizontal, int Vertical) : Horizontal(Horizontal), Vertical(Vertical) {}

	int Horizontal;
	int Vertical;
};

// Style struct
struct _Style {

	_Style() :
		TextureColor(0.0f),
		BackgroundColor(0.0f),
		BorderColor(0.0f),
		HasBackgroundColor(false),
		HasBorderColor(false),
		Program(nullptr),
		Texture(nullptr),
		Stretch(false) {}

	// Attributes
	std::string Name;

	// Colors
	glm::vec4 TextureColor;
	glm::vec4 BackgroundColor;
	glm::vec4 BorderColor;
	bool HasBackgroundColor;
	bool HasBorderColor;

	// Graphics
	const _Program *Program;
	const _Texture *Texture;

	// Properties
	bool Stretch;
};

// Classes
class _Element {

	public:

		_Element() {}
		_Element(tinyxml2::XMLElement *Node, _Element *Parent);
		~_Element();

		static float GetUIScale();

		void SerializeElement(tinyxml2::XMLDocument &Document, tinyxml2::XMLElement *ParentNode);

		void Update(double FrameTime, const glm::vec2 &Mouse);
		void Render() const;
		bool HandleKey(const _KeyEvent &KeyEvent);
		void HandleMouseButton(bool Pressed);
		void CalculateBounds(bool Scale=true);
		_Element *GetClickedElement();

		void RemoveChild(_Element *Element);
		void UpdateChildrenOffset(const glm::vec2 &Update) { ChildrenOffset += Update; CalculateChildrenBounds(); }
		void CalculateChildrenBounds(bool Scale=true);

		void Clear() { CursorTimer = 0; Text = ""; CursorPosition = 0; }
		void ResetCursor() { CursorTimer = 0; }

		void SetDebug(int Value);
		void SetClickable(bool Value, int Depth=-1);
		void SetActive(bool Value);
		void SetFade(float Value);
		void SetEnabled(bool Value);
		void SetWrap(float Width);
		void SetOffsetPercent(const glm::vec2 &Value) { BaseOffset = Value * (Parent->BaseSize - BaseSize); CalculateBounds(); }
		void SetWidth(float Width) { BaseSize.x = Size.x = Width; CalculateBounds(false); }
		void SetHeight(float Height) { BaseSize.y = Size.y = Height; CalculateBounds(false); }
		void SetText(const std::string &Text) { this->Text = Text; CursorPosition = Text.length(); }

		float GetOffsetPercentX() { return Offset.x / (Parent->Size.x - Size.x); }
		float GetOffsetPercentY() { return Offset.y / (Parent->Size.y - Size.y); }

		// Attributes
		std::string ID;
		_Element *Parent{nullptr};
		int Index{-1};
		void *UserData{nullptr};

		bool Active{false};
		bool Enabled{true};
		bool Checked{false};
		bool Clickable{false};
		bool Draggable{false};
		bool MaskOutside{false};
		bool Stretch{true};
		bool Wrap{false};
		bool Format{false};
		bool Scaled{true};
		bool SizePercent[2]{false, false};
		int Debug{0};

		// Graphics
		glm::vec4 Color{1.0f};
		std::string ColorID;
		std::string SoundID;
		const _Style *Style{nullptr};
		const _Style *HoverStyle{nullptr};
		const _Style *DisabledStyle{nullptr};
		const _Texture *Texture{nullptr};
		const _TextureArray *TextureArray{nullptr};
		const _Atlas *Atlas{nullptr};
		const _Sound *Sound{nullptr};
		uint32_t TextureIndex{0};
		float Fade{1.0f};
		float Rotation{0.0f};

		// Layout
		_Bounds Bounds;
		_Alignment Alignment;
		glm::vec2 BaseOffset{0.0f};
		glm::vec2 BaseSize{0.0f};
		glm::vec2 Size{0.0f};
		glm::vec2 Offset{0.0f};

		// Input
		_Element *HitElement{nullptr};
		_Element *PressedElement{nullptr};
		_Element *ReleasedElement{nullptr};
		glm::vec2 HitOffset{0.0f};
		glm::vec2 PressedOffset{0.0f};

		// Text
		const _Font *Font{nullptr};
		std::string Text;
		std::string AllowedCharacters;
		size_t MaxLength{0};
		size_t CursorPosition{0};
		double CursorTimer{0.0};
		int LastKeyPressed{0};
		bool Password{false};

		// Children
		std::vector<_Element *> Children;
		glm::vec2 ChildrenOffset{0.0f};

	private:

		void DrawStyle(const _Style *DrawStyle) const;
		void AssignAttributeString(tinyxml2::XMLElement *Node, const char *Attribute, std::string &String);

		std::vector<std::string> Texts;

};

extern _Element *FocusedElement;

const _Alignment LEFT_TOP         = _Alignment(_Alignment::LEFT,   _Alignment::TOP);
const _Alignment LEFT_MIDDLE      = _Alignment(_Alignment::LEFT,   _Alignment::MIDDLE);
const _Alignment LEFT_BOTTOM      = _Alignment(_Alignment::LEFT,   _Alignment::BOTTOM);
const _Alignment LEFT_BASELINE    = _Alignment(_Alignment::LEFT,   _Alignment::BASELINE);
const _Alignment RIGHT_TOP        = _Alignment(_Alignment::RIGHT,  _Alignment::TOP);
const _Alignment RIGHT_MIDDLE     = _Alignment(_Alignment::RIGHT,  _Alignment::MIDDLE);
const _Alignment RIGHT_BOTTOM     = _Alignment(_Alignment::RIGHT,  _Alignment::BOTTOM);
const _Alignment RIGHT_BASELINE   = _Alignment(_Alignment::RIGHT,  _Alignment::BASELINE);
const _Alignment CENTER_TOP       = _Alignment(_Alignment::CENTER, _Alignment::TOP);
const _Alignment CENTER_MIDDLE    = _Alignment(_Alignment::CENTER, _Alignment::MIDDLE);
const _Alignment CENTER_BOTTOM    = _Alignment(_Alignment::CENTER, _Alignment::BOTTOM);
const _Alignment CENTER_BASELINE  = _Alignment(_Alignment::CENTER, _Alignment::BASELINE);

}
