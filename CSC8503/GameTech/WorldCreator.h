#pragma once
#include "../GameTech/GameTechRenderer.h"
#include "../CSC8503Common/GameWorld.h"
#include "../../Plugins/OpenGLRendering/OGLMesh.h"
#include "../../Plugins/OpenGLRendering/OGLShader.h"
#include "../../Plugins/OpenGLRendering/OGLTexture.h"
#include "../../Common/TextureLoader.h"
#include "../CSC8503Common/PxPhysicsSystem.h"
using namespace NCL;
using namespace CSC8503;
const float MESH_SIZE = 3.0f;
class WorldCreator {
public:
	static void Create(PxPhysicsSystem* p, GameWorld* w);
	~WorldCreator();
	static void AddPxCubeToWorld(const PxTransform& t, const PxVec3 dimensions);
	static void AddPxSphereToWorld(const PxTransform& t, const PxReal radius);
	static void AddPxCapsuleToWorld(const PxTransform& t, const PxReal radius, const PxReal halfHeight);
	static void AddPxFloorToWorld(const PxTransform& t, const PxVec3 dimensions);

	static void AddPxPickupToWorld(const PxTransform& t, const PxReal radius);
	static void AddPxPlayerToWorld(const PxTransform& t, const PxReal scale);
	static void AddPxEnemyToWorld(const PxTransform& t, const PxReal scale);

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

