/*			Created By Rich Davison
 *			Edited By Samuel Buzz Appleby
 *               21/01/2021
 *                170348069
 *			Game Tech Renderer Implementation */
#include "GameTechRenderer.h"
#include "../../Common/Imgui/imgui_internal.h"
#include "../../Common/AudioManager.h"
#include <string.h>
#include "../../Common/Assets.h"
#include "../../Common/MeshMaterial.h"

using namespace NCL;
using namespace Rendering;
using namespace CSC8503;

#define SHADOWSIZE 4096

Matrix4 biasMatrix = Matrix4::Translation(Vector3(0.5, 0.5, 0.5)) * Matrix4::Scale(Vector3(0.5, 0.5, 0.5));


GameTechRenderer::GameTechRenderer(GameWorld& world, PxPhysicsSystem& physics) :
	OGLRenderer(*Window::GetWindow()), gameWorld(world), pXPhysics(physics)
{

	glEnable(GL_DEPTH_TEST);
	shadowShader = new OGLShader("GameTechShadowVert.glsl", "GameTechShadowFrag.glsl");

	glGenTextures(1, &shadowTex);
	glBindTexture(GL_TEXTURE_2D, shadowTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOWSIZE, SHADOWSIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenFramebuffers(1, &shadowFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowTex, 0);
	glDrawBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glClearColor(1, 1, 1, 1);

	//Set up the light properties
	lightColour = Vector4(1, 1, 1, 1.0f);
	lightRadius = 1000.0f;
	lightPosition = Vector3(0.0f, 600.0f, 100);
	lightDirection = Vector3(0.0f, -1, -1.f);

	//Skybox!
	skyboxShader = new OGLShader("skyboxVertex.glsl", "skyboxFragment.glsl");
	skyboxMesh = new OGLMesh();
	skyboxMesh->SetVertexPositions({ Vector3(-1, 1,-1), Vector3(-1,-1,-1) , Vector3(1,-1,-1) , Vector3(1,1,-1) });
	skyboxMesh->SetVertexIndices({ 0,1,2,2,3,0 });
	skyboxMesh->UploadToGPU();

	LoadSkybox();
	levelState = UIState::MENU;
}

GameTechRenderer::~GameTechRenderer()
{
	glDeleteTextures(1, &shadowTex);
	glDeleteFramebuffers(1, &shadowFBO);

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void GameTechRenderer::LoadSkybox()
{
	string filenames[6] = {
		"/Cubemap/skyrender0004.png",
		"/Cubemap/skyrender0001.png",
		"/Cubemap/skyrender0003.png",
		"/Cubemap/skyrender0006.png",
		"/Cubemap/skyrender0002.png",
		"/Cubemap/skyrender0005.png"
	};

	int width[6] = { 0 };
	int height[6] = { 0 };
	int channels[6] = { 0 };
	int flags[6] = { 0 };

	vector<char*> texData(6, nullptr);

	for (int i = 0; i < 6; ++i)
	{
		TextureLoader::LoadTexture(filenames[i], texData[i], width[i], height[i], channels[i], flags[i]);
		if (i > 0 && (width[i] != width[0] || height[0] != height[0]))
		{
			std::cout << __FUNCTION__ << " cubemap input textures don't match in size?\n";
			return;
		}
	}
	glGenTextures(1, &skyboxTex);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTex);

	GLenum type = channels[0] == 4 ? GL_RGBA : GL_RGB;

	for (int i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width[i], height[i], 0, type, GL_UNSIGNED_BYTE, texData[i]);
	}

	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void GameTechRenderer::InitGUI(HWND handle)
{
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(handle);
	ImGui_ImplOpenGL3_Init("#version 130");

	titleFont = io.Fonts->AddFontFromFileTTF("../../Assets/Fonts/JosefinSans-Bold.ttf", 50.0f);
	textFont = io.Fonts->AddFontFromFileTTF("../../Assets/Fonts/JosefinSans-Regular.ttf", 15.0f);

	p_open = true;
	window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoResize;
	window_flags |= ImGuiWindowFlags_NoCollapse;
	window_flags |= ImGuiWindowFlags_AlwaysAutoResize;
	//window_flags |= ImGuiWindowFlags_NoScrollbar;
	//window_flags |= ImGuiWindowFlags_MenuBar;
	//window_flags |= ImGuiWindowFlags_NoNav;
	//window_flags |= ImGuiWindowFlags_NoBackground;
	//window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
	//window_flags |= ImGuiWindowFlags_NoTitleBar;
}

bool GameTechRenderer::TestValidHost()
{
	//std::vector<string> vect;
	//std::stringstream ss(ipString);
	//while (ss.good()) {
	//	string substr;
	//	getline(ss, substr, '.');
	//	if (substr.size() < 1)
	return false;
	/*	vect.push_back(substr);
	}
	return vect.size() == 4 && portString.length() > 0 && isdigit(portString.at(0));*/
}

void GameTechRenderer::RenderFrame()
{
	glEnable(GL_CULL_FACE);
	glClearColor(1, 1, 1, 1);
	BuildObjectList();
	SortObjectList();
	RenderShadowMap();
	RenderSkybox();
	RenderCamera();
	RenderUI();
	glDisable(GL_CULL_FACE); //Todo - text indices are going the wrong way...
}


void GameTechRenderer::RenderUI()
{
	for (int i = 0; i < 5; i++) ImGui::GetIO().MouseDown[i] = false;

	int button = -1;
	if (Window::GetMouse()->ButtonDown(MouseButtons::LEFT)) button = 0;

	if (button != -1) ImGui::GetIO().MouseDown[button] = true;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplWin32_NewFrame();
	bool* showWin = new bool(false), anotherWin;

	ImGui::NewFrame();

	static float f = 0.0f;
	static int counter = 0;
	const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
	readyToJoin = TestValidHost();
	string* activeString = enterIP ? &ipString : &portString;

	switch (levelState)
	{
	case UIState::PAUSED:
		ImGui::PushFont(titleFont);
		ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + main_viewport->Size.x / 4, main_viewport->WorkPos.x + main_viewport->Size.y / 4), ImGuiCond_Always);
		ImGui::SetNextWindowSize(ImVec2(main_viewport->Size.x / 2, main_viewport->Size.y / 2), ImGuiCond_Always);
		ImGui::Begin("PAUSED", &p_open, window_flags);
		if (ImGui::Button("Resume"))
		{
			levelState = UIState::INGAME;
		}
		if (ImGui::Button("Options"))
		{
			levelState = UIState::INGAMEOPTIONS;
		}
		if (ImGui::Button("Exit to Menu"))
		{
			levelState = UIState::MENU;
		}
		ImGui::PopFont();
		ImGui::End();
		break;
	case UIState::MENU:
		ImGui::PushFont(titleFont);
		ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x, main_viewport->WorkPos.y), ImGuiCond_Always);
		ImGui::SetNextWindowSize(ImVec2(main_viewport->Size.x, main_viewport->Size.y), ImGuiCond_Always);
		ImGui::Begin("Title Screen", &p_open, window_flags);
		if (ImGui::Button("Level 1"))
		{
			selectedLevel = 1;
			levelState = UIState::MODESELECT;
		}
		if (ImGui::Button("Level 2"))
		{
			selectedLevel = 2;
			levelState = UIState::MODESELECT;
		}
		if (ImGui::Button("Level 3"))
		{
			selectedLevel = 3;
			levelState = UIState::MODESELECT;
		}
		if (ImGui::Button("Options"))
		{
			levelState = UIState::OPTIONS;
		}
		if (ImGui::Button("Quit"))
		{
			levelState = UIState::QUIT;
		}
		ImGui::PopFont();
		ImGui::End();
		break;
	case UIState::OPTIONS:
		ImGui::PushFont(titleFont);
		ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x, main_viewport->WorkPos.y), ImGuiCond_Always);
		ImGui::SetNextWindowSize(ImVec2(main_viewport->Size.x, main_viewport->Size.y), ImGuiCond_Always);
		ImGui::Begin("Options", &p_open, window_flags);
		ImGui::Text("VOLUME");
		ImGui::SliderInt("", &(AudioManager::GetVolume()), 0, 100);
		if (ImGui::Button("Back"))
		{
			levelState = UIState::MENU;
		}
		ImGui::PopFont();
		ImGui::End();
		break;
	case UIState::INGAMEOPTIONS:
		ImGui::PushFont(titleFont);
		ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + main_viewport->Size.x / 4, main_viewport->WorkPos.x + main_viewport->Size.y / 4), ImGuiCond_Always);
		ImGui::SetNextWindowSize(ImVec2(main_viewport->Size.x / 2, main_viewport->Size.y / 2), ImGuiCond_Always);
		ImGui::Begin("Options", &p_open, window_flags);
		ImGui::Text("VOLUME");
		ImGui::SliderInt("", &(AudioManager::GetVolume()), 0, 100);
		ImGui::SetWindowFontScale(0.5);
		ImGui::TextWrapped("(Debug Mode Activated with C + H)");
		if (ImGui::Button("Back"))
		{
			levelState = UIState::PAUSED;
		}
		ImGui::PopFont();
		ImGui::End();
		break;
	case UIState::MODESELECT:
		ImGui::PushFont(titleFont);
		ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x, main_viewport->WorkPos.y), ImGuiCond_Always);
		ImGui::SetNextWindowSize(ImVec2(main_viewport->Size.x, main_viewport->Size.y), ImGuiCond_Always);
		ImGui::Begin("Play Mode", &p_open, window_flags);
		if (ImGui::Button("Single Player"))
		{
			levelState = selectedLevel == 1 ? UIState::SOLOLEVEL1 : selectedLevel == 2 ? UIState::SOLOLEVEL2 : UIState::SOLOLEVEL3;
		}
		if (ImGui::Button("Multiplayer"))
		{
			levelState = UIState::MULTIPLAYERMENU;
		}
		if (ImGui::Button("Back"))
		{
			levelState = UIState::MENU;
		}
		ImGui::PopFont();
		ImGui::End();
		break;
	case UIState::MULTIPLAYERMENU:
		ImGui::PushFont(titleFont);
		ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x, main_viewport->WorkPos.y), ImGuiCond_Always);
		ImGui::SetNextWindowSize(ImVec2(main_viewport->Size.x, main_viewport->Size.y), ImGuiCond_Always);
		ImGui::Begin("Multiplayer Menu", &p_open, window_flags);
		if (ImGui::Button("Host Game"))
		{
			levelState = selectedLevel == 1 ? UIState::HOSTLEVEL1 : UIState::HOSTLEVEL2;
		}

		if (!readyToJoin)
		{
			ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
			ImGui::Button("Join Game");
			ImGui::PopItemFlag();
			ImGui::PopStyleVar();
		}
		else
		{
			if (ImGui::Button("Join Game"))
			{
				levelState = selectedLevel == 1 ? UIState::JOINLEVEL1 : UIState::JOINLEVEL2;
			}
		}

		ImGui::Text("Host I.P:");
		ImGui::SameLine();

		if (enterIP)
		{
			ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
			ImGui::Button(ipString.c_str(), ImVec2(400, 50));
			ImGui::PopItemFlag();
			ImGui::PopStyleVar();
		}
		else
		{
			if (ImGui::Button(ipString.c_str(), ImVec2(400, 50)))
			{
				ipString.clear();
				enterPort = false;
				enterIP = true;
			}
		}
		ImGui::SameLine();
		ImGui::Text("Port No:");
		ImGui::SameLine();

		if (enterPort)
		{
			ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
			ImGui::Button(portString.c_str(), ImVec2(300, 50));
			ImGui::PopItemFlag();
			ImGui::PopStyleVar();
		}
		else
		{
			if (ImGui::Button(portString.c_str(), ImVec2(300, 50)))
			{
				portString.clear();
				enterIP = false;
				enterPort = true;
			}
		}

		/* Using hex to get keyboard inputs */
		for (int i = 0x30; i <= 0x39; ++i)
		{
			if (Window::GetKeyboard()->KeyPressed((KeyboardKeys)i))
			{
				activeString->append(std::to_string(i - 0x30));
			}
		}
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::PERIOD) && activeString == &ipString)
		{
			activeString->append(".");
		}

		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::BACK) && activeString->length() > 0)
		{
			activeString->pop_back();
		}

		if (ImGui::Button("Back"))
		{
			portString.clear();
			ipString.clear();
			enterPort = false;
			enterIP = false;
			levelState = UIState::MODESELECT;
		}
		ImGui::PopFont();
		ImGui::End();
		break;
	case UIState::INGAME:
		ImGui::PushFont(textFont);
		ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + 20, main_viewport->WorkPos.y + 20), ImGuiCond_Always);
		ImGui::Begin("Game Info", &p_open, window_flags);
		if (levelState == UIState::DEBUG)
		{
			if (gameWorld.GetShuffleObjects())
				ImGui::Text("Shuffle Objects(F1):On");
			else
				ImGui::Text("Shuffle Objects(F1):Off");
		}
		if (player) {
			ImGui::Text("Coins Collected %d", player->GetCoinsCollected());
			if(player->GetPowerUpTimer() > 0.0f)
				ImGui::Text("Powered Up!");
		}
		
		ImGui::Text("FPS Average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::PopFont();
		ImGui::End();

		ImGui::PushFont(textFont);
		ImGui::SetNextWindowPos(ImVec2(main_viewport->Size.x - 250, main_viewport->WorkPos.y + 20), ImGuiCond_Always);
		ImGui::Begin("Controls", &p_open, window_flags);
		ImGui::Text("Pause(ESC)");

		switch (gameWorld.GetMainCamera()->GetState())
		{
		case CameraState::FREE:
			ImGui::Text("Change to Global Camera[1]");
			break;
		case CameraState::GLOBAL1:
			ImGui::Text("Change to Free Camera[1]");
			break;
		case CameraState::GLOBAL2:
			ImGui::Text("Change to Free Camera[1]");
			break;
		case CameraState::THIRDPERSON:
			ImGui::Text("Change to Topdown Camera[1]");
			break;
		case CameraState::TOPDOWN:
			ImGui::Text("Change to Thirdperson Camera[1]");
			break;
		}
		ImGui::PopFont();
		ImGui::End();
		break;
	case UIState::DEBUG:
		ImGui::PushFont(textFont);
		ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + 20, main_viewport->WorkPos.y + 20), ImGuiCond_Always);
		ImGui::Begin("Game Info", &p_open, window_flags);
		if (gameWorld.GetShuffleObjects())
			ImGui::Text("Shuffle Objects(F1):On");
		else
			ImGui::Text("Shuffle Objects(F1):Off");
		if (player)
			ImGui::Text("Coins Collected %d", player->GetCoinsCollected());
		ImGui::Text("FPS Average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::PopFont();
		ImGui::End();

		ImGui::PushFont(textFont);
		ImGui::SetNextWindowPos(ImVec2(main_viewport->Size.x - 250, main_viewport->WorkPos.y + 20), ImGuiCond_Always);
		ImGui::Begin("Controls", &p_open, window_flags);
		ImGui::Text("Exit to Menu (ESC)");
		ImGui::Text("Pause(P)");

		if (!selectionObject)
		{
			ImGui::Text("Select Object (LM Click)");
		}
		else
		{
			ImGui::Text("De-Select Object (RM Click)");
			if (!lockedObject)
				ImGui::Text("Lock Selected Object (L)");
			else
				ImGui::Text("Unlock Object (L)");
		}
		ImGui::Text("Change to play mode(Q)");

		switch (gameWorld.GetMainCamera()->GetState())
		{
		case CameraState::FREE:
			ImGui::Text("Change to Global Camera[1]");
			break;
		case CameraState::GLOBAL1:
			ImGui::Text("Change to Free Camera[1]");
			break;
		case CameraState::GLOBAL2:
			ImGui::Text("Change to Free Camera[1]");
			break;
		case CameraState::THIRDPERSON:
			ImGui::Text("Change to Topdown Camera[1]");
			break;
		case CameraState::TOPDOWN:
			ImGui::Text("Change to Thirdperson Camera[1]");
			break;
		}
		ImGui::PopFont();
		ImGui::End();

		if (selectionObject)
		{
			ImGui::PushFont(textFont);
			ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + 20, main_viewport->WorkPos.y + (2.5 * main_viewport->Size.y / 3.5) - 20), ImGuiCond_Always);
			ImGui::Begin("Debug Information", &p_open, window_flags);
			ImGui::Text("Selected Object:%s", selectionObject->GetName().c_str());
			ImGui::Text("Position:%s", Vector3(selectionObject->GetTransform().GetPosition()).ToString().c_str());
			ImGui::Text("Orientation:%s", Quaternion(selectionObject->GetTransform().GetOrientation()).ToEuler().ToString().c_str());

			if (selectionObject->GetPhysicsObject() != nullptr)
			{
				if (selectionObject->GetPhysicsObject()->GetPXActor()->is<PxRigidDynamic>())
				{
					PxRigidDynamic* body = (PxRigidDynamic*)selectionObject->GetPhysicsObject()->GetPXActor();
					ImGui::Text("Linear Velocity:%s", Vector3(body->getLinearVelocity()).ToString().c_str());
					ImGui::Text("Angular Velocity:%s", Vector3(body->getAngularVelocity()).ToString().c_str());
					ImGui::Text("Mass:%.1f", body->getMass());
				}
				else
				{
					ImGui::Text("Linear Velocity:%s", Vector3(0, 0, 0).ToString().c_str());
					ImGui::Text("Angular Velocity:%s", Vector3(0, 0, 0).ToString().c_str());
					ImGui::Text("Mass:N/A");
				}

				ImGui::Text("Friction:%.1f", selectionObject->GetPhysicsObject()->GetMaterial()->getDynamicFriction());
				ImGui::Text("Elasticity:%.1f", selectionObject->GetPhysicsObject()->GetMaterial()->getRestitution());
			}
			ImGui::PopFont();
			ImGui::End();
		}

		ImGui::PushFont(textFont);
		ImGui::SetNextWindowPos(ImVec2(main_viewport->Size.x - 250, main_viewport->WorkPos.y + (5 * main_viewport->Size.y / 6) - 20), ImGuiCond_Always);
		ImGui::Begin("PhysX Information", &p_open, window_flags);
		ImGui::Text("Static Physics Objects:%d", pXPhysics.GetGScene()->getNbActors(PxActorTypeFlag::eRIGID_STATIC));
		ImGui::Text("Dynamic Physics Objects:%d", pXPhysics.GetGScene()->getNbActors(PxActorTypeFlag::eRIGID_DYNAMIC));
		ImGui::Text("Total Game Objects:%d", gameWorld.gameObjects.size());
		ImGui::Text("Current Collisions:%d", gameWorld.GetTotalCollisions());
		ImGui::PopFont();
		ImGui::End();
		break;
	}

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GameTechRenderer::BuildObjectList()
{
	activeObjects.clear();

	gameWorld.OperateOnContents(
		[&](GameObject* o)
		{
			const RenderObject* g = o->GetRenderObject();
			if (g)
			{
				activeObjects.emplace_back(g);
			}

		}
	);
}

