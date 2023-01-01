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
#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>

namespace ae {

struct _CameraSettings {
	float UpdateDivisor{0.1f};
	float Fovy{90.0f};
	float Near{0.1f};
	float Far{500.0f};
	float SnappingThreshold{0.005f};
};

// Camera class
class _Camera {

	public:

		_Camera(const _CameraSettings &Settings);
		~_Camera() {}

		// Updates
		void CalculateFrustum(float AspectRatio);
		void Set3DProjection(double BlendFactor);
		void ConvertScreenToWorld(const glm::vec2 &ScreenPosition, glm::vec2 &WorldPosition) const;
		void ConvertWorldToScreen(const glm::vec2 &WorldPosition, glm::vec2 &ScreenPosition) const;

		void Update(double FrameTime);
		void UpdatePosition(const glm::vec2 &UpdatePosition) { this->TargetPosition += glm::vec3(UpdatePosition, 0.0f); }
		void UpdateDistance(float Update) { this->TargetPosition.z += Update; }

		void ForcePosition(const glm::vec3 &Position) { this->TargetPosition = this->LastPosition = this->Position = Position; }
		void Set2DPosition(const glm::vec2 &Position) { this->TargetPosition = glm::vec3(Position.x, Position.y, this->Position.z); }
		void SetDistance(float Distance) { this->TargetPosition.z = Distance; }
		void SetUpdateDivisor(float Divisor) { UpdateDivisor = Divisor; }

		void GetDrawPosition(double BlendFactor, glm::vec3 &DrawPosition) const;
		const glm::vec3 &GetPosition() const { return Position; }

		bool IsCircleInView(const glm::vec2 &Position, float Radius) const;
		bool IsAABBInView(const glm::vec4 &Bounds) const;

		glm::mat4 Transform;
		glm::vec4 AABB{0.0f};

	private:

		glm::mat4 Projection;

		glm::vec3 LastPosition{0.0f};
		glm::vec3 Position{0.0f};
		glm::vec3 TargetPosition{0.0f};
		float UpdateDivisor{0.1f};
		float SnappingThreshold{0.005f};

		glm::vec2 Frustum{0.0f};
		float Fovy{90.0f};
		float Near{0.1f};
		float Far{500.0f};
};

}
