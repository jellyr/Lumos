#include "JM.h"
#include "Camera2DBinding.h"
#include "Scripting/LuaScript.h"
#include "../MathsBindings/Vector3Binding.h"
#include "Scripting/Bindings/MathsBindings/Matrix4Binding.h"

namespace jm
{
	const char Camera2DBinding::className[] = "Camera2D";

	Luna<Camera2DBinding>::FunctionType Camera2DBinding::methods[] = {
			lunamethod(Camera2DBinding, BuildViewMatrix),
			lunamethod(Camera2DBinding, GetPosition),
			lunamethod(Camera2DBinding, SetPosition),
			lunamethod(Camera2DBinding, GetPitch),
			lunamethod(Camera2DBinding, SetPitch),
			lunamethod(Camera2DBinding, GetYaw),
			lunamethod(Camera2DBinding, SetYaw),
			lunamethod(Camera2DBinding, InvertPitch),
			lunamethod(Camera2DBinding, InvertYaw),
			lunamethod(Camera2DBinding, UpdateScroll),
			lunamethod(Camera2DBinding, SetMouseSensitivity),
			lunamethod(Camera2DBinding, GetProjectionMatrix),
			lunamethod(Camera2DBinding, SetProjectionMatrix),
			lunamethod(Camera2DBinding, GetViewMatrix),
			lunamethod(Camera2DBinding, GetNear),
			lunamethod(Camera2DBinding, GetFar),
			lunamethod(Camera2DBinding, GetFOV),
			lunamethod(Camera2DBinding, Print),
			{ NULL, NULL }
	};
	Luna<Camera2DBinding>::PropertyType Camera2DBinding::properties[] = {
			{ NULL, NULL }
	};

	Camera2DBinding::Camera2DBinding(Camera2D* camera) : CameraBinding(camera)
	{
	}

	Camera2DBinding::Camera2DBinding(lua_State* L) : CameraBinding(L)
	{
		int argc = LuaScripting::SGetArgCount(L);
		float fov = 0.f, n = 0.f, f = 0.f, sr=1.f;

		if (argc > 0)
		{
			fov = LuaScripting::SGetFloat(L, 1);
			if (argc > 1)
			{
				n = LuaScripting::SGetFloat(L, 2);
				if (argc > 2)
				{
					f = LuaScripting::SGetFloat(L, 3);
					if (argc > 3)
					{
						sr = LuaScripting::SGetFloat(L, 4);
					}
				}
			}
		}
		camera = new Camera2D(fov, n, f, sr,1);
	}

	Camera2DBinding::~Camera2DBinding()
	{
	}

	void Camera2DBinding::Bind()
	{
		static bool initialized = false;
		if (!initialized)
		{
			initialized = true;
			Luna<Camera2DBinding>::Register(LuaScripting::GetGlobal()->GetLuaState());
			//LuaScripting::GetGlobal()->RunText("camera = Camera()");
		}
	}
}