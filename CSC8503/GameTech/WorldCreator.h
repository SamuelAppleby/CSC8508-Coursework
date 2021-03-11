#pragma once
#include "../GameTech/GameTechRenderer.h"
#include "../CSC8503Common/GameWorld.h"
#include "../../Plugins/OpenGLRendering/OGLMesh.h"
#include "../../Plugins/OpenGLRendering/OGLShader.h"
#include "../../Plugins/OpenGLRendering/OGLTexture.h"
#include "../../Common/TextureLoader.h"
#include "../CSC8503Common/PxPhysicsSystem.h"
#include "../Common/Light.h"
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
	static void AddPxFloorToWorld(const PxTransform& t, const PxVec3 halfSizes);

	static void AddPxSeeSawToWorld(const PxTransform& t, const PxVec3 halfSizes, float density = 10.0f, float friction = 0.5f, float elasticity = 0.1f);
	static void AddPxRevolvingDoorToWorld(const PxTransform& t, const PxVec3 halfSizes, float density = 10.0f, float friction = 0.5f, float elasticity = 0.1f);

	static void AddPxPickupToWorld(const PxTransform& t, const PxReal radius);
	static void AddPxPlayerToWorld(const PxTransform& t, const PxReal scale);
	static void AddPxEnemyToWorld(const PxTransform& t, const PxReal scale);

	static void AddLightToWorld(Vector3 position,Vector3 color, float radius = 5);
private:
	static PxPhysicsSystem* pXPhysics;
	static PxMaterial* normalMat;
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
	static OGLTexture* dogeTex;
	static OGLShader* basicShader;
	static OGLShader* toonShader;
};

