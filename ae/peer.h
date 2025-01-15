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
#ifndef AE_NO_NETWORK
#pragma once

// Libraries
#include <cstdint>

// Forward Declarations
class _Object;
struct _ENetPeer;

namespace ae {

// Peer
class _Peer {

	public:

		_Peer(_ENetPeer *ENetPeer) : ENetPeer(ENetPeer) { }
		~_Peer();

		_ENetPeer *ENetPeer{nullptr};
		_Object *Object{nullptr};
		uint32_t AccountID{0};
		int DisconnectFlag{-1};

};

}
#endif
