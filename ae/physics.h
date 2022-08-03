/******************************************************************************
* Copyright (c) 2022 Alan Witkowski
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
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

namespace ae {

// Physics shape
class _Shape {

	public:

		_Shape() {}

		// AABB
		bool IsAABB() const { return HalfSize[1] != 0.0f; }
		glm::vec4 GetAABB(const glm::vec2 &Position);

		// Properties
		glm::vec2 HalfSize{0.0f};
};

// Physics response
struct _Manifold {
	_Manifold() {}
	bool IsDiagonal() const { return Normal.x != 0 && Normal.y != 0; }

	void *ObjectA{nullptr};
	void *ObjectB{nullptr};
	glm::vec2 Normal{0.0f};
	float Penetration = 0.0f;
};

// Rigid body
class _RigidBody {

	public:

		_RigidBody() {}
		_RigidBody(const glm::vec2 &Position, const glm::vec2 &Velocity, const glm::vec2 &Acceleration);

		// Update
		void Update(float DeltaTime);
		void ForcePosition(const glm::vec2 &Position) { this->Position = this->LastPosition = Position; }
		void SetMass(float Mass) { InverseMass = Mass > 0.0f ? 1.0f / Mass : 0.0f; }

		// State
		glm::vec2 LastPosition{0.0f};
		glm::vec2 Position{0.0f};
		glm::vec2 Velocity{0.0f};
		glm::vec2 Acceleration{0.0f};
		float InverseMass{0.0f};
		float Restitution{1.0f};
		int CollisionMask{0};
		int CollisionGroup{0};
		bool CollisionResponse{true};

	private:

		void RungeKutta4Evaluate(const _RigidBody &Derivative, float DeltaTime, _RigidBody &Output);

};

}
