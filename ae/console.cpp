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

// Initialize
_Console::_Console(const _Program *Program, const _Font *Font) :
	Element(nullptr),
	Font(Font),
	FontHeight(0.0f) {

	// Add style
	Style = new _Style();
	Style->Program = Program;
	Style->HasBackgroundColor = true;
	Style->BackgroundColor = glm::vec4(0.0f, 0.0f, 0.0f, 0.95f);

	// Add background element
	Element = new _Element();
	Element->Parent = Graphics.Element;
	Element->Style = Style;
	Element->Alignment = LEFT_TOP;
	Element->SetDebug(0);
	UpdateSize();
	Graphics.Element->Children.push_back(Element);

	// Get font dimensions
	_TextBounds TextBounds;
	Font->GetStringDimensions("Py", TextBounds);
	FontHeight = TextBounds.AboveBase + TextBounds.BelowBase + 5;
}

// Destructor
_Console::~_Console() {
	delete Style;
}

// Update
void _Console::Update(double FrameTime) {

}

// Render
void _Console::Render(double BlendFactor) {
	if(!Element->Active)
		return;

	// Draw background
	Element->Render();

	// Draw messages
	glm::vec2 DrawPosition(5.0f, Element->Bounds.End.y - FontHeight);
	for(auto Iterator = Messages.rbegin(); Iterator != Messages.rend(); ++Iterator) {
		_Message &Message = (*Iterator);
		Font->DrawText(Message.Text, DrawPosition, LEFT_BASELINE, Message.Color);
		DrawPosition.y -= FontHeight;

		if(DrawPosition.y < 0)
			break;
	}
}

// Toggle display of console
void _Console::Toggle() {
	Element->Active = !Element->Active;
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
