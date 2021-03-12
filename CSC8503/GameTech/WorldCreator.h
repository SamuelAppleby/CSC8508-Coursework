#pragma once
#include "../GameTech/GameTechRenderer.h"
#include "../CSC8503Common/GameWorld.h"
#include "../../Plugins/OpenGLRendering/OGLMesh.h"
#include "../../Plugins/OpenGLRendering/OGLShader.h"
#include "../../Plugins/OpenGLRendering/OGLTexture.h"
#include "../../Common/TextureLoader.h"
#include "../CSC8503Common/PxPhysicsSystem.h"

#include "../CSC8503Common/Cannonball.h"
#include "../CSC8503Common/Cannon.h"
#include "../CSC8503Common/RotatingCube.h"
#include "../CSC8503Common/KillPlane.h"

using namespace NCL;
using namespace CSC8503;
const float MESH_SIZE = 3.0f;
class WorldCreator {
public:
	static void Create(PxPhysicsSystem* p, GameWorld* w);
	~WorldCreator();
	static void AddPxCubeToWorld(const PxTransform& t, const PxVec3 halfSizes, float density = 10.0f, float friction = 0.5f, float elasticity = 0.1f);
	static void AddPxSphereToWorld(const PxTransform& t, const PxReal radius, float density = 10.0f, float friction = 0.5f, float elasticity = 0.1f);
	static void AddPxCapsuleToWorld(const PxTransform& t, const PxReal radius, const PxReal halfHeight,
		float density = 10.0f, float friction = 0.5f, float elasticity = 0.1f);
	static void AddPxFloorToWorld(const PxTransform& t, const PxVec3 halfSizes, float friction = 0.5f, float elasticity = 0.1f);

	static void AddPxSeeSawToWorld(const PxTransform& t, const PxVec3 halfSizes, float density = 10.0f, float friction = 0.5f, float elasticity = 0.1f);
	static void AddPxRevolvingDoorToWorld(const PxTransform& t, const PxVec3 halfSizes, float density = 10.0f, float friction = 0.5f, float elasticity = 0.1f);

	static void AddPxPickupToWorld(const PxTransform& t, const PxReal radius);
	static void AddPxPlayerToWorld(const PxTransform& t, const PxReal scale);
	static void AddPxEnemyToWorld(const PxTransform& t, const PxReal scale);

	static void AddPxRotatingCubeToWorld(const PxTransform& t, const PxVec3 halfSizes, const PxVec3* rotation = new PxVec3(0, 10, 0), float friction = 0.5f, float elasticity = 0.5, string name = "RotatingCube");
	static void AddPxCannonBallToWorld(const PxTransform& t, Cannon* cannonObj, const PxReal radius = 5, const PxVec3* force = new PxVec3(0, 85000, 700000), float density = 5.0f, float friction = 0.5f, float elasticity = 0.1f);
	static void AddPxCannonToWorld(Cannon* cannonObj);
	static void AddPxKillPlaneToWorld(const PxTransform& t, const PxVec3 halfSizes, const PxVec3 respawnCentre, Vector3 respawnSizeRange, bool hide = true, string name = "Killplane", float density = 10.0f, float friction = 0.5f, float elasticity = 0.1f);


private:
	static PxPhysicsSystem* pXPhysics;
	static GameWorld* world;
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
	static OGLShader* basicShader;
};

