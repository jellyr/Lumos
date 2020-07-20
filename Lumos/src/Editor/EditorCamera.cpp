#include "lmpch.h"
#include "EditorCamera.h"
#include "Editor.h"
#include "Graphics/Camera/Camera.h"
#include "Core/Application.h"
#include "Core/OS/Input.h"

namespace Lumos
{
	EditorCameraController::EditorCameraController()
	{
		m_RotateDampeningFactor = 0.0f;
		m_FocalPoint = Maths::Vector3::ZERO;
		m_Velocity = Maths::Vector3(0.0f);
		m_MouseSensitivity = 0.005f;
	}

	EditorCameraController::~EditorCameraController()
	{
	}

	void EditorCameraController::HandleMouse(Camera* camera, float dt, float xpos, float ypos)
	{
		if(Input::GetInput()->GetMouseHeld(InputCode::MouseKey::ButtonRight))
		{
			if(m_2DMode)
			{
				m_MouseSensitivity = 0.005f;
				Maths::Vector3 position = camera->GetPosition();
				position.x -= (xpos - m_PreviousCurserPos.x) * camera->GetScale() * m_MouseSensitivity * 0.5f;
				position.y += (ypos - m_PreviousCurserPos.y) * camera->GetScale() * m_MouseSensitivity * 0.5f;
				camera->SetPosition(position);
			}
			else
			{
				m_MouseSensitivity = 0.1f;
				m_RotateVelocity = m_RotateVelocity + Maths::Vector2((xpos - m_PreviousCurserPos.x), (ypos - m_PreviousCurserPos.y)) * m_MouseSensitivity;

				float pitch = camera->GetPitch();
				float yaw = camera->GetYaw();

				pitch -= m_RotateVelocity.y;
				yaw -= m_RotateVelocity.x;

				if(yaw < 0)
				{
					yaw += 360.0f;
				}
				if(yaw > 360.0f)
				{
					yaw -= 360.0f;
				}

				camera->SetYaw(yaw);
				camera->SetPitch(pitch);
			}
		}

		m_PreviousCurserPos = Maths::Vector2(xpos, ypos);

		m_RotateVelocity = m_RotateVelocity * pow(m_RotateDampeningFactor, dt);

		UpdateScroll(camera, Input::GetInput()->GetScrollOffset(), dt);
	}

	void EditorCameraController::HandleKeyboard(Camera* camera, float dt)
	{
		if(m_2DMode)
		{
			Maths::Vector3 up = Maths::Vector3(0, 1, 0), right = Maths::Vector3(1, 0, 0);

			m_CameraSpeed = camera->GetScale() * dt * 20.0f;

			if(Input::GetInput()->GetKeyHeld(Lumos::InputCode::Key::A))
			{
				m_Velocity -= right * m_CameraSpeed;
			}

			if(Input::GetInput()->GetKeyHeld(Lumos::InputCode::Key::D))
			{
				m_Velocity += right * m_CameraSpeed;
			}

			if(Input::GetInput()->GetKeyHeld(Lumos::InputCode::Key::W))
			{
				m_Velocity += up * m_CameraSpeed;
			}

			if(Input::GetInput()->GetKeyHeld(Lumos::InputCode::Key::S))
			{
				m_Velocity -= up * m_CameraSpeed;
			}

			if(!Maths::Equals(m_Velocity, Maths::Vector3::ZERO, Maths::Vector3(Maths::M_EPSILON)))
			{
				Maths::Vector3 position = camera->GetPosition();
				position += m_Velocity * dt;
				m_Velocity = m_Velocity * pow(m_DampeningFactor, dt);

				camera->SetPosition(position);
			}
		}
		else
		{

			float multiplier = 1000.0f;

			if(Input::GetInput()->GetKeyHeld(InputCode::Key::LeftShift))
			{
				multiplier = 10000.0f;
			}

			m_CameraSpeed = multiplier * dt;

			if(Input::GetInput()->GetMouseHeld(InputCode::MouseKey::ButtonRight))
			{
				if(Input::GetInput()->GetKeyHeld(InputCode::Key::W))
				{
					m_Velocity -= camera->GetForwardDirection() * m_CameraSpeed;
				}

				if(Input::GetInput()->GetKeyHeld(InputCode::Key::S))
				{
					m_Velocity += camera->GetForwardDirection() * m_CameraSpeed;
				}

				if(Input::GetInput()->GetKeyHeld(InputCode::Key::A))
				{
					m_Velocity -= camera->GetRightDirection() * m_CameraSpeed;
				}

				if(Input::GetInput()->GetKeyHeld(InputCode::Key::D))
				{
					m_Velocity += camera->GetRightDirection() * m_CameraSpeed;
				}

				if(Input::GetInput()->GetKeyHeld(InputCode::Key::Q))
				{
					m_Velocity -= camera->GetUpDirection() * m_CameraSpeed;
				}

				if(Input::GetInput()->GetKeyHeld(InputCode::Key::E))
				{
					m_Velocity += camera->GetUpDirection() * m_CameraSpeed;
				}
			}

			if(!Maths::Equals(m_Velocity, Maths::Vector3::ZERO, Maths::Vector3(Maths::M_EPSILON)))
			{
				Maths::Vector3 position = camera->GetPosition();
				position += m_Velocity * dt;
				camera->SetPosition(position);
				m_Velocity = m_Velocity * pow(m_DampeningFactor, dt);
			}
		}
	}

	void EditorCameraController::UpdateScroll(Camera* camera, float offset, float dt)
	{
		if(m_2DMode)
		{
			float multiplier = 2.0f;
			if(Input::GetInput()->GetKeyHeld(InputCode::Key::LeftShift))
			{
				multiplier = 10.0f;
			}

			if(offset != 0.0f)
			{
				m_ZoomVelocity += dt * offset * multiplier;
			}

			if(!Maths::Equals(m_ZoomVelocity, 0.0f))
			{
				float scale = camera->GetScale();

				scale -= m_ZoomVelocity;

				if(scale < 0.15f)
				{
					scale = 0.15f;
					m_ZoomVelocity = 0.0f;
				}
				else
				{
					m_ZoomVelocity = m_ZoomVelocity * pow(m_ZoomDampeningFactor, dt);
				}

				camera->SetScale(scale);
			}
		}
		else
		{

			if(offset != 0.0f)
			{
				m_ZoomVelocity -= dt * offset * 10.0f;
			}

			if(!Maths::Equals(m_ZoomVelocity, 0.0f))
			{
				Maths::Vector3 pos = camera->GetPosition();
				pos += camera->GetForwardDirection() * m_ZoomVelocity;
				m_ZoomVelocity = m_ZoomVelocity * pow(m_ZoomDampeningFactor, dt);

				camera->SetPosition(pos);
			}
		}
	}
}
