/******************************************************************************
* Copyright (c) 2018 Alan Witkowski
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
#include <list>

// Forward Declarations
namespace tinyxml2 {
	class XMLDocument;
	class XMLElement;
}

namespace ae {

// Forward Declarations
class _Font;
class _Texture;
class _Atlas;
class _Program;
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

	_Alignment() { Horizontal = CENTER; Vertical = MIDDLE; }
	_Alignment(int Horizontal, int Vertical) : Horizontal(Horizontal), Vertical(Vertical) { }

	int Horizontal, Vertical;
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
		Stretch(false) { }

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

		_Element();
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
		void CalculateChildrenBounds();

		void Clear() { CursorTimer = 0; Text = ""; CursorPosition = 0; }
		void ResetCursor() { CursorTimer = 0; }

		void SetDebug(int Debug);
		void SetClickable(bool Clickable, int Depth=-1);
		void SetActive(bool Active);
		void SetFade(float Fade);
		void SetEnabled(bool Enabled);
		void SetOffsetPercent(const glm::vec2 &Value) { BaseOffset = Value * (Parent->BaseSize - BaseSize); CalculateBounds(); }
		void SetWidth(float Width) { BaseSize.x = Size.x = Width; CalculateBounds(false); }
		void SetHeight(float Height) { BaseSize.y = Size.y = Height; CalculateBounds(false); }
		void SetText(const std::string &Text) { this->Text = Text; CursorPosition = Text.length(); }
		void SetWrap(float Width);

		glm::vec2 GetOffsetPercent() { return Offset / (Parent->Size - Size); }

		// Attributes
		std::string Name;
		_Element *Parent;
		int Index;
		void *UserData;

		bool Active;
		bool Enabled;
		bool Checked;
		bool Clickable;
		bool Draggable;
		bool MaskOutside;
		bool Stretch;
		int Debug;

		// Graphics
		glm::vec4 Color;
		std::string ColorName;
		const _Style *Style;
		const _Style *HoverStyle;
		const _Style *DisabledStyle;
		const _Texture *Texture;
		const _Atlas *Atlas;
		uint32_t TextureIndex;
		float Fade;

		// Layout
		_Bounds Bounds;
		_Bounds ScaledBounds;
		_Alignment Alignment;
		glm::vec2 BaseOffset;
		glm::vec2 BaseSize;
		glm::vec2 Size;
		glm::vec2 Offset;

		// Input
		_Element *HitElement;
		_Element *PressedElement;
		_Element *ReleasedElement;
		glm::vec2 HitOffset;
		glm::vec2 PressedOffset;

		// Text
		const _Font *Font;
		std::string Text;
		size_t MaxLength;
		size_t CursorPosition;
		double CursorTimer;
		int LastKeyPressed;
		bool Password;

		// Children
		std::list<_Element *> Children;
		glm::vec2 ChildrenOffset;

	private:

		void DrawStyle(const _Style *DrawStyle) const;
		void AssignAttributeString(tinyxml2::XMLElement *Node, const char *Attribute, std::string &String);

		std::list<std::string> Texts;

};

extern _Element *FocusedElement;

const _Alignment LEFT_TOP         = _Alignment(_Alignment::LEFT,   _Alignment::TOP);
const _Alignment LEFT_BOTTOM      = _Alignment(_Alignment::LEFT,   _Alignment::BOTTOM);
const _Alignment RIGHT_BOTTOM     = _Alignment(_Alignment::RIGHT,  _Alignment::BOTTOM);
const _Alignment CENTER_MIDDLE    = _Alignment(_Alignment::CENTER, _Alignment::MIDDLE);
const _Alignment LEFT_BASELINE    = _Alignment(_Alignment::LEFT,   _Alignment::BASELINE);
const _Alignment RIGHT_BASELINE   = _Alignment(_Alignment::RIGHT,  _Alignment::BASELINE);
const _Alignment CENTER_BASELINE  = _Alignment(_Alignment::CENTER, _Alignment::BASELINE);

}
