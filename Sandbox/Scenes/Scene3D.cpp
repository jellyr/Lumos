#include <Graphics/Environment.h>
#include "Scene3D.h"
#include "Graphics/MeshFactory.h"

using namespace Lumos;
using namespace Maths;

Scene3D::Scene3D(const std::string& SceneName)
	: Scene(SceneName)
{
}

Scene3D::~Scene3D()
{
}

void Scene3D::OnInit()
{
	Scene::OnInit();

	Application::Get().GetSystem<LumosPhysicsEngine>()->SetDampingFactor(0.998f);
	Application::Get().GetSystem<LumosPhysicsEngine>()->SetIntegrationType(IntegrationType::RUNGE_KUTTA_4);
	Application::Get().GetSystem<LumosPhysicsEngine>()->SetBroadphase(Lumos::CreateRef<Octree>(5, 3, Lumos::CreateRef<SortAndSweepBroadphase>()));
	Application::Get().GetSystem<LumosPhysicsEngine>()->SetPaused(false);

	LoadModels();

	LoadLuaScene("/Scripts/LuaSceneTest.lua");

	Application::Get().GetWindow()->HideMouse(false);

	m_SceneBoundingRadius = 20.0f;

	auto environment = m_EntityManager->Create();
	environment.AddComponent<Graphics::Environment>("/Textures/cubemap/Arches_E_PineTree", 11, 3072, 4096, ".tga");
	environment.AddComponent<NameComponent>("Environment");

	auto lightEntity = GetRegistry().create();
	GetRegistry().emplace<Graphics::Light>(lightEntity, Maths::Vector3(26.0f, 22.0f, 48.5f), Maths::Vector4(1.0f), 1.3f);
	GetRegistry().emplace<Maths::Transform>(lightEntity, Matrix4::Translation(Maths::Vector3(26.0f, 22.0f, 48.5f)) * Maths::Quaternion::LookAt(Maths::Vector3(26.0f, 22.0f, 48.5f), Maths::Vector3::ZERO).RotationMatrix4());
	GetRegistry().emplace<NameComponent>(lightEntity, "Light");

	auto cameraEntity = GetRegistry().create();
	Camera& camera = GetRegistry().emplace<Camera>(cameraEntity, -20.0f, -40.0f, Maths::Vector3(-31.0f, 12.0f, 51.0f), 60.0f, 0.1f, 1000.0f, (float)m_ScreenWidth / (float)m_ScreenHeight);
	GetRegistry().emplace<NameComponent>(cameraEntity, "Camera");

#ifndef LUMOS_PLATFORM_IOS
	auto shadowRenderer = new Graphics::ShadowRenderer();
	auto shadowLayer = new Layer3D(shadowRenderer);
	Application::Get().GetRenderManager()->SetShadowRenderer(shadowRenderer);
	PushLayer(shadowLayer);
#endif

	PushLayer(new Layer3D(new Graphics::SkyboxRenderer(m_ScreenWidth, m_ScreenHeight), "Skybox"));
	PushLayer(new Layer3D(new Graphics::DeferredRenderer(m_ScreenWidth, m_ScreenHeight), "Deferred"));
}

void Scene3D::OnUpdate(const TimeStep& timeStep)
{
	Scene::OnUpdate(timeStep);

	if(Input::GetInput()->GetKeyPressed(InputCode::Key::P))
		Application::Get().GetSystem<LumosPhysicsEngine>()->SetPaused(!Application::Get().GetSystem<LumosPhysicsEngine>()->IsPaused());
	if(Input::GetInput()->GetKeyPressed(InputCode::Key::P))
		Application::Get().GetSystem<B2PhysicsEngine>()->SetPaused(!Application::Get().GetSystem<B2PhysicsEngine>()->IsPaused());

	Camera* cameraComponent = nullptr;

	auto cameraView = m_EntityManager->GetEntitiesWithType<Camera>();
	if(cameraView.size() > 0)
	{
		cameraComponent = m_EntityManager->GetRegistry().template try_get<Camera>(cameraView[0]);
	}

	if(cameraComponent)
	{
		if(Input::GetInput()->GetKeyPressed(InputCode::Key::J))
			CommonUtils::AddSphere(this, cameraComponent->GetPosition(), -cameraComponent->GetForwardDirection());
		if(Input::GetInput()->GetKeyPressed(InputCode::Key::K))
			CommonUtils::AddPyramid(this, cameraComponent->GetPosition(), -cameraComponent->GetForwardDirection());
		if(Input::GetInput()->GetKeyPressed(InputCode::Key::L))
			CommonUtils::AddLightCube(this, cameraComponent->GetPosition(), -cameraComponent->GetForwardDirection());
	}
}

