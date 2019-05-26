#include "LM.h"
#include "System.h"
#include "VFS.h"
#include "JobSystem.h"
#include "Profiler.h"
#include "Scripting/LuaScript.h"

namespace lumos
{ 
	namespace internal 
	{
	void System::Init()
	{
		LUMOS_PROFILE(system::Profiler::SetEnabled(true));
		LUMOS_PROFILE(system::Profiler::OnInit());

		LMLog::OnInit();
		system::JobSystem::OnInit();
		LUMOS_CORE_INFO("Initializing System");
		VFS::OnInit();
        LuaScript::Instance()->OnInit();
	}

	void System::Shutdown()
	{
		LUMOS_CORE_INFO("Shutting down System");
        LuaScript::Release();
		VFS::OnShutdown();
    }
} }
