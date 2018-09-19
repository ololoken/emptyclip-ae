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
#include <ae/console.h>
#include <ae/assets.h>
#include <ae/ui.h>
#include <ae/font.h>
#include <ae/graphics.h>

namespace ae {

const float PADDING = 5.0f;
const glm::vec4 TEXT_COLOR = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
const glm::vec4 CONSOLE_BG_COLOR = glm::vec4(0.0f, 0.0f, 0.0f, 0.95f);
const glm::vec4 TEXTBOX_BG_COLOR = glm::vec4(0.05f, 0.05f, 0.05f, 0.5f);

// Initialize
_Console::_Console(const _Program *Program, const _Font *Font) :
	Element(nullptr),
	Font(Font),
	RowHeight(0.0f),
	BelowBase(0.0f) {

	// Get font dimensions
	_TextBounds TextBounds;
	Font->GetStringDimensions("Py", TextBounds);
	RowHeight = TextBounds.AboveBase + TextBounds.BelowBase + PADDING;
	BelowBase = TextBounds.BelowBase;

	// Add style
	Style = new _Style();
	Style->Program = Program;
	Style->HasBackgroundColor = true;
	Style->BackgroundColor = CONSOLE_BG_COLOR;

	// Add textbox style
	InputStyle = new _Style();
	InputStyle->Program = Program;
	InputStyle->HasBackgroundColor = true;
	InputStyle->BackgroundColor = TEXTBOX_BG_COLOR;

	// Add background element
	Element = new _Element();
	Element->Parent = Graphics.Element;
	Element->Style = Style;
	Element->Alignment = LEFT_TOP;
	Graphics.Element->Children.push_back(Element);

	// Add textbox background element
	_Element *TextboxBackgroundElement = new _Element();
	TextboxBackgroundElement->Parent = Element;
	TextboxBackgroundElement->Style = InputStyle;
	TextboxBackgroundElement->Alignment = LEFT_BOTTOM;
	TextboxBackgroundElement->Size = glm::vec2(Graphics.Element->Size.x, RowHeight + PADDING);
	Element->Children.push_back(TextboxBackgroundElement);

	// Add textbox element
	TextboxElement = new _Element();
	TextboxElement->Parent = TextboxBackgroundElement;
	TextboxElement->Alignment = LEFT_BASELINE;
	TextboxElement->MaxLength = 255;
	TextboxElement->Offset = glm::vec2(PADDING, RowHeight - 1);
	TextboxElement->Font = Font;
	TextboxBackgroundElement->Children.push_back(TextboxElement);

	// Update size of main element
	UpdateSize();
}

// Destructor
_Console::~_Console() {
	delete Style;
	delete InputStyle;
}

// Update
void _Console::Update(double FrameTime) {

	// Handle commands
	if(TextboxElement->ReturnKeyPressed) {
		if(!TextboxElement->Text.empty())
			AddMessage(TextboxElement->Text, TEXT_COLOR);
		TextboxElement->Text = "";
		TextboxElement->ReturnKeyPressed = false;
	}
}

// Render
void _Console::Render(double BlendFactor) {
	if(!IsOpen())
		return;

	// Draw background
	Element->Render();

	// Draw messages
	glm::vec2 DrawPosition(PADDING, Element->Bounds.End.y - TextboxElement->Parent->Size.y - BelowBase - PADDING);
	for(auto Iterator = Messages.rbegin(); Iterator != Messages.rend(); ++Iterator) {
		_Message &Message = (*Iterator);
		Font->DrawText(Message.Text, DrawPosition, LEFT_BASELINE, Message.Color);
		DrawPosition.y -= RowHeight;

		if(DrawPosition.y < 0)
			break;
	}
}

// Return if true if console is open
bool _Console::IsOpen() {
	return Element->Active;
}

// Toggle display of console
void _Console::Toggle() {
	Element->SetActive(!Element->Active);
	ae::FocusedElement = Element->Active ? TextboxElement : nullptr;
	TextboxElement->ResetCursor();
}

// Update size of console based on parent element
void _Console::UpdateSize() {
	Element->Size = glm::vec2(Element->Parent->Size.x, Element->Parent->Size.y / 2.0f);
	Element->CalculateBounds();
}

// Add message to console
void _Console::AddMessage(const std::string &Text, const glm::vec4 &Color) {
	_Message Message;
	Message.Text = Text;
	Message.Color = Color;
	Messages.push_back(Message);
}

}
