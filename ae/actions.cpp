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
#include <ae/actions.h>
#include <ae/state.h>
#include <ae/assets.h>
#include <fstream>
#include <iostream>

namespace ae {

_Actions Actions;

// Reset the action state
void _Actions::ResetState() {
	for(size_t i = 0; i < State.size(); i++) {
		State[i].Value = 0.0f;
		State[i].Source = -1;
	}
}

// Clear all mappings
void _Actions::ClearMappings(int InputType) {
	for(int Input = 0; Input < ACTIONS_MAXINPUTS; Input++)
		InputMap[InputType][Input].clear();
}

// Remove all actions for an input
void _Actions::ClearMappingForInput(int InputType, int Input) {
	InputMap[InputType][Input].clear();
}

// Remove a mapping for an action
void _Actions::ClearMappingsForAction(int InputType, size_t Action, int Rank) {
	for(int Input = 0; Input < ACTIONS_MAXINPUTS; Input++) {
		for(auto MapIterator = InputMap[InputType][Input].begin(); MapIterator != InputMap[InputType][Input].end(); ) {
			if(MapIterator->Action == Action && MapIterator->Rank == Rank) {
				MapIterator = InputMap[InputType][Input].erase(MapIterator);
			}
			else
				++MapIterator;
		}
	}
}

// Serialize input map to stream
void _Actions::Serialize(std::ofstream &File, int InputType) {
	for(int Input = 0; Input < ACTIONS_MAXINPUTS; Input++) {
		for(auto &Iterator : InputMap[InputType][Input]) {
			File << "action_" << State[Iterator.Action].Name << "=" << Iterator.Rank << "_" << InputType << "_" << Input << std::endl;
		}
	}
}

// Add an input mapping
void _Actions::AddInputMap(int Rank, int InputType, int Input, size_t Action, float Scale, float DeadZone, bool IfNone) {
	if(Action >= State.size() || Input < 0 || Input >= ACTIONS_MAXINPUTS)
		return;

	if(!IfNone || (IfNone && !HasInputForAction(Action)))
		InputMap[InputType][Input].push_back(_ActionMap(Rank, Action, Scale, DeadZone));
}

// Returns an input for an action
int _Actions::GetInputForAction(int InputType, size_t Action, int Rank) {

	for(int Input = 0; Input < ACTIONS_MAXINPUTS; Input++) {
		for(auto &MapIterator : InputMap[InputType][Input]) {
			if(MapIterator.Action == Action && MapIterator.Rank == Rank) {
				return Input;
			}
		}
	}

	return -1;
}

// Return true if an input exists for an action
bool _Actions::HasInputForAction(size_t Action) {
	for(int i = 0; i < _Input::INPUT_COUNT; i++) {
		if(GetInputForAction(i, Action) != -1)
			return true;
	}

	return false;
}

// Get name of input key/button for a given action
std::string _Actions::GetInputNameForAction(size_t Action, int Rank) {

	for(int InputType = 0; InputType < _Input::INPUT_COUNT; InputType++) {
		int Input = GetInputForAction(InputType, Action, Rank);
		if(Input != -1) {
			switch(InputType) {
				case _Input::KEYBOARD:
					return _Input::GetKeyName(Input);
				case _Input::MOUSE_BUTTON:
					return _Input::GetMouseButtonName((uint32_t)Input);
			}
		}
	}

	return "";
}

// Inject an input into the action handler
void _Actions::InputEvent(_State *GameState, int InputType, int Input, float Value) {
	if(Input < 0 || Input >= ACTIONS_MAXINPUTS || !GameState)
		return;

	for(auto &MapIterator : InputMap[InputType][Input]) {

		// Only let joystick overwrite action state if the keyboard isn't being used
		if(InputType != _Input::JOYSTICK_AXIS || (InputType == _Input::JOYSTICK_AXIS && (State[MapIterator.Action].Source == -1 || State[MapIterator.Action].Source == _Input::JOYSTICK_AXIS))) {

			// If key was released, set source to -1 so that joystick can overwrite it
			if(InputType == _Input::KEYBOARD && Value == 0.0f)
				State[MapIterator.Action].Source = -1;
			else
				State[MapIterator.Action].Source = InputType;

			// Check for deadzone
			if(fabs(Value) <= MapIterator.DeadZone)
				Value = 0.0f;

			State[MapIterator.Action].Value = Value;
		}

		// Check for deadzone
		if(fabs(Value) <= MapIterator.DeadZone)
			Value = 0.0f;

		// Apply input scale to action
		float InputValue = Value * MapIterator.Scale;

		// If true is returned, stop handling the same key
		if(GameState->HandleAction(InputType, MapIterator.Action, (int)InputValue))
			break;
	}
}

}
