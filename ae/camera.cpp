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
#include <ae/camera.h>
#include <ae/graphics.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>
#include <cmath>

namespace ae {

// Initialize
_Camera::_Camera(const _CameraSettings &Settings) :
	UpdateDivisor(Settings.UpdateDivisor),
	SnappingThreshold(Settings.SnappingThreshold),
	Fovy(Settings.Fovy),
	Near(Settings.Near),
	Far(Settings.Far) {
}

// Calculate the frustum
void _Camera::CalculateFrustum(float AspectRatio) {
	Frustum.y = (float)std::tan(Fovy / 360.0f * glm::pi<float>()) * Near;
	Frustum.x = Frustum.y * AspectRatio;
	Projection = glm::frustum(-Frustum.x, Frustum.x, Frustum.y, -Frustum.y, Near, Far);
}

// Set up 3d projection matrix
void _Camera::Set3DProjection(double BlendFactor) {
	glm::vec3 DrawPosition;
	GetDrawPosition(BlendFactor, DrawPosition);

	glm::vec2 Size(DrawPosition.z * Graphics.AspectRatio, DrawPosition.z);

	// Get AABB at z=0
	AABB[0] = -Size.x + DrawPosition.x;
	AABB[1] = -Size.y + DrawPosition.y;
	AABB[2] = Size.x + DrawPosition.x;
	AABB[3] = Size.y + DrawPosition.y;

	Transform = Projection * glm::translate(glm::mat4(1.0f), -DrawPosition);
}

// Converts screen space to world space
void _Camera::ConvertScreenToWorld(const glm::vec2 &ScreenPosition, glm::vec2 &WorldPosition) const {
	WorldPosition.x = (ScreenPosition.x / (float)(Graphics.ViewportSize.x) - 0.5f) * Position.z * Graphics.AspectRatio * 2 + Position.x;
	WorldPosition.y = (ScreenPosition.y / (float)(Graphics.ViewportSize.y) - 0.5f) * Position.z * 2 + Position.y;
}

// Converts world space to screen space
void _Camera::ConvertWorldToScreen(const glm::vec2 &WorldPosition, glm::vec2 &ScreenPosition) const {
	ScreenPosition.x = (int)(Graphics.ViewportSize.x * (0.5f + ((WorldPosition.x - Position.x) / (Position.z * Graphics.AspectRatio * 2))));
	ScreenPosition.y = (int)(Graphics.ViewportSize.y * (0.5f + ((WorldPosition.y - Position.y) / (Position.z * 2))));
}

// Update camera
void _Camera::Update(double FrameTime) {
	LastPosition = Position;

	// Cap distance
	if(TargetPosition.z <= 1.0f)
		TargetPosition.z = 1.0f;
	else if(TargetPosition.z >= Far)
		TargetPosition.z = Far;

	// Update position
	glm::vec2 Delta(TargetPosition - Position);
	if(std::abs(Delta.x) > SnappingThreshold)
		Position.x += FrameTime * Delta.x / UpdateDivisor;
	else
		Position.x = TargetPosition.x;

	if(std::abs(Delta.y) > SnappingThreshold)
		Position.y += FrameTime * Delta.y / UpdateDivisor;
	else
		Position.y = TargetPosition.y;

	// Update distance
	float DeltaZ = TargetPosition.z - Position.z;
	if(std::abs(DeltaZ) > SnappingThreshold)
		Position.z += FrameTime * DeltaZ / UpdateDivisor;
	else
		Position.z = TargetPosition.z;
}

// Interpolate between position and last position
void _Camera::GetDrawPosition(double BlendFactor, glm::vec3 &DrawPosition) const {
	if(Position == LastPosition)
		DrawPosition = Position;
	else
		DrawPosition = Position * (float)BlendFactor + LastPosition * (1.0f - (float)BlendFactor);
}

// Determines whether a circle is in view
bool _Camera::IsCircleInView(const glm::vec2 &Center, float Radius) const {

	// Get closest point on AABB
	glm::vec2 Point(Center);
	if(Point.x < AABB[0])
		Point.x = AABB[0];
	if(Point.y < AABB[1])
		Point.y = AABB[1];
	if(Point.x > AABB[2])
		Point.x = AABB[2];
	if(Point.y > AABB[3])
		Point.y = AABB[3];

	// Test circle collision with point
	float DistanceSquared = glm::distance2(Point, Center);
	bool Hit = DistanceSquared < Radius * Radius;

	return Hit;
}

// Determines whether an AABB is in view
bool _Camera::IsAABBInView(const glm::vec4 &Bounds) const {

	if(Bounds[2] < AABB[0] || Bounds[0] > AABB[2])
		return false;

	if(Bounds[3] < AABB[1] || Bounds[1] > AABB[3])
		return false;

	return true;
}

}