void GameTechRenderer::SortObjectList()
{
	//Who cares!
}

void GameTechRenderer::RenderShadowMap()
{
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glViewport(0, 0, SHADOWSIZE, SHADOWSIZE);

	glCullFace(GL_FRONT);

	BindShader(shadowShader);
	int mvpLocation = glGetUniformLocation(shadowShader->GetProgramID(), "mvpMatrix");

	Matrix4 shadowViewMatrix = Matrix4::BuildViewMatrix(lightPosition, Vector3(0, 0, 0), Vector3(0, 1, 0));
	Matrix4 shadowProjMatrix = Matrix4::Perspective(100.0f, 500.0f, 1, 45.0f);

	Matrix4 mvMatrix = shadowProjMatrix * shadowViewMatrix;

	shadowMatrix = biasMatrix * mvMatrix; //we'll use this one later on

	for (const auto& i : activeObjects)
	{
		Matrix4 modelMatrix = (*i).GetTransform()->GetMatrix();
		Matrix4 mvpMatrix = mvMatrix * modelMatrix;
		glUniformMatrix4fv(mvpLocation, 1, false, (float*)&mvpMatrix);


		BindMesh((*i).GetMesh());
		int layerCount = (*i).GetMesh()->GetSubMeshCount();

		for (int i = 0; i < layerCount; ++i)
		{
			DrawBoundMesh(i);
		}
	}
	glViewport(0, 0, currentWidth, currentHeight);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glCullFace(GL_BACK);
}

