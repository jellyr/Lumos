﻿#include "JM.h"
#include "Physics3DComponent.h"
#include "Physics/JMPhysicsEngine/PhysicsObject3D.h"
#include "Physics/JMPhysicsEngine/JMPhysicsEngine.h"
#include "Entity/Entity.h"

namespace jm
{
	Physics3DComponent::Physics3DComponent(std::shared_ptr<PhysicsObject3D>& physics)
		: m_PhysicsObject(physics)
	{
		JMPhysicsEngine::Instance()->AddPhysicsObject(physics);
	}

	void Physics3DComponent::Init()
	{
		m_PhysicsObject->SetEntity(m_Entity);
	}

	void Physics3DComponent::OnUpdateComponent(float dt)
	{
		m_Entity->SetPosition(m_PhysicsObject->GetPosition());
	}

	void Physics3DComponent::DebugDraw(uint64 debugFlags)
	{
		m_PhysicsObject->DebugDraw(debugFlags);

		if (debugFlags & DEBUGDRAW_FLAGS_COLLISIONVOLUMES)
		{
			if (m_PhysicsObject->GetCollisionShape())
				m_PhysicsObject->GetCollisionShape()->DebugDraw(m_PhysicsObject.get());
		}
	}
}