#pragma once
#include "JM.h"
#include "Scripting/LuaScript.h"
#include "Scripting/Luna.h"
#include "Maths/Vector3.h"

namespace jm
{
	class JM_EXPORT Vector3Binding
	{
	public:
		maths::Vector3 vec3;

		static const char className[];
		static Luna<Vector3Binding>::FunctionType methods[];
		static Luna<Vector3Binding>::PropertyType properties[];

		Vector3Binding(const maths::Vector3& vector);
		Vector3Binding(lua_State* L);
		~Vector3Binding();

		int GetX(lua_State* L);
		int GetY(lua_State* L);
		int GetZ(lua_State* L);

		int SetX(lua_State* L);
		int SetY(lua_State* L);
		int SetZ(lua_State* L);

		int Transform(lua_State* L);
		int Length(lua_State* L);
		int Normalize(lua_State* L);
		int QuaternionNormalize(lua_State* L);
		int Clamp(lua_State* L);
		int Saturate(lua_State* L);

		int Dot(lua_State* L);
		int Cross(lua_State* L);
		int Multiply(lua_State* L);
		int Add(lua_State* L);
		int Subtract(lua_State* L);
		int Lerp(lua_State* L);
		int Print(lua_State* L);

		int QuaternionMultiply(lua_State* L);
		int QuaternionFromRollPitchYaw(lua_State* L);
		int Slerp(lua_State* L);

		static void Bind();
	};
}

