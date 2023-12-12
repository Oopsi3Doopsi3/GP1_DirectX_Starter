#pragma once
#include <cassert>
#include <SDL_keyboard.h>
#include <SDL_mouse.h>

#include "Math.h"
#include "Timer.h"

namespace dae
{
	struct Camera
	{
		Camera() = default;

		Camera(const Vector3& _origin, float _fovAngle, float aspectRatio):
			origin{_origin},
			fovAngle{_fovAngle},
			aspectRatio{aspectRatio}
		{
		}


		Vector3 origin{};
		float fovAngle{90.f};
		float fov{ tanf((fovAngle * TO_RADIANS) / 2.f) };
		float aspectRatio{};

		bool fovChanged{ false };
		bool arChanged{ false };

		Vector3 forward{Vector3::UnitZ};
		Vector3 up{Vector3::UnitY};
		Vector3 right{Vector3::UnitX};

		float totalPitch{};
		float totalYaw{};

		Matrix invViewMatrix{};
		Matrix viewMatrix{};
		Matrix projectionMatrix{};

		float nearClippingPlane{ .1f };
		float farClippingPlane{ 100.f };

		void Initialize(float aspect, float _fovAngle = 90.f, Vector3 _origin = {0.f,0.f,0.f})
		{
			aspectRatio = aspect;

			fovAngle = _fovAngle;
			fov = tanf((fovAngle * TO_RADIANS) / 2.f);

			origin = _origin;

			CalculateProjectionMatrix();
		}

		void CalculateViewMatrix()
		{
			//ONB => invViewMatrix
			//Inverse(ONB) => ViewMatrix
			viewMatrix = { right,
						up,
						forward,
						origin };

			invViewMatrix = Matrix::Inverse(viewMatrix);

			//ViewMatrix => Matrix::CreateLookAtLH(...) [not implemented yet]
			//DirectX Implementation => https://learn.microsoft.com/en-us/windows/win32/direct3d9/d3dxmatrixlookatlh
			//viewMatrix = Matrix::CreateLookAtLH(origin, forward, up);
		}

		void CalculateProjectionMatrix()
		{
			//ProjectionMatrix => Matrix::CreatePerspectiveFovLH(...) [not implemented yet]
			//DirectX Implementation => https://learn.microsoft.com/en-us/windows/win32/direct3d9/d3dxmatrixperspectivefovlh
			projectionMatrix =
			{
				Vector4{1.f / (aspectRatio * fov), 0, 0, 0},
				Vector4{0, 1.f / fov, 0, 0},
				Vector4{0, 0, farClippingPlane / (farClippingPlane - nearClippingPlane), 1},
				Vector4{0, 0, -(farClippingPlane * nearClippingPlane) / (farClippingPlane - nearClippingPlane), 0}
			};
		}

		void Update(const Timer* pTimer)
		{
			const float deltaTime = pTimer->GetElapsed();

			//Camera Update Logic
			//Keyboard Input
			const uint8_t* pKeyboardState = SDL_GetKeyboardState(nullptr);

			constexpr float speed{ 30.f };
			constexpr float mouseSpeed{ 1.f };

			if (pKeyboardState[SDL_SCANCODE_W])
			{
				origin += forward * speed * deltaTime;
			}
			if (pKeyboardState[SDL_SCANCODE_S])
			{
				origin -= forward * speed * deltaTime;
			}
			if (pKeyboardState[SDL_SCANCODE_A])
			{
				origin -= right * speed * deltaTime;
			}
			if (pKeyboardState[SDL_SCANCODE_D])
			{
				origin += right * speed * deltaTime;
			}

			//Mouse Input
			int mouseX{}, mouseY{};
			const uint32_t mouseState = SDL_GetRelativeMouseState(&mouseX, &mouseY);
			bool rightDown{ (mouseState & SDL_BUTTON(SDL_BUTTON_RIGHT)) != 0};
			bool leftDown{ (mouseState & SDL_BUTTON(SDL_BUTTON_LEFT)) != 0};

			bool didRotate{ false };

			if (leftDown && rightDown)
			{
				origin.y += speed * mouseY * deltaTime;
			}
			else if (leftDown)
			{
				origin -= speed * forward * float(mouseY) * deltaTime;
				totalYaw += mouseSpeed * mouseX * deltaTime;
				didRotate = true;
			}
			else if (rightDown)
			{
				totalYaw += speed * mouseX * deltaTime;
				totalPitch -= speed * mouseY * deltaTime;
				didRotate = true;
			}

			if (didRotate)
			{
				forward = Matrix::CreateRotation(totalPitch, totalYaw, 0.f).TransformVector(Vector3::UnitZ);
				right = Vector3::Cross(Vector3::UnitY, forward).Normalized();
				up = Vector3::Cross(forward, right).Normalized();
			}

			//Update Matrices
			CalculateViewMatrix();
			if(fovChanged || arChanged)	CalculateProjectionMatrix(); //Try to optimize this - should only be called once or when fov/aspectRatio changes
		}

		Matrix GetViewMatrix()
		{
			return invViewMatrix;
		}

		Matrix GetProjectionMatrix()
		{
			return projectionMatrix;
		}
	};
}
