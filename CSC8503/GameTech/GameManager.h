#pragma once
#include "../../Plugins/OpenGLRendering/OGLMesh.h"
#include "../../Plugins/OpenGLRendering/OGLShader.h"
#include "../../Plugins/OpenGLRendering/OGLTexture.h"
#include "../../Common/TextureLoader.h"
#include "../Common/Light.h"
#include "../../Common/Win32Window.h"

#include "../CSC8503Common/GameWorld.h"
#include "../CSC8503Common/PxPhysicsSystem.h"
#include "GameTechRenderer.h"
#include "../../Common/AudioManager.h"

#include "../CSC8503Common/Cannonball.h"
#include "../CSC8503Common/Cannon.h"
#include "../CSC8503Common/KillPlane.h"
#include "../CSC8503Common/Obstacles.h"
#include "../CSC8503Common/PlayerObject.h"

using namespace NCL;
using namespace CSC8503;
const float MESH_SIZE = 3.0f;
enum class LevelState { LEVEL1, LEVEL2, LEVEL3 };

class GameManager {
public:
	static void Create(PxPhysicsSystem* p, GameWorld* w, AudioManager* a);
	static void LoadAssets();
	static void ResetMenu();

	~GameManager();
	static void AddPxCubeToWorld(const PxTransform& t, const PxVec3 halfSizes, float density = 10.0f, float friction = 0.5f, float elasticity = 0.1f);
	static void AddPxSphereToWorld(const PxTransform& t, const PxReal radius, float density = 10.0f, float friction = 0.5f, float elasticity = 0.1f);
	static void AddPxCapsuleToWorld(const PxTransform& t, const PxReal radius, const PxReal halfHeight,
		float density = 10.0f, float friction = 0.5f, float elasticity = 0.1f);
	static void AddPxFloorToWorld(const PxTransform& t, const PxVec3 halfSizes, float friction = 0.5f, float elasticity = 0.1f);
	
	static void AddBounceSticks(const PxTransform& t, const PxReal radius, const PxReal halfHeight,
		float density = 10.0f, float friction = 0.5f, float elasticity = 0.1f);
	static void AddPxSeeSawToWorld(const PxTransform& t, const PxVec3 halfSizes, float density = 10.0f, float friction = 0.5f, float elasticity = 0.1f);
	static void AddPxRevolvingDoorToWorld(const PxTransform& t, const PxVec3 halfSizes, float density = 10.0f, float friction = 0.5f, float elasticity = 0.1f);

	static void AddPxPickupToWorld(const PxTransform& t, const PxReal radius);
	static PlayerObject* AddPxPlayerToWorld(const PxTransform& t, const PxReal scale);
	static void AddPxEnemyToWorld(const PxTransform& t, const PxReal scale);

	static void AddLightToWorld(Vector3 position,Vector3 color, float radius = 5);
	static void AddPxRotatingCubeToWorld(const PxTransform& t, const PxVec3 halfSizes, const PxVec3 rotation, float friction = 0.5f, float elasticity = 0.5);
	static Cannonball* AddPxCannonBallToWorld(const PxTransform& t, const PxReal radius = 5, const PxVec3* force = new PxVec3(0, 85000, 700000), float density = 10.0f, float friction = 0.5f, float elasticity = 0.1f);
	static void AddPxCannonToWorld(const PxTransform& t, const PxVec3 trajectory, const int shotTime, const int shotSize, PxVec3 translate = PxVec3(0,100,0));
	static void AddPxKillPlaneToWorld(const PxTransform& t, const PxVec3 halfSizes, const PxVec3 respawnCentre, Vector3 respawnSizeRange, bool hide = true);

	static GameWorld* GetWorld() {
		return world;
	}

	static Obstacles* GetObstacles()
	{
		return obstacles;
	}
	static PxPhysicsSystem* GetPhysicsSystem() {
		return pXPhysics;
	}

	static GameTechRenderer* GetRenderer() {
		return renderer;
	}

	static AudioManager* GetAudioManager() {
		return audioManager;
	}

	static CameraState GetCameraState() {
		return world->GetMainCamera()->GetState();
	}

	static void SetCamState(CameraState val) {
		world->GetMainCamera()->SetState(val);
		renderer->SetCamState(val);
	}

	static GameObject* GetLockedObject() {
		return lockedObject;
	}

	static GameObject* GetSelectionObject() {
		return selectionObject;
	}

	static void SetLockedObject(GameObject* val) {
		lockedObject = val;
		renderer->SetLockedObject(lockedObject);
	}

	static void SetSelectionObject(GameObject* val) {
		selectionObject = val;
		renderer->SetSelectionObject(selectionObject);
	}

	static void SetLevelState(LevelState val) {
		levelState = val;
	}

	static LevelState GetLevelState() {
		return levelState;
	}

	static void SetWindow(Win32Code::Win32Window* val) {
		window = val;
	}

	static Win32Code::Win32Window* GetWindow() {
		return window;
	}

private:
	static Win32Code::Win32Window* window;

	static PxPhysicsSystem* pXPhysics;
	static GameWorld* world;
	static GameTechRenderer* renderer;
	static AudioManager* audioManager;
	static Obstacles* obstacles;

	static OGLMesh* capsuleMesh;
	static OGLMesh* cubeMesh;
	static OGLMesh* sphereMesh;
	static OGLMesh* charMeshA;
	static OGLMesh* charMeshB;
	static OGLMesh* enemyMesh;
	static OGLMesh* bonusMesh;

	static OGLTexture* basicTex;
	static OGLTexture* floorTex;
	static OGLTexture* lavaTex;
	static OGLTexture* iceTex;
	static OGLTexture* trampolineTex;
	static OGLTexture* obstacleTex;
	static OGLTexture* woodenTex;
	static OGLTexture* finishTex;
	static OGLTexture* menuTex;
	static OGLTexture* plainTex;
	static OGLTexture* wallTex;
	static OGLTexture* dogeTex;

	static OGLShader* basicShader;
	static OGLShader* toonShader;

	static GameObject* selectionObject;
	static GameObject* lockedObject;

	static LevelState levelState;
};

