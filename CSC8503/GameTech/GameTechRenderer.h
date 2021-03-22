/*			Created By Rich Davison
 *			Edited By Samuel Buzz Appleby
 *               21/01/2021
 *                170348069
 *			Game Tech Renderer Definition		 */
#pragma once
#include "../../Plugins/OpenGLRendering/OGLRenderer.h"
#include "../../Plugins/OpenGLRendering/OGLShader.h"
#include "../../Plugins/OpenGLRendering/OGLTexture.h"
#include "../../Plugins/OpenGLRendering/OGLMesh.h"
#include "../CSC8503Common/GameWorld.h"
#include "../../Common/Imgui/imgui_impl_opengl3.h"
#include "../../Common/Imgui/imgui_impl_win32.h"
#include "../CSC8503Common/PxPhysicsSystem.h"
#include "../../Common/Imgui/imgui_internal.h"
#include "../../Common/AudioManager.h"
#include <sstream>
#include "../CSC8503Common/PlayerObject.h"
namespace NCL {
	class Maths::Vector3;
	class Maths::Vector4;
	namespace CSC8503
	{
		class RenderObject;
		enum class UIState { PAUSED, MENU, OPTIONS, MODESELECT, MULTIPLAYERMENU, SOLOLEVEL1, SOLOLEVEL2, SOLOLEVEL3,
			HOSTLEVEL1, JOINLEVEL1, HOSTLEVEL2, JOINLEVEL2, HOSTLEVEL3, JOINLEVEL3, INGAME, INGAMEOPTIONS, QUIT, DEBUG };

		class GameTechRenderer : public OGLRenderer
		{
		public:
			GameTechRenderer(GameWorld& world, PxPhysicsSystem& physics);
			~GameTechRenderer();
			void InitGUI(HWND handle);
			bool TestValidHost();

			void SetUIState(UIState val) {
				levelState = val;
			}

			UIState GetUIState() {
				return levelState;
			}

			void SetSelectionObject(GameObject* val) {
				selectionObject = val;
			}

			void SetLockedObject(GameObject* val) {
				lockedObject = val;
			}

			string GetIP() const
			{
				return ipString;
			}

			string GetPort() const
			{
				return portString;
			}

			void SetPlayer(PlayerObject* val) {
				player = val;
			}
			GLuint playerTex;

		protected:
			void RenderFrame()	override;

			Matrix4 SetupDebugLineMatrix()	const override;
			Matrix4 SetupDebugStringMatrix()const override;

			OGLShader* defaultShader;

			GameWorld&	gameWorld;
			PxPhysicsSystem& pXPhysics;
			PlayerObject* player;
			void RenderUI();

			void BuildObjectList();
			void SortObjectList();
			void RenderShadowMap();
			void RenderCamera();
			void RenderSkybox();

			void LoadSkybox();

			vector<const RenderObject*> activeObjects;

			OGLShader* skyboxShader;
			OGLMesh* skyboxMesh;
			GLuint		skyboxTex;

			//shadow mapping things
			OGLShader* shadowShader;
			GLuint		shadowTex;
			GLuint		shadowFBO;
			Matrix4     shadowMatrix;

			Vector4		lightColour;
			float		lightRadius;
			Vector3		lightPosition;
			Vector3		lightDirection;


			ImFont* textFont;
			ImFont* titleFont;

			bool p_open;
			ImGuiWindowFlags window_flags;

			UIState levelState;
			GameObject* lockedObject;
			GameObject* selectionObject;

			int selectedLevel = 0;
			bool readyToJoin = false;

			string ipString = "Enter IP";
			string portString = "Enter Port";

			bool enterIP = false;
			bool enterPort = false;
		};
	}
}

