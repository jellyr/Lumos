#pragma once
#include "JM.h"
#include "Maths/Maths.h"

namespace jm
{

	enum class JM_EXPORT LightType
	{
		DirectionalLight = 0,
		SpotLight = 1,
		PointLight = 2
	};

	struct JM_EXPORT Light
	{
	public:

		Light(const maths::Vector3& position, const maths::Vector3& colour, float radius, float brightness, const LightType& type)
			: m_Position(position)
			, m_Colour(colour)
			, m_Radius(radius)
			, m_Brightness(brightness)
			, m_Type(type)
			, m_IsOn(true)
		{
		}

		Light() : m_Colour(maths::Vector3(0.4f, 0.4f, 0.4f)), m_Radius(0), m_Brightness(4.0f), m_Type(), m_IsOn(true)
		{
		}

		~Light() = default;

		maths::Vector3		GetPosition() const { return m_Position; }
		void		SetPosition(const maths::Vector3&  val) { m_Position = val; }

		maths::Vector3		GetDirection() const { return m_Direction; }
		void		SetDirection(const maths::Vector3&  val) { m_Direction = val; m_Direction.Normalise(); }

		float		GetRadius() const { return m_Radius; }
		void		SetRadius(float val) { m_Radius = val; }

		maths::Vector3		GetColour() const { return m_Colour; }
		void		SetColour(const maths::Vector3&  val) { m_Colour = val; }

		float		GetBrightness() const { return m_Brightness; }
		void		SetBrightness(float val) { m_Brightness = val; }

		LightType	GetLightType() const { return m_Type; }
		void		SetLightType(LightType val) { m_Type = val; }

		bool		GetIsOn() const { return m_IsOn; }
		void		SetIsOn(bool on) { m_IsOn = on; }

	protected:

		maths::Vector3   m_Position;
		maths::Vector3   m_Colour;
		maths::Vector3   m_Direction;
		float	  m_Radius;
		float	  m_Brightness;
		LightType m_Type;
		bool	  m_IsOn;
	};
}