void GameTechRenderer::RenderSkybox()
{
	glDisable(GL_CULL_FACE);
	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);

	float screenAspect = (float)currentWidth / (float)currentHeight;
	Matrix4 viewMatrix = gameWorld.GetMainCamera()->BuildViewMatrix();
	Matrix4 projMatrix = gameWorld.GetMainCamera()->BuildProjectionMatrix(screenAspect);

	BindShader(skyboxShader);

	int projLocation = glGetUniformLocation(skyboxShader->GetProgramID(), "projMatrix");
	int viewLocation = glGetUniformLocation(skyboxShader->GetProgramID(), "viewMatrix");
	int texLocation = glGetUniformLocation(skyboxShader->GetProgramID(), "cubeTex");

	glUniformMatrix4fv(projLocation, 1, false, (float*)&projMatrix);
	glUniformMatrix4fv(viewLocation, 1, false, (float*)&viewMatrix);

	glUniform1i(texLocation, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTex);

	BindMesh(skyboxMesh);
	DrawBoundMesh();

	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
}

void GameTechRenderer::RenderCamera()
{
	float screenAspect = (float)currentWidth / (float)currentHeight;
	Matrix4 viewMatrix = gameWorld.GetMainCamera()->BuildViewMatrix();
	Matrix4 projMatrix = gameWorld.GetMainCamera()->BuildProjectionMatrix(screenAspect);

	OGLShader* activeShader = nullptr;
	int projLocation = 0;
	int viewLocation = 0;
	int modelLocation = 0;
	int colourLocation = 0;
	int hasVColLocation = 0;
	int hasTexLocation = 0;
	int shadowLocation = 0;

	int lightPosLocation = 0;
	int lightDirLocation = 0;
	int lightColourLocation = 0;
	int lightRadiusLocation = 0;

	int cameraLocation = 0;

	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D, shadowTex);

	for (const auto& i : activeObjects)
	{
		OGLShader* shader = (OGLShader*)(*i).GetShader();
		BindShader(shader);



		if (activeShader != shader)
		{
			projLocation = glGetUniformLocation(shader->GetProgramID(), "projMatrix");
			viewLocation = glGetUniformLocation(shader->GetProgramID(), "viewMatrix");
			modelLocation = glGetUniformLocation(shader->GetProgramID(), "modelMatrix");
			shadowLocation = glGetUniformLocation(shader->GetProgramID(), "shadowMatrix");
			colourLocation = glGetUniformLocation(shader->GetProgramID(), "objectColour");
			hasVColLocation = glGetUniformLocation(shader->GetProgramID(), "hasVertexColours");
			hasTexLocation = glGetUniformLocation(shader->GetProgramID(), "hasTexture");

			lightPosLocation = glGetUniformLocation(shader->GetProgramID(), "lightPos");
			lightDirLocation = glGetUniformLocation(shader->GetProgramID(), "lightDir");
			lightColourLocation = glGetUniformLocation(shader->GetProgramID(), "lightColour");
			lightRadiusLocation = glGetUniformLocation(shader->GetProgramID(), "lightRadius");

			cameraLocation = glGetUniformLocation(shader->GetProgramID(), "cameraPos");
			Vector3 pos = gameWorld.GetMainCamera()->GetPosition();
			glUniform3fv(cameraLocation, 1, (float*)&pos);

			glUniformMatrix4fv(projLocation, 1, false, (float*)&projMatrix);
			glUniformMatrix4fv(viewLocation, 1, false, (float*)&viewMatrix);

			glUniform3fv(lightPosLocation, 1, (float*)&lightPosition);
			glUniform3fv(lightDirLocation, 1, (float*)&lightDirection);
			glUniform4fv(lightColourLocation, 1, (float*)&lightColour);
			glUniform1f(lightRadiusLocation, lightRadius);

			int shadowTexLocation = glGetUniformLocation(shader->GetProgramID(), "shadowTex");
			glUniform1i(shadowTexLocation, 1);

			activeShader = shader;
		}

		Matrix4 modelMatrix = (*i).GetTransform()->GetMatrix();
		glUniformMatrix4fv(modelLocation, 1, false, (float*)&modelMatrix);

		Matrix4 fullShadowMat = shadowMatrix * modelMatrix;
		glUniformMatrix4fv(shadowLocation, 1, false, (float*)&fullShadowMat);

		Vector4 col = i->GetColour();
		glUniform4fv(colourLocation, 1, (float*)&col);

		glUniform1i(hasVColLocation, !(*i).GetMesh()->GetColourData().empty());

		glUniform1i(hasTexLocation, (OGLTexture*)(*i).GetDefaultTexture() ? 1 : 0);

		BindMesh((*i).GetMesh());
		int layerCount = (*i).GetMesh()->GetSubMeshCount();

		vector<GLuint> mats;

		BindTextureToShader((OGLTexture*)(*i).GetDefaultTexture(), "mainTex", 0);

		if ((*i).GetMeshMaterial() != nullptr)
		{
			for (int j = 0; j < layerCount; j++)
			{
				const MeshMaterialEntry* matEntry =
					(*i).GetMeshMaterial()->GetMaterialForLayer(j);

				mats.emplace_back(playerTex);
				//BindTextureToShader((OGLTexture*)mats[j], "mainTex", 0);
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, mats[j]);
				DrawSubMesh(j);
			}
			//for (int j = 0; j < layerCount; j++)
			//{
				//glActiveTexture(GL_TEXTURE0);
				//glBindTexture(GL_TEXTURE_2D, mats[j]);
				//BindTextureToShader((OGLTexture*)playerTex, "mainTex", 0);
				//DrawSubMesh(j);

			//}

		}
		else
		{
			for (int i = 0; i < layerCount; ++i)
			{
				DrawBoundMesh(i);
			}
		}
		//BindTextureToShader()

		//for (int j = 0; j < layerCount; ++j)
		//{
			//DrawBoundMesh(j);
		//}
		//}




	}
}

Matrix4 GameTechRenderer::SetupDebugLineMatrix()	const
{
	float screenAspect = (float)currentWidth / (float)currentHeight;
	Matrix4 viewMatrix = gameWorld.GetMainCamera()->BuildViewMatrix();
	Matrix4 projMatrix = gameWorld.GetMainCamera()->BuildProjectionMatrix(screenAspect);

	return projMatrix * viewMatrix;
}

Matrix4 GameTechRenderer::SetupDebugStringMatrix()	const
{
	return Matrix4::Orthographic(-1, 1.0f, 100, 0, 0, 100);
}