void Scene3D::Render2D()
{
}

void Scene3D::OnCleanupScene()
{
	if(m_CurrentScene)
	{
		Application::Get().GetSystem<LumosPhysicsEngine>()->ClearConstraints();
	}

	Scene::OnCleanupScene();
}

void Scene3D::LoadModels()
{
	const float groundWidth = 100.0f;
	const float groundHeight = 0.5f;
	const float groundLength = 100.0f;

	auto testMaterial = CreateRef<Material>();
	testMaterial->LoadMaterial("checkerboard", "/CoreTextures/checkerboard.tga");

	auto ground = GetRegistry().create();
	Ref<RigidBody3D> testPhysics = CreateRef<RigidBody3D>();
	testPhysics->SetRestVelocityThreshold(-1.0f);
	testPhysics->SetCollisionShape(CreateRef<CuboidCollisionShape>(Maths::Vector3(groundWidth, groundHeight, groundLength)));
	testPhysics->SetFriction(0.8f);
	testPhysics->SetIsAtRest(true);
	testPhysics->SetIsStatic(true);

	GetRegistry().emplace<Maths::Transform>(ground, Matrix4::Scale(Maths::Vector3(groundWidth, groundHeight, groundLength)));
	GetRegistry().emplace<Physics3DComponent>(ground, testPhysics);

	GetRegistry().emplace<Lumos::ScriptComponent>(ground, "Scripts/LuaComponentTest.lua", this);

	Ref<Graphics::Mesh> groundModel = AssetsManager::DefaultModels()->Get("Cube");
	GetRegistry().emplace<MeshComponent>(ground, groundModel);

	MaterialProperties properties;
	properties.albedoColour = Vector4(0.6f, 0.1f, 0.1f, 1.0f);
	properties.roughnessColour = Vector4(0.6f);
	properties.metallicColour = Vector4(0.15f);
	properties.usingAlbedoMap = 0.5f;
	properties.usingRoughnessMap = 0.0f;
	properties.usingNormalMap = 0.0f;
	properties.usingMetallicMap = 0.0f;
	testMaterial->SetMaterialProperites(properties);
	GetRegistry().emplace<MaterialComponent>(ground, testMaterial);

	//Create a pendulum
	auto pendulumHolder = GetRegistry().create();
	Physics3DProperties physicsProperties;
	physicsProperties.Position = Maths::Vector3(12.5f, 15.0f, 20.0f);
	physicsProperties.Mass = 1.0f;
	physicsProperties.Shape = CreateRef<CuboidCollisionShape>(Maths::Vector3(0.5f, 0.5f, 0.5f));
	physicsProperties.Friction = 0.8f;
	physicsProperties.AtRest = true;
	physicsProperties.Static = true;
	physicsProperties.Friction = 0.8f;

	Ref<RigidBody3D> pendulumHolderPhysics = CreateRef<RigidBody3D>(physicsProperties);

	GetRegistry().emplace<Physics3DComponent>(pendulumHolder, pendulumHolderPhysics);
	GetRegistry().emplace_or_replace<Maths::Transform>(pendulumHolder, Matrix4::Scale(Maths::Vector3(0.5f, 0.5f, 0.5f)));
	GetRegistry().emplace<NameComponent>(pendulumHolder, "Pendulum Holder");
	Ref<Graphics::Mesh> pendulumHolderModel = AssetsManager::DefaultModels()->Get("Cube");
	GetRegistry().emplace<MeshComponent>(pendulumHolder, pendulumHolderModel);
	GetRegistry().emplace<Lumos::ScriptComponent>(pendulumHolder, "Scripts/PlayerTest.lua", this);

	auto pendulum = GetRegistry().create();
	Ref<RigidBody3D> pendulumPhysics = CreateRef<RigidBody3D>();
	pendulumPhysics->SetFriction(0.8f);
	pendulumPhysics->SetIsAtRest(true);
	pendulumPhysics->SetInverseMass(1.0);
	pendulumPhysics->SetIsStatic(false);
	pendulumPhysics->SetPosition(Maths::Vector3(12.5f, 10.0f, 20.0f));
	pendulumPhysics->SetCollisionShape(CreateRef<SphereCollisionShape>(0.5f));
	GetRegistry().emplace<Physics3DComponent>(pendulum, pendulumPhysics);
	GetRegistry().emplace_or_replace<Maths::Transform>(pendulum, Matrix4::Scale(Maths::Vector3(0.5f, 0.5f, 0.5f)));
	GetRegistry().emplace<NameComponent>(pendulum, "Pendulum");
	Ref<Graphics::Mesh> pendulumModel = AssetsManager::DefaultModels()->Get("Sphere");
	GetRegistry().emplace<MeshComponent>(pendulum, pendulumModel);

	auto pendulumConstraint = new SpringConstraint(GetRegistry().get<Physics3DComponent>(pendulumHolder).GetRigidBody().get(), GetRegistry().get<Physics3DComponent>(pendulum).GetRigidBody().get(), GetRegistry().get<Physics3DComponent>(pendulumHolder).GetRigidBody()->GetPosition(), GetRegistry().get<Physics3DComponent>(pendulum).GetRigidBody()->GetPosition(), 0.9f, 0.5f);
	Application::Get().GetSystem<LumosPhysicsEngine>()->AddConstraint(pendulumConstraint);

#if 0
	auto soundFilePath = std::string("/Sounds/fire.ogg");
	AssetsManager::Sounds()->LoadAsset("Background", soundFilePath);

	{
		auto soundNode = Ref<SoundNode>(SoundNode::Create());
		soundNode->SetSound(AssetsManager::Sounds()->Get("Background").get());
		soundNode->SetVolume(1.0f);
		soundNode->SetPosition(Maths::Vector3(0.1f, 10.0f, 10.0f));
		soundNode->SetLooping(true);
		soundNode->SetIsGlobal(false);
		soundNode->SetPaused(false);
		soundNode->SetReferenceDistance(1.0f);
		soundNode->SetRadius(30.0f);
		GetRegistry().emplace<SoundComponent>(pendulum, soundNode);
	}
#endif

	//plastics
	int numSpheres = 0;
	for(int i = 0; i < 10; i++)
	{
		float roughness = i / 10.0f;
		Maths::Vector4 spec(0.24f);
		Vector4 diffuse(0.9f);

		Ref<Material> m = CreateRef<Material>();
		MaterialProperties properties;
		properties.albedoColour = diffuse;
		properties.roughnessColour = Vector4(roughness);
		properties.metallicColour = spec;
		properties.usingAlbedoMap = 0.0f;
		properties.usingRoughnessMap = 0.0f;
		properties.usingNormalMap = 0.0f;
		properties.usingMetallicMap = 0.0f;
		m->SetMaterialProperites(properties);

		auto sphere = GetRegistry().create();

		GetRegistry().emplace<Maths::Transform>(sphere, Matrix4::Translation(Maths::Vector3(float(i), 17.0f, 0.0f)) * Matrix4::Scale(Maths::Vector3(0.5f, 0.5f, 0.5f)));
		Ref<Graphics::Mesh> sphereModel = AssetsManager::DefaultModels()->Get("Sphere");
		GetRegistry().emplace<MeshComponent>(sphere, sphereModel);
		GetRegistry().emplace<MaterialComponent>(sphere, m);
		GetRegistry().emplace<NameComponent>(sphere, "Sphere" + StringFormat::ToString(numSpheres++));
	}

	//metals
	for(int i = 0; i < 10; i++)
	{
		float roughness = i / 10.0f;
		Vector4 spec(0.9f);
		Vector4 diffuse(0.9f);

		Ref<Material> m = CreateRef<Material>();
		MaterialProperties properties;
		properties.albedoColour = diffuse;
		properties.roughnessColour = Vector4(roughness);
		properties.metallicColour = spec;
		properties.usingAlbedoMap = 0.0f;
		properties.usingRoughnessMap = 0.0f;
		properties.usingNormalMap = 0.0f;
		properties.usingMetallicMap = 0.0f;
		m->SetMaterialProperites(properties);

		auto sphere = GetRegistry().create();

		GetRegistry().emplace<Maths::Transform>(sphere, Matrix4::Translation(Maths::Vector3(float(i), 18.0f, 0.0f)) * Matrix4::Scale(Maths::Vector3(0.5f, 0.5f, 0.5f)));
		Ref<Graphics::Mesh> sphereModel = AssetsManager::DefaultModels()->Get("Sphere");
		GetRegistry().emplace<MeshComponent>(sphere, sphereModel);
		GetRegistry().emplace<MaterialComponent>(sphere, m);
		GetRegistry().emplace<NameComponent>(sphere, "Sphere" + StringFormat::ToString(numSpheres++));
	}
}

void Scene3D::OnImGui()
{
}
