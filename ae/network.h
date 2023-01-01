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
#ifndef AE_NO_NETWORK
#pragma once

// Libraries
#include <vector>
#include <queue>
#include <cstdint>

// Forward Declarations
typedef struct _ENetEvent ENetEvent;
typedef struct _ENetHost ENetHost;
typedef struct _ENetAddress ENetAddress;

namespace ae {

const uint32_t NETWORK_BROADCAST = 0xFFFFFFFFU;

// Forward Declarations
class _Buffer;
class _Peer;

// Network address
struct _NetworkAddress {
	_NetworkAddress() {}
	_NetworkAddress(uint32_t Host, uint16_t Port) : Host(Host), Port(Port) {}

	void GetIP(char *IP);

	uint32_t Host{0};
	uint16_t Port{0};
};

// Network Event
struct _NetworkEvent {

	// Types
	enum EventType {
		CONNECT,
		DISCONNECT,
		PACKET,
	};

	EventType Type{PACKET};
	double Time{0.0};
	int EventData{0};
	_Buffer *Data{nullptr};
	_Peer *Peer{nullptr};
};

class _Network {

	public:

		// Different ways to send data
		enum SendType {
			RELIABLE = 1,
			UNSEQUENCED = 2,
		};

		_Network();
		virtual ~_Network();

		void Update(double FrameTime);
		bool CheckPings(_Buffer &Data, _NetworkAddress &NetworkAddress);

		// Settings
		void SetFakeLag(double Value) { FakeLag = Value; }

		// Sockets
		void SendPingPacket(const _Buffer &Buffer, const _NetworkAddress &NetworkAddress);

		// Updates
		bool GetNetworkEvent(_NetworkEvent &NetworkEvent);
		bool HasConnection() { return Connection != nullptr; }

		// Stats
		double GetSentSpeed() { return SentSpeed; }
		double GetReceiveSpeed() { return ReceiveSpeed; }

		// Internals
		void SetUpdatePeriod(double UpdatePeriod) { this->UpdatePeriod = UpdatePeriod; }
		double GetUpdatePeriod() const { return UpdatePeriod; }
		bool NeedsUpdate() { return UpdateTimer >= UpdatePeriod; }
		void ResetUpdateTimer() { UpdateTimer = 0.0; }

		// Static functions
		static void InitializeSystem();
		static void CloseSystem();

		// Determine if an ack is newer or the same as another
		static bool MoreRecentAck(uint16_t Previous, uint16_t Current, uint16_t Max) {
			return (Current > Previous && Current - Previous <= Max / 2) || (Previous > Current && Previous - Current > Max / 2);
		}

	protected:

		virtual void CreateEvent(_NetworkEvent &Event, double Time, ENetEvent &EEvent) {}
		virtual void HandleEvent(_NetworkEvent &Event, ENetEvent &EEvent) {}

		// State
		ENetHost *Connection{nullptr};
		int PingSocket{-1};
		double Time{0.0};

		// Updates
		double UpdateTimer{0.0};
		double UpdatePeriod{1.0 / 20.0};

		// Stats
		double SentSpeed{0.0};
		double ReceiveSpeed{0.0};
		double SecondTimer{0.0};

		// Fake lag
		double FakeLag{0.0};
		std::queue<_NetworkEvent> NetworkEvents;
};

}
#endif
