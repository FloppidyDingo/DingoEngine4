#pragma once

#define GLEW_STATIC

#include "DingoEngine4.h"
#include "DE4Types.h"
#include "Utils.h"
#include "shaders.h"

#include <iostream>
#include <fstream>
#include <algorithm>
#include <array>
#include <math.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <chrono>
#include <al.h>
#include <alc.h>
#include <Windows.h>
#include <mutex>
#include <condition_variable>

//DLL Main
#pragma region DLL Main
BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}
#pragma endregion

//Function prototypes
#pragma region prototypes
long long getMillis();
bool getShaderError(GLuint obj, GLuint check);
bool getLinkError(GLuint obj, GLuint check);
bool getAlError();
#pragma endregion

//Actual DE4 code-----------------------------------------------------------------------------------

#pragma region variables
//engine variables
bool engineRunning;
bool physicsEnabled;
bool releaseContext;
bool attachContext;
std::mutex mtx;
std::condition_variable cv;

//object management
std::vector<Entity> Entities;
unsigned int entityCount = 0;
unsigned int activeEntity;
std::vector<Animation> Animations;
unsigned int animationCount = 0;
unsigned int activeAnimation;
std::vector<TileSheet> Tilesheets;
unsigned int tileSheetCount = 0;
unsigned int activeSheet;
std::vector<Light> Lights;
unsigned int lightCount = 0;
unsigned int activeLight;
std::vector<Scene> Scenes;
unsigned int sceneCount = 0;
unsigned int activeScene;
std::vector<Trigger> Triggers;
unsigned int triggerCount = 0;
unsigned int activeTrigger;
std::vector<Sound> Sounds;
unsigned int soundCount = 0;
unsigned int activeSound;
std::vector<ALuint> Buffers;
std::vector<Text> Texts;
unsigned int textCount = 0;
unsigned int activeText;
std::vector<GameState> GameStates;
unsigned int stateCount = 0;
unsigned int activeState;

//scene management
unsigned int engineScene;
int rebuildGUI;
int rebuildEntities;
int rebuildLights;
int rebuildTriggers;
int rebuildTexts;

//physics variables
unsigned int physicsMode;
float gravity;
float terminalVelocity;
float camPos[2];
float camDir[2];
std::vector<std::array<unsigned int, 4>> noCollides;
unsigned int noCollideCount = 0;

//profiler and logging
std::string profilerInfo; //The profiler string
int fps; //Frames per second
int fpsTemp;
long long fpsStart; //FPS counter timer start
long long fpsTimer; //fps timer
int physObjects; //Number of physics checks
int drawCalls; //Number of draw calls
int textureSwaps; //Number of texture swaps
bool Profile; //Profiler enable
int frame = 0; //total number of frames
std::ofstream logFile; //log file
bool deepDebug; //log all activity?

//graphics variables
GLFWwindow* window;
int resolutionX;
int resolutionY;
int windowX;
int windowY;
int FrameRate;
float globalScale;
GLint GUIShader;
GLint ENTShader;
GLuint VAO;
GLuint VBO;
GLuint texbuffer;
GLuint opacbuffer;
GLuint lightbuffer;
std::vector<GLfloat> lighting;
std::vector<GLfloat> texData{
	0, 0,
	1, 0,
	1, 1,
	0, 1,
};
std::vector<GLfloat> vertData{
	//position
	0, 0, 0,
	0, 0, 0,
	0, 0, 0,
	0, 0, 0,
};

//Audio Variables
ALCdevice* audioDevice;

//event handler
void (*fUpdate)();
void (*fTrigger)(unsigned int trigID, unsigned int entityID);
void (*fCollision)(unsigned int idA, unsigned int idB);
void (*fKeyPressed)(const char ID[]);
void (*fKeyReleased)(const char ID[]);
void (*fKeyboardEvent)();
void (*fMousePressed)(unsigned int button, float x, float y);
void (*fMouseReleased)(unsigned int button, float x, float y);
void (*fPostPhysicsTick)();
float msex;
float msey;
bool msepress;
bool mserelease;
int msebtn;
int kbdpresses;
int kbdreleases;
bool keyBreak;
unsigned char kbdkey[128]; //list of the indexes of activated registered keys
unsigned char kbdreleasekey[128];
std::vector<KbdEvent*> keys;

//Map generation
std::string tilePath;
void (*fTagStart)(const char tag[]);
void (*fTagEnd)(const char tag[]);
void (*fTagVariable)(const char id[], const char value[]);
bool(*fEntityCreated)(unsigned int codeID);
bool(*fTriggerCreated)(unsigned int codeID);
bool(*fLightCreated)(unsigned int codeID);
std::vector<unsigned int> cleanupENTs;
std::vector<unsigned int> cleanupLGTs;
std::vector<unsigned int> cleanupTRGs;
struct objectData {
	std::string objectID;
	std::string tileID;
	unsigned int frame = 0;
	bool solid = true;
	float mass = 0;
	bool visible = true;
	float opacity = 1;
	int collision = 0;
	bool invertX = false;
	bool invertY = false;

	bool ambient = true;
	float brightness = 1;
	float red = 1;
	float green = 1;
	float blue = 1;
	float radius = 0;

	float width = 0;
	float height = 0;
};

//cinematics variables
struct keyframe {
	unsigned int code;
	unsigned int frames;
	bool manual;
	bool timed;
};
void (*cinematicCall)(unsigned int code);
std::vector<keyframe> keyframes;
bool cinematicsRunning;
bool manualWait;
bool timedWait;
unsigned int framesRemaining;
unsigned int currentKeyframe;
#pragma endregion

//Update each frame
/*
When updating:
	-Update all animations
	-populate Engine with all object IDs from scenes, place addresses in lists
	-Run Event Handler (mouse/keyboard events, update function)
	-advance cinematics timer
	-Update all objects (Animations, entities, behavior functions)
	-Apply all physics calculations and call all applicable collision functions
	-Call postPhysicsTick function
	-move camera and entities
	-check for triggers
	-Draw all objects
	-Apply Lighting
	-update title / profiler
*/
void frameUpdate() {
	//reset profiler
	profilerInfo = "";
	physObjects = 0;
	long long totalStart = getMillis();

	//run event hander
	#pragma region event handler
	long long userStart = getMillis();
	int kbdtmpprs[128];
	int kbdtmppresses = kbdpresses;
	kbdpresses = 0;
	std::copy(std::begin(kbdkey), std::end(kbdkey), std::begin(kbdtmpprs));
	int kbdtmprls[128];
	int kbdtmpreleases = kbdreleases;
	kbdreleases = 0;
	std::copy(std::begin(kbdreleasekey), std::end(kbdreleasekey), std::begin(kbdtmprls));

	if (kbdtmppresses > 0 && keys.size() > 0) {
		//parse through detected key presses
		for (int i = 0; i < kbdtmppresses; i++) {
			//pass key press to event handler
			if (fKeyPressed != nullptr) {
				fKeyPressed(keys[kbdtmpprs[i]]->id.c_str());
			}
			//break if key tables have been modified
			if (keyBreak) {
				break;
			}
			//pass key press to game state
			for (unsigned int i2 = 0; i2 < GameStates.size(); i2++) {
				if (GameStates[i2].fKeyPressed != nullptr && GameStates[i2].isEnabled()) {
					GameStates[i2].fKeyPressed(keys[kbdtmpprs[i]]->id.c_str());
				}
				if (keyBreak) {
					break;
				}
			}
			if (keyBreak) {
				break;
			}
			//log key event
			if (deepDebug) {
				logFile << getMillis() << " | Key Pressed: " << keys[kbdtmpprs[i]]->id.c_str() << std::endl;
			}
		}
	}
	if (kbdtmpreleases > 0 && keys.size() > 0) {
		for (int i = 0; i < kbdtmpreleases; i++) {
			if (fKeyReleased != nullptr) {
				fKeyReleased(keys[kbdtmprls[i]]->id.c_str());
			}
			if (keyBreak) {
				break;
			}
			for (unsigned int i2 = 0; i2 < GameStates.size(); i2++) {
				if (GameStates[i2].fKeyReleased != nullptr && GameStates[i2].isEnabled()) {
					GameStates[i2].fKeyReleased(keys[kbdtmprls[i]]->id.c_str());
				}
				if (keyBreak) {
					break;
				}
			}
			if (keyBreak) {
				break;
			}
			if (deepDebug) {
				logFile << getMillis() << " | Key Released:" << keys[kbdtmprls[i]]->id.c_str() << std::endl;
			}
		}
	}
	if (msepress) {
		if (fMousePressed != nullptr) {
			fMousePressed(msebtn, msex, msey);
		}
		for (unsigned int i = 0; i < GameStates.size(); i++) {
			if (GameStates[i].fMousePressed != nullptr && GameStates[i].isEnabled()) {
				GameStates[i].fMousePressed(msebtn, msex, msey);
			}
		}
	} else if (mserelease) {
		if (fMouseReleased != nullptr) {
			fMouseReleased(msebtn, msex, msey);
		}
		for (unsigned int i = 0; i < GameStates.size(); i++) {
			if (GameStates[i].fMouseReleased != nullptr && GameStates[i].isEnabled()) {
				GameStates[i].fMouseReleased(msebtn, msex, msey);
			}
		}
	}
	keyBreak = false;

	double msdx, msdy;
	glfwGetCursorPos(window, &msdx, &msdy);
	msex = ((float)msdx - (windowX / 2)) * ((float)resolutionX / (float)windowX) / globalScale;
	msey = -((float)msdy - (windowY / 2)) * ((float)resolutionY / (float)windowY) / globalScale;

	//call update function
	fUpdate();
	for (unsigned int i = 0; i < GameStates.size(); i++) {
		if (GameStates[i].fUpdate != nullptr && GameStates[i].isEnabled()) {
			GameStates[i].fUpdate();
		}
	}
	long long userEnd = getMillis();
	#pragma endregion

	//cinematics
	#pragma region cinematics
	if (cinematicsRunning) {
		if (framesRemaining == 0) {
			cinematicCall(keyframes[currentKeyframe].code);

			currentKeyframe++;
			if (currentKeyframe > keyframes.size() - 1) {
				cinematicsRunning = false;
			} else {
				manualWait = keyframes[currentKeyframe].manual;
				timedWait = keyframes[currentKeyframe].timed;
				framesRemaining = keyframes[currentKeyframe].frames;
			}
		} else {
			if (timedWait) {
				framesRemaining--;
			}
		}
	}
	#pragma endregion

	//rebuild scene if needed and setup data
	#pragma region setup
	long long tListStart = getMillis();

	unsigned int sceneIndex = 0;
	while (sceneIndex < Scenes.size()) {
		if (Scenes.at(sceneIndex).codeID == engineScene) {
			break;
		}
		sceneIndex++;
	}

	if (rebuildGUI > 0) {
		Scene scene;
		for (unsigned int i = rebuildGUI; i < Entities.size(); i++) {
			for (unsigned int i2 = 0; i2 < Scenes[sceneIndex].GUI.size(); i2++) {
				if (Scenes[sceneIndex].GUI[i2].id == Entities[i].codeID) {
					Scenes[sceneIndex].GUI[i2].index = i;
				}
			}
		}
		rebuildGUI = -1;
	}
	if (rebuildEntities > 0) {
		for (unsigned int i = rebuildEntities; i < Entities.size(); i++) {
			for (unsigned int i2 = 0; i2 < Scenes[sceneIndex].Entities.size(); i2++) {
				if (Scenes[sceneIndex].Entities[i2].id == Entities[i].codeID) {
					Scenes[sceneIndex].Entities[i2].index = i;
				}
			}
		}
		rebuildEntities = -1;
	}
	if (rebuildLights > 0) {
		for (unsigned int i = rebuildLights; i < Lights.size(); i++) {
			for (unsigned int i2 = 0; i2 < Scenes[sceneIndex].Lights.size(); i2++) {
				if (Scenes[sceneIndex].Lights[i2].id == Lights[i].codeID) {
					Scenes[sceneIndex].Lights[i2].index = i;
				}
			}
		}
		rebuildLights = -1;
	}
	if (rebuildTriggers > 0) {
		for (unsigned int i = rebuildTriggers; i < Triggers.size(); i++) {
			for (unsigned int i2 = 0; i2 < Scenes[sceneIndex].Triggers.size(); i2++) {
				if (Scenes[sceneIndex].Triggers[i2].id == Triggers[i].codeID) {
					Scenes[sceneIndex].Triggers[i2].index = i;
				}
			}
		}
		rebuildTriggers = -1;
	}
	if (rebuildTexts > 0) {
		for (unsigned int i = rebuildTexts; i < Texts.size(); i++) {
			for (unsigned int i2 = 0; i2 < Scenes[sceneIndex].Texts.size(); i2++) {
				if (Scenes[sceneIndex].Texts[i2].id == Texts[i].codeID) {
					Scenes[sceneIndex].Texts[i2].index = i;
				}
			}
		}
		rebuildTexts = -1;
	}

	//get current scene
	Scene scene = Scenes.at(sceneIndex);

	std::vector<entry*> physicsObjects;
	for (entry& ent : scene.Entities) {
		if (Entities[ent.index].isSolid()) {
			physicsObjects.push_back(&ent);
		}
	}

	long long tListEnd = getMillis();
	#pragma endregion

	//Update objects
	#pragma region Object update
	for (unsigned int i = 0; i < Animations.size(); i++) {
		Animations[i].update();
	}
	for (unsigned int i = 0; i < scene.Entities.size(); i++) {
		Entities[scene.Entities[i].index].update();
	}
	for (unsigned int i = 0; i < scene.GUI.size(); i++) {
		Entities[scene.GUI[i].index].update();
	}
	#pragma endregion

	//physics calculations
	#pragma region physics
	long long physicsStart = getMillis();

	if (physicsEnabled) {
		//gravity
		if (physicsMode == DE4_PLATFORMER) {
			for (entry e : scene.Entities) {
				if (Entities[e.index].dir[1] > -terminalVelocity) {
					Entities[e.index].dir[1] -= gravity * Entities[e.index].getMass();
				}
			}
		}

		//collisions and friction
		for (entry* ent1 : physicsObjects) {
			bool xFrictionApplied = false;
			bool yFrictionApplied = false;
			if (((Entities[ent1->index].dir[0] != 0) || (Entities[ent1->index].dir[1] != 0))) {
				for (entry* ent2 : physicsObjects) {
					Entity e1 = Entities[ent1->index];  //moving entity
					Entity e2 = Entities[ent2->index]; //solid entity

					/*
					Conditions for collision:
						-the solid block is within a certain radius
						-moving entity has a velocity > 0 (checked previously)
						-the code IDs do not match
						-the combination of collision groups are not in a no-collide
						-both entities are solid (implied since only solid entities are added to the physics list
					*/
					//check the noCollide list
					bool noCollide = false;
					for (std::array<unsigned int, 4> group : noCollides) {
						noCollide = ((group[1] == e1.getCollisionGroup()) && (group[2] == e2.getCollisionGroup())) || ((group[1] == e2.getCollisionGroup()) && (group[2] == e1.getCollisionGroup()));
						if (noCollide && group[3] > 0) {
							break;
						}
					}
					float moveFactor = (fabs(e1.dir[0]) + fabs(e1.dir[1])) + (e1.getWidth() / 2) + (e1.getHeight() / 2) + (e2.getWidth() / 2) + (e2.getHeight() / 2);
					//Check and exexute physics calculations
					if ((e1.codeID != e2.codeID) && !noCollide && range(e1, e2) < moveFactor) {
						/*
						Collision detection
							-Generate rectangle using moving's velocity
							-generate recatngle with solid's dimensions
							-compare to see if they intersect
							-call collision detection if hit
							-change moving's x or y velocity to not hit solid
						*/
						physObjects++;

						float ax1; //moving top left
						float ay1;
						float ax2; //moving bottom right
						float ay2;

						float bx1; //solid top left
						float by1;
						float bx2; //solid bottom right
						float by2;

						bool intersect = false;

						//solid rectangle
						bx1 = e2.x - (e2.getPhysWidth() / 2);
						by1 = e2.y + (e2.getPhysHeight() / 2);
						bx2 = e2.x + (e2.getPhysWidth() / 2);
						by2 = e2.y - (e2.getPhysHeight() / 2);

						//x intersect
						//moving rectangle
						if (e1.dir[0] > 0) {
							ax1 = e1.x - (e1.getPhysWidth() / 2);
							ax2 = e1.x + (e1.getPhysWidth() / 2) + e1.dir[0];
						} else {
							ax1 = e1.x - (e1.getPhysWidth() / 2) + e1.dir[0];
							ax2 = e1.x + (e1.getPhysWidth() / 2);
						}
						ay1 = e1.y + (e1.getPhysHeight() / 2);
						ay2 = e1.y - (e1.getPhysHeight() / 2);
						//compare rectangles
						if (ax1 < bx2 && ax2 > bx1 && ay1 > by2 && ay2 < by1) {
							//intersect detected
							intersect = true;
							if ((e1.x > e2.x)) {
								e1.dir[0] = -((e1.x - (e1.getPhysWidth() / 2)) - (e2.x + (e2.getPhysWidth() / 2)));
							} else if ((e1.x < e2.x)) {
								e1.dir[0] = (e2.x - (e2.getPhysWidth() / 2)) - (e1.x + (e1.getPhysWidth() / 2));
							}
						}

						//y intersect
						if (e1.dir[1] > 0) {
							ay1 = e1.y + (e1.getPhysHeight() / 2) + e1.dir[1];
							ay2 = e1.y - (e1.getPhysHeight() / 2);
						} else {
							ay1 = e1.y + (e1.getPhysHeight() / 2);
							ay2 = e1.y - (e1.getPhysHeight() / 2) + e1.dir[1];
						}
						ax1 = e1.x - (e1.getPhysWidth() / 2);
						ax2 = e1.x + (e1.getPhysWidth() / 2);
						//compare rectangles
						if (ax1 < bx2 && ax2 > bx1 && ay1 > by2 && ay2 < by1) {
							//intersect detected
							intersect = true;
							if (e1.y > e2.y) {
								e1.dir[1] = -((e1.y - (e1.getPhysHeight() / 2)) - (e2.y + (e2.getPhysHeight() / 2)));
							} else if (e1.y < e2.y) {
								e1.dir[1] = (e2.y - (e2.getPhysHeight() / 2)) - (e1.y + (e1.getPhysHeight() / 2));
							}
						}

						//x top friction
						if (!xFrictionApplied) {
							ax1 = e1.x - (e1.getPhysWidth() / 2);
							ax2 = e1.x + (e1.getPhysWidth() / 2);
							ay1 = e1.y + (e1.getPhysHeight() / 2) + 1;
							ay2 = e1.y - (e1.getPhysHeight() / 2);
							//check if tile is above
							if (ax1 < bx2 && ax2 > bx1 && ay1 > by2 && ay2 < by1) {
								//intersect detected
								e1.dir[0] = e1.dir[0] * e1.friction[0] * e2.friction[1];
								xFrictionApplied = true;
							}
						}
						//x bottom friction
						if (!xFrictionApplied) {
							ax1 = e1.x - (e1.getPhysWidth() / 2);
							ax2 = e1.x + (e1.getPhysWidth() / 2);
							ay1 = e1.y + (e1.getPhysHeight() / 2);
							ay2 = e1.y - (e1.getPhysHeight() / 2) - 1;
							//check if tile is below
							if (ax1 < bx2 && ax2 > bx1 && ay1 > by2 && ay2 < by1) {
								//intersect detected
								e1.dir[0] = e1.dir[0] * e1.friction[1] * e2.friction[0];
								xFrictionApplied = true;
							}
						}
						//y left friction
						if (!yFrictionApplied) {
							ax1 = e1.x - (e1.getPhysWidth() / 2) - 1;
							ax2 = e1.x + (e1.getPhysWidth() / 2);
							ay1 = e1.y + (e1.getPhysHeight() / 2);
							ay2 = e1.y - (e1.getPhysHeight() / 2);
							//check if tile is left
							if (ax1 < bx2 && ax2 > bx1 && ay1 > by2 && ay2 < by1) {
								//intersect detected
								e1.dir[1] = e1.dir[1] * e1.friction[2] * e2.friction[3];
								yFrictionApplied = true;
							}
						}
						//y right friction
						if (!yFrictionApplied) {
							ax1 = e1.x - (e1.getPhysWidth() / 2);
							ax2 = e1.x + (e1.getPhysWidth() / 2) + 1;
							ay1 = e1.y + (e1.getPhysHeight() / 2);
							ay2 = e1.y - (e1.getPhysHeight() / 2);
							//check if tile is right
							if (ax1 < bx2 && ax2 > bx1 && ay1 > by2 && ay2 < by1) {
								//intersect detected
								e1.dir[1] = e1.dir[1] * e1.friction[3] * e2.friction[2];
								yFrictionApplied = true;
							}
						}

						//apply movement clamp
						if (fabs(e1.dir[0]) < 0.01f) {
							e1.dir[0] = 0;
						}
						if (fabs(e1.dir[1]) < 0.01f) {
							e1.dir[1] = 0;
						}

						//apply modified directions to original entity
						Entities[ent1->index].dir[0] = e1.dir[0];
						Entities[ent1->index].dir[1] = e1.dir[1];

						//event if intersect
						if (fCollision != nullptr && intersect) {
							fCollision(e1.codeID, e2.codeID);
						}
						for (unsigned int i = 0; i < GameStates.size(); i++) {
							if (GameStates[i].fCollide != nullptr && GameStates[i].isEnabled() && intersect) {
								GameStates[i].fCollide(e1.codeID, e2.codeID);
							}
						}
					}
				}
			}
		}

		//movement
		for (entry e : scene.Entities) {
			Entities[e.index].x += Entities[e.index].dir[0];
			Entities[e.index].y += Entities[e.index].dir[1];
		}

		//post physics tick
		if (fPostPhysicsTick != nullptr) {
			fPostPhysicsTick();
		}
	} else {
		//check force move entities if physics is disabled
		for (entry e : scene.Entities) {
			if (Entities[e.index].forceMove) {
				Entities[e.index].x += Entities[e.index].dir[0];
				Entities[e.index].y += Entities[e.index].dir[1];
			}
			
		}
	}
	
	long long physicsEnd = getMillis();
	#pragma endregion

	//camera
	camPos[0] += camDir[0];
	camPos[1] += camDir[1];

	//trigger calculation
	#pragma region triggers
	for (entry ti : scene.Triggers) {
		Trigger t = Triggers[ti.index];
		for (entry ent : scene.Entities) {
			Entity e = Entities[ent.index];
			float el = e.x - (e.getWidth() / 2);
			float er = e.x + (e.getWidth() / 2);
			float et = e.y + (e.getHeight() / 2);
			float eb = e.y - (e.getHeight() / 2);

			float tl = t.x - (t.width / 2);
			float tr = t.x + (t.width / 2);
			float tt = t.y + (t.height / 2);
			float tb = t.y + (t.height / 2);

			if (el < tr && er > tl && et > tb && eb < tt && t.enabled) { //check if entity intersects trigger
				if (fTrigger != nullptr) {
					fTrigger(t.codeID, e.codeID);
				}
				for (unsigned int i = 0; i < GameStates.size(); i++) {
					if (GameStates[i].fTrigger != nullptr && GameStates[i].isEnabled()) {
						GameStates[i].fTrigger(t.codeID, e.codeID);
					}
				}
			}
		}
	}
	#pragma endregion

	//Graphics Engine
	#pragma region renderer
	long long drawStart = getMillis();
	drawCalls = 0;
	textureSwaps = 0;

	//Render
	#pragma region Data setup
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//camera coordinates
	float gcx = camPos[0];
	float gcy = camPos[1];
	//construct light array
	lighting.clear();
	for (entry li : scene.Lights) {
		Light l = Lights[li.index];
		lighting.push_back(l.type); //type
		lighting.push_back(l.rgb[0]); //R
		lighting.push_back(l.rgb[1]); //G
		lighting.push_back(l.rgb[2]); //B
		lighting.push_back(l.brightness); //Brightness
		lighting.push_back(l.x); //X
		lighting.push_back(l.y); //Y
		lighting.push_back(l.radius); //Radius
	}
	lighting.push_back(3);//end lighting
	
	#pragma endregion
	//assign shader
	
	glUseProgram(ENTShader);
	int prevID = -1;
	Entity ent;
	for (entry e : scene.Entities) {
		ent = Entities[e.index];
		//check if entity is in the screen and visible
		bool render = (((ent.x - gcx) + (ent.getWidth() * globalScale / 2)) > -(resolutionX / 2) && ((ent.x - gcx) + (ent.getWidth() * globalScale / 2)) < (resolutionX / 2) &&
			((ent.y - gcy) + (ent.getHeight() * globalScale / 2)) > -(resolutionY / 2) && ((ent.y - gcy) + (ent.getHeight() * globalScale / 2)) < (resolutionY / 2)) && ent.isVisible();
		if (render) 
		{
			drawCalls++;
			//check if texture ID is already in use, if not set it to the correct one
			if (ent.getTextureID() != prevID) {
				prevID = ent.getTextureID();
				textureSwaps++;
			}
			//add tile data to pass to the shader
			//process vertex data
			vertData = {
				//position
				0, 0, 0,
				0, 0, 0,
				0, 0, 0,
				0, 0, 0,
			};
			// x inversion
			if (ent.isInvertX()) {
				vertData[0] = ((ent.x - gcx) * globalScale) + (ent.getWidth() * globalScale / 2);
				vertData[3] = ((ent.x - gcx) * globalScale) - (ent.getWidth() * globalScale / 2);
				vertData[6] = ((ent.x - gcx) * globalScale) - (ent.getWidth() * globalScale / 2);
				vertData[9] = ((ent.x - gcx) * globalScale) + (ent.getWidth() * globalScale / 2);
			} else {
				vertData[0] = ((ent.x - gcx) * globalScale) - (ent.getWidth() * globalScale / 2);
				vertData[3] = ((ent.x - gcx) * globalScale) + (ent.getWidth() * globalScale / 2);
				vertData[6] = ((ent.x - gcx) * globalScale) + (ent.getWidth() * globalScale / 2);
				vertData[9] = ((ent.x - gcx) * globalScale) - (ent.getWidth() * globalScale / 2);
			}
			//y inversion
			if (ent.isInvertY()) {
				vertData[1] = ((ent.y - gcy) * globalScale) - (ent.getHeight() * globalScale / 2);
				vertData[4] = ((ent.y - gcy) * globalScale) - (ent.getHeight() * globalScale / 2);
				vertData[7] = ((ent.y - gcy) * globalScale) + (ent.getHeight() * globalScale / 2);
				vertData[10] = ((ent.y - gcy) * globalScale) + (ent.getHeight() * globalScale / 2);
			} else {
				vertData[1] = ((ent.y - gcy) * globalScale) + (ent.getHeight() * globalScale / 2);
				vertData[4] = ((ent.y - gcy) * globalScale) + (ent.getHeight() * globalScale / 2);
				vertData[7] = ((ent.y - gcy) * globalScale) - (ent.getHeight() * globalScale / 2);
				vertData[10] = ((ent.y - gcy) * globalScale) - (ent.getHeight() * globalScale / 2);
			}
			//rescale to screen coords
			vertData[0] = vertData[0] / resolutionX * 2;
			vertData[1] = vertData[1] / resolutionY * 2;
			vertData[3] = vertData[3] / resolutionX * 2;
			vertData[4] = vertData[4] / resolutionY * 2;
			vertData[6] = vertData[6] / resolutionX * 2;
			vertData[7] = vertData[7] / resolutionY * 2;
			vertData[9] = vertData[9] / resolutionX * 2;
			vertData[10] = vertData[10] / resolutionY * 2;

			//process texture atlas
			atlasTile tile = ent.getTileSheet().atlas[ent.getFrame()];
			texData = {
				tile.tlu, tile.tlv,
				tile.tru, tile.trv,
				tile.bru, tile.brv,
				tile.blu, tile.blv
				
			};

			glEnableVertexAttribArray(LOC_POS);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, vertData.size() * sizeof(GLfloat), &vertData[0], GL_DYNAMIC_DRAW);
			glVertexAttribPointer(LOC_POS, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
			//tex coords
			glEnableVertexAttribArray(LOC_TEX);
			glBindBuffer(GL_ARRAY_BUFFER, texbuffer);
			glBufferData(GL_ARRAY_BUFFER, texData.size() * sizeof(GLfloat), &texData[0], GL_STATIC_DRAW);
			glVertexAttribPointer(LOC_TEX, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
			//opacity
			float opacData = ent.getOpacity();
			glVertexAttrib1f(LOC_OPAC, opacData);
			//lighting
			glUniform1fv(glGetUniformLocation(ENTShader, "Lights"), lighting.size(), &lighting[0]);
			//bind the texture
			glActiveTexture(GL_TEXTURE0); // activate the texture unit first before binding texture (for compatibility with intel graphics)
			glBindTexture(GL_TEXTURE_2D, prevID);
			glBindVertexArray(VAO);
			//send draw call
			glDrawArrays(GL_QUADS, 0, 4);
		}
	}

	//render GUI Layer
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glUseProgram(GUIShader);
	prevID = -1;
	for (entry e : scene.GUI) {
		ent = Entities[e.index];
		if (ent.isVisible()) {
			drawCalls++;
			//check if texture ID is already in use, if not set it to the correct one
			if (ent.getTextureID() != prevID) {
				prevID = ent.getTextureID();
				textureSwaps++;
			}
			//add tile data to pass to the shader
			//process vertex data
			vertData = {
				//position
				(((ent.x * globalScale) - (ent.getWidth() * globalScale / 2))), (((ent.y * globalScale) + (ent.getHeight() * globalScale / 2))), 0,
				(((ent.x * globalScale) + (ent.getWidth() * globalScale / 2))), (((ent.y * globalScale) + (ent.getHeight() * globalScale / 2))), 0,
				(((ent.x * globalScale) + (ent.getWidth() * globalScale / 2))), (((ent.y * globalScale) - (ent.getHeight() * globalScale / 2))), 0,
				(((ent.x * globalScale) - (ent.getWidth() * globalScale / 2))), (((ent.y * globalScale) - (ent.getHeight() * globalScale / 2))), 0,
			};
			//rescale to screen coords
			vertData[0] = vertData[0] / resolutionX * 2;
			vertData[1] = vertData[1] / resolutionY * 2;
			vertData[3] = vertData[3] / resolutionX * 2;
			vertData[4] = vertData[4] / resolutionY * 2;
			vertData[6] = vertData[6] / resolutionX * 2;
			vertData[7] = vertData[7] / resolutionY * 2;
			vertData[9] = vertData[9] / resolutionX * 2;
			vertData[10] = vertData[10] / resolutionY * 2;

			//process texture atlas
			atlasTile tile = ent.getTileSheet().atlas[ent.getFrame()];
			texData = {
				tile.tlu, tile.tlv,
				tile.tru, tile.trv,
				tile.bru, tile.brv,
				tile.blu, tile.blv

			};

			glEnableVertexAttribArray(LOC_POS);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, vertData.size() * sizeof(GLfloat), &vertData[0], GL_DYNAMIC_DRAW);
			glVertexAttribPointer(LOC_POS, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
			//tex coords
			glEnableVertexAttribArray(LOC_TEX);
			glBindBuffer(GL_ARRAY_BUFFER, texbuffer);
			glBufferData(GL_ARRAY_BUFFER, texData.size() * sizeof(GLfloat), &texData[0], GL_DYNAMIC_DRAW);
			glVertexAttribPointer(LOC_TEX, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
			//opacity
			float opacData = ent.getOpacity();
			glVertexAttrib1f(LOC_OPAC, opacData);
			//bind the texture
			glActiveTexture(GL_TEXTURE0); // activate the texture unit first before binding texture (for compatibility with intel graphics)
			glBindTexture(GL_TEXTURE_2D, prevID);
			glBindVertexArray(VAO);
			//send draw call
			glDrawArrays(GL_QUADS, 0, 4);
		}
	}

	//render Text Layer
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glUseProgram(GUIShader);
	prevID = -1;
	float curx;
	float cury;
	float width;
	float height;
	for (entry e : scene.Texts) {
		Text t = Texts[e.index];
		//check if texture ID is already in use, if not set it to the correct one
		if (t.sheet.textureID != prevID) {
			prevID = t.sheet.textureID;
			textureSwaps++;
		}

		//update text data
		Texts[e.index].lineHeight = (float)t.sheet.heightList[0];
		Texts[e.index].width = 0;
		Texts[e.index].height = t.sheet.heightList[0];

		//process each character
		curx = t.x;
		cury = t.y;
		Texts[e.index].lineCount = 1;
		for (unsigned int index = 0; index < t.text.size(); index++) {
			char c = t.text.data()[index];
			if (c == '\n') {
				//newline if character is newline
				curx = t.x;
				cury -= t.lineHeight * t.scale;
				Texts[e.index].lineCount++;
				Texts[e.index].height += t.sheet.heightList[0];
			} else {
				//convert character to tile ID and determine tile attributes
				c -= 0x20;
				c += (t.font * 95);
				width = t.sheet.widthList[c] * t.scale;
				height = t.sheet.heightList[c] * t.scale;

				//draw tile
				//process verex data
				vertData = {
					//position
					(((curx * globalScale) - (width * globalScale / 2))), (((cury * globalScale) + (height * globalScale / 2))), 0,
					(((curx * globalScale) + (width * globalScale / 2))), (((cury * globalScale) + (height * globalScale / 2))), 0,
					(((curx * globalScale) + (width * globalScale / 2))), (((cury * globalScale) - (height * globalScale / 2))), 0,
					(((curx * globalScale) - (width * globalScale / 2))), (((cury * globalScale) - (height * globalScale / 2))), 0,
				};
				//rescale to screen coords
				vertData[0] = vertData[0] / resolutionX * 2;
				vertData[1] = vertData[1] / resolutionY * 2;
				vertData[3] = vertData[3] / resolutionX * 2;
				vertData[4] = vertData[4] / resolutionY * 2;
				vertData[6] = vertData[6] / resolutionX * 2;
				vertData[7] = vertData[7] / resolutionY * 2;
				vertData[9] = vertData[9] / resolutionX * 2;
				vertData[10] = vertData[10] / resolutionY * 2;
				//process texture atlas
				atlasTile tile = t.sheet.atlas[c];
				texData = {
					tile.tlu, tile.tlv,
					tile.tru, tile.trv,
					tile.bru, tile.brv,
					tile.blu, tile.blv
				};
				Texts[e.index].width += t.sheet.widthList[c];
				curx += t.sheet.widthList[c] * t.scale;
				if (t.lineWidth > 0 && curx > t.lineWidth / globalScale) {
					//newline if line width is overrun
					curx = t.x;
					cury -= t.lineHeight * t.scale;
					Texts[e.index].lineCount++;
					Texts[e.index].height += t.sheet.heightList[0];
				}
				if (t.visible) {
					//send data to GPU
					drawCalls++;
					glEnableVertexAttribArray(LOC_POS);
					glBindBuffer(GL_ARRAY_BUFFER, VBO);
					glBufferData(GL_ARRAY_BUFFER, vertData.size() * sizeof(GLfloat), &vertData[0], GL_DYNAMIC_DRAW);
					glVertexAttribPointer(LOC_POS, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
					//tex coords
					glEnableVertexAttribArray(LOC_TEX);
					glBindBuffer(GL_ARRAY_BUFFER, texbuffer);
					glBufferData(GL_ARRAY_BUFFER, texData.size() * sizeof(GLfloat), &texData[0], GL_DYNAMIC_DRAW);
					glVertexAttribPointer(LOC_TEX, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
					//opacity
					glVertexAttrib1f(LOC_OPAC, 1.0f);
					//bind the texture
					glActiveTexture(GL_TEXTURE0); // activate the texture unit first before binding texture (for compatibility with intel graphics)
					glBindTexture(GL_TEXTURE_2D, prevID);
					glBindVertexArray(VAO);
					//send draw call
					glDrawArrays(GL_QUADS, 0, 4);
				}
			}
		}

		//update width and height data for scale
		Texts[e.index].width *= Texts[e.index].scale;
		Texts[e.index].height *= Texts[e.index].scale;
		
	}

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glUseProgram(0);

	long long drawEnd = getMillis();
	#pragma endregion

	long long totalEnd = getMillis();
	//profiler
	#pragma region profiler
	if (Profile) {
		if (fpsTimer - fpsStart < 1000) {
			fpsTemp++;
			fpsTimer = getMillis();
		} else {
			fps = fpsTemp;
			fpsTemp = 0;
			fpsStart = getMillis();
			fpsTimer = fpsStart;
		}
		frame++;
		profilerInfo +=
			"Frame: " + std::to_string(frame) + //total frames
			" | TOBJ: " + std::to_string(scene.Entities.size() + scene.GUI.size()) + //Total objects
			" | POBJ: " + std::to_string(physObjects) + //Physics objects
			" | DCS: " + std::to_string(drawCalls) + //Draw Calls
			" | TSWP: " + std::to_string(textureSwaps) + //Texture Swaps
			" ||| TTOT: " + std::to_string(totalEnd - totalStart) + //Total frame time
			" | TPHYS: " + std::to_string(physicsEnd - physicsStart) + //Physics time
			" | TUSE: " + std::to_string(userEnd - userStart) + //Update and event handler time
			" | TSET: " + std::to_string(tListEnd - tListStart) + //data setup time
			" | TDRAW: " + std::to_string(drawEnd - drawStart) + //Render time
			" | FPS: " + std::to_string(fps) + //frames per second
			" | MSE: " + std::to_string(msex) + ", " + std::to_string(msey); //mouse position
		glfwSetWindowTitle(window, profilerInfo.c_str());
	}
	#pragma endregion

	//thread control
	if (attachContext) {
		glfwMakeContextCurrent(window);
		attachContext = false;
	}
	if (releaseContext) {
		std::unique_lock<std::mutex> lck(mtx);
		glfwMakeContextCurrent(NULL);
		releaseContext = false;
		cv.notify_all();
	}
}

#pragma region event handler backend
void kbdhandle(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (fKeyboardEvent != nullptr) {
		fKeyboardEvent();
	}
	for (unsigned int i = 0; i < GameStates.size(); i++) {
		if (GameStates[i].fKeyboardEvent != nullptr && GameStates[i].isEnabled()) {
			GameStates[i].fKeyboardEvent();
		}
	}
	if (action == GLFW_PRESS) {
		//search through registered keys
		for (unsigned int i = 0; i < keys.size(); i++) {
			if (keys[i]->key == key) {
				kbdkey[kbdpresses] = i;
				kbdpresses++;
			}
		}
	}
	else if (action == GLFW_RELEASE) {
		//search through registered keys
		for (unsigned int i = 0; i < keys.size(); i++) {
			if (keys[i]->key == key) {
				kbdreleasekey[kbdreleases] = i;
				kbdreleases++;
			}
		}
	}
	
}

void msehandle(GLFWwindow* window, int button, int action, int mods) {
	int msebutton = -1;
	switch (button) {
		case GLFW_MOUSE_BUTTON_LEFT: {
			msebutton = DE4_MSE_LCLICK;
			break;
		}
		case GLFW_MOUSE_BUTTON_RIGHT: {
			msebutton = DE4_MSE_RCLICK;
			break;
		}
		case GLFW_MOUSE_BUTTON_MIDDLE: {
			msebutton = DE4_MSE_MCLICK;
			break;
		}
	}
	if (msebutton != -1) {
		if (action == GLFW_PRESS) {
			if (fMousePressed != nullptr) {
				fMousePressed(msebutton, msex, msey);
			}
		}
		else if (action == GLFW_RELEASE) {
			if (fMouseReleased != nullptr) {
				fMouseReleased(msebutton, msex, msey);
			}
		}
	}
}

void windowResize(GLFWwindow* window, int width, int height){
	windowX = width;
	windowY = height;
	glViewport(0, 0, width, height);
}
#pragma endregion

//Inteface Funtions---------------------------------------------------------------------------------
#pragma region Functions
void DE4Start(bool debug, int resx, int resy, bool profile, int framerate, void (*init)(void), int argc, char** argv) {
	#pragma region Startup Code
	//start log file
	deepDebug = debug;
	logFile.open("log.txt");

	//check some things to make sure nothing will completely explode
	std::cout << "Dingo Engine 4 Version " << DE4_VERSION << "\n";
	logFile << "Dingo Engine 4 Version " << DE4_VERSION << "\n";
	if (fUpdate == NULL) {
		std::cout << "Update loop not attached!!!";
		logFile << "Update loop not attached!!!";
		logFile.close();
		exit(EXIT_FAILURE);
	}

	//initialise variables
	Profile = profile;
	resolutionX = resx;
	windowX = resx;
	windowY = resy;
	resolutionY = resy;
	FrameRate = framerate;
	globalScale = 1;
	cinematicsRunning = false;

	//start glfw
	if (!glfwInit()){
		std::cout << "GLFW failed to start!";
		logFile << "GLFW failed to start!";
		logFile.close();
		exit(EXIT_FAILURE);
	}

	if (deepDebug) {
		logFile << getMillis() << " | GLFW started" << std::endl;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);

	window = glfwCreateWindow(resx, resy, "", NULL, NULL);

	if (!window)
	{
		logFile << getMillis() << " | Window creation failed" << std::endl;
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	if (deepDebug) {
		logFile << getMillis() << " | Window created, resolution " << resx << ", " << resy << std::endl;
	}

	glfwMakeContextCurrent(window);
	attachContext = false;
	releaseContext = false;
	glfwSwapInterval(1);

	GLuint err = glewInit();
	if (err != GLEW_OK)
	{
		std::cout << "GL initialization error 0x" + err;
		logFile << "GL initialization error 0x" + err;
		logFile.close();
		exit(EXIT_FAILURE);
	}
	if (deepDebug) {
		logFile << getMillis() << " | OpenGL started" << std::endl;
	}

	//setup event handler
	glfwSetKeyCallback(window, kbdhandle);
	glfwSetMouseButtonCallback(window, msehandle);
	glfwSetFramebufferSizeCallback(window, windowResize);

	//compiler shaders
	const char* source;
	int length;
	//compile gui shader
	source = vertexSource.c_str();
	length = vertexSource.length();
	GLuint guivertex = 0;
	guivertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSourceARB(guivertex, 1, &source, &length);
	glCompileShader(guivertex);
	if (!getShaderError(guivertex, GL_COMPILE_STATUS)) {
		std::cerr << "vertex shader failed to compile" << std::endl;
		logFile << "vertex shader failed to compile" << std::endl;
		logFile.close();
		exit(EXIT_FAILURE);
	}
	source = guifragmentSource.c_str();
	length = guifragmentSource.length();
	GLuint guifragment = 0;
	guifragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSourceARB(guifragment, 1, &source, &length);
	glCompileShader(guifragment);
	if (!getShaderError(guifragment, GL_COMPILE_STATUS)) {
		std::cerr << "GUI Fragment shader failed to compile" << std::endl;
		logFile << "GUI Fragment shader failed to compile" << std::endl;
		logFile.close();
		exit(EXIT_FAILURE);
	}
	GUIShader = glCreateProgram();
	glAttachShader(GUIShader, guivertex);
	glAttachShader(GUIShader, guifragment);
	glLinkProgram(GUIShader);
	if (!getLinkError(GUIShader, GL_LINK_STATUS)) {
		std::cerr << "GUI shader failed to link" << std::endl;
		logFile << "GUI shader failed to link" << std::endl;
		logFile.close();
		exit(EXIT_FAILURE);
	}
	//compile ent shader
	//recycle vertex shader from GUI program
	source = entfragmentSource.c_str();
	length = entfragmentSource.length();
	GLuint entfragment = 0;
	entfragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSourceARB(entfragment, 1, &source, &length);
	glCompileShader(entfragment);
	if (!getShaderError(entfragment, GL_COMPILE_STATUS)) {
		std::cerr << "ENT Fragment shader failed to compile" << std::endl;
		logFile << "ENT Fragment shader failed to compile" << std::endl;
		logFile.close();
		exit(EXIT_FAILURE);
	}
	ENTShader = glCreateProgram();
	glAttachShader(ENTShader, guivertex);
	glAttachShader(ENTShader, entfragment);
	glLinkProgram(ENTShader);
	if (!getLinkError(ENTShader, GL_LINK_STATUS)) {
		std::cerr << "ENT shader failed to link" << std::endl;
		logFile << "ENT shader failed to link" << std::endl;
		logFile.close();
		exit(EXIT_FAILURE);
	}
	if (deepDebug) {
		logFile << getMillis() << " | Shader compilation succeeded" << std::endl;
	}

	//generate and bind VAO
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	//generate and bind vertex buffer
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//generate and bind texture coordinate buffer
	glGenBuffers(1, &texbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, texbuffer);
	//generate and bind opacity buffer
	glGenBuffers(1, &opacbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, opacbuffer);
	//generate and bind lighting buffer
	glGenBuffers(1, &lightbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, lightbuffer);
	//set GL texture filtering options
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	if (deepDebug) {
		logFile << getMillis() << " | GL buffers created" << std::endl;
	}

	//start audio engine
	ALCdevice* openALDevice = alcOpenDevice(nullptr);
	if (!openALDevice)
	{
		std::cerr << "OpenAL failed to start" << std::endl;
		logFile << "OpenAL failed to start" << std::endl;
		getAlError();
	}
	if (deepDebug) {
		logFile << getMillis() << " | OpenAL started" << std::endl;
	}

	//create AL context
	ALCcontext* openALContext;
	openALContext = alcCreateContext(openALDevice, nullptr);
	getAlError();

	//make context current
	ALCboolean contextMadeCurrent = false;
	contextMadeCurrent = alcMakeContextCurrent(openALContext);
	getAlError();
	if (deepDebug) {
		logFile << getMillis() << " | Audio context created" << std::endl;
	}

	settingSetMusicVolume(1);
	settingSetSFXVolume(1);

	//execute assigned init function
	init();
	int tempx, tempy;
	glfwGetFramebufferSize(window, &tempx, &tempy);
	windowResize(window, tempx, tempy);
	glfwSwapInterval(1);
	engineRunning = true;
	physicsEnabled = true;
	while (!glfwWindowShouldClose(window) && engineRunning)
	{
		frameUpdate();
		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	glfwTerminate();
	logFile << "DE4 exiting...";
	logFile.close();
	#pragma endregion
}

bool getShaderError(GLuint shader, GLuint check) {
	GLint isCompiled = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		std::vector<GLchar> errorLog(maxLength);
		glGetShaderInfoLog(shader, maxLength, &maxLength, &errorLog[0]);

		// Provide the infolog in whatever manor you deem best.
		for (std::vector<char>::const_iterator i = errorLog.begin(); i != errorLog.end(); ++i) {
			std::cout << *i;
		}

		// Exit with failure.
		glDeleteShader(shader); // Don't leak the shader.
		return 0;
	}
	return 1;
 }

bool getLinkError(GLuint shader, GLuint check) {
	GLint isCompiled = 0;
	glGetProgramiv(shader, GL_LINK_STATUS, &isCompiled);
	if (isCompiled == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetProgramiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		std::vector<GLchar> errorLog(maxLength);
		glGetProgramInfoLog(shader, maxLength, &maxLength, &errorLog[0]);

		// Provide the infolog in whatever manor you deem best.
		for (std::vector<char>::const_iterator i = errorLog.begin(); i != errorLog.end(); ++i) {
			std::cout << *i << ' ';
		}

		// Exit with failure.
		glDeleteProgram(shader); // Don't leak the shader.
		return 0;
	}
	return 1;
}

bool getAlError() {
	ALenum error = alGetError();
	if (error != AL_NO_ERROR) {
		switch (error) {
			case AL_INVALID_NAME: {
				std::cerr << "AL_INVALID_NAME: a bad name (ID) was passed to an OpenAL function" << std::endl;
				logFile << "AL_INVALID_NAME: a bad name (ID) was passed to an OpenAL function" << std::endl;
				break;
			}
			case AL_INVALID_ENUM: {
				std::cerr << "AL_INVALID_ENUM: an invalid enum value was passed to an OpenAL function" << std::endl;
				logFile << "AL_INVALID_ENUM: an invalid enum value was passed to an OpenAL function" << std::endl;
				break;
			}
			case AL_INVALID_VALUE: {
				std::cerr << "AL_INVALID_VALUE: an invalid value was passed to an OpenAL function" << std::endl;
				logFile << "AL_INVALID_VALUE: an invalid value was passed to an OpenAL function" << std::endl;
				break;
			}
			case AL_INVALID_OPERATION: {
				std::cerr << "AL_INVALID_OPERATION: the requested operation is not valid" << std::endl;
				logFile << "AL_INVALID_OPERATION: the requested operation is not valid" << std::endl;
				break;
			}
			case AL_OUT_OF_MEMORY: {
				std::cerr << "AL_OUT_OF_MEMORY: the requested operation resulted in OpenAL running out of memory" << std::endl;
				logFile << "AL_OUT_OF_MEMORY: the requested operation resulted in OpenAL running out of memory" << std::endl;
				break;
			}
			default: {
				std::cerr << "UNKNOWN AL ERROR: " << error << std::endl;
				logFile << "UNKNOWN AL ERROR: " << error << std::endl;
			}
		}
		return false;
	}
	return true;
}

long long getMillis() {
	std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(
		std::chrono::system_clock::now().time_since_epoch()
	);
	return ms.count();
}

void log(std::string log) {
	if (deepDebug) {
		logFile << getMillis() << " | " << log << std::endl;
		std::cout << getMillis() << " | " << log << std::endl;
	}
}

void error(std::string error) {
	logFile << getMillis() << "ERROR | " << error << std::endl;
	std::cerr << getMillis() << "ERROR | " << error << std::endl;
}
#pragma endregion

//DLL Implimentation ---------------------------------------------------------------------------------------
#pragma region Engine Configuration and control
void DE4SetScene(unsigned int sceneID)
{
	unsigned int i = 0;
	while (i < Scenes.size()) {
		Scene a1 = Scenes.at(i);
		if (a1.codeID == sceneID) {
			engineScene = Scenes.at(i).codeID;
			break;
		}
		i++;
	}
}

void DE4SetSFXVolume(float volume) {
	settingSetSFXVolume(volume);
	for (Sound & sound : Sounds) {
		sound.setVolume(sound.getVolume());
	}
}

float DE4GetSFXVolume() {
	return settingGetSFXVolume();
}

void DE4SetMusicVolume(float volume) {
	settingSetMusicVolume(volume);
	for (Sound& sound : Sounds) {
		sound.setVolume(sound.getVolume());
	}
}

float DE4GetMusicVolume() {
	return settingGetMusicVolume();
}

void DE4SetFullScreen(bool fullScreen) {
	//glfwSetWindowAttrib(window, GLFW_DECORATED, GLFW_TRUE);
	glfwSetWindowMonitor(window, fullScreen ? glfwGetPrimaryMonitor() : NULL, 0, 30, resolutionX, resolutionY, FrameRate);
	if (!fullScreen) {
		glfwSetWindowAttrib(window, GLFW_MAXIMIZED, GLFW_TRUE);
		glfwSwapInterval(60 / FrameRate);
	} else {
		glfwSwapInterval(1);
	}
}

void DE4SetGlobalScale(float scale) {
	globalScale = scale;
}

float DE4GetGlobalScale() {
	return globalScale;
}

void DE4SetTitle(const char title[]) {
	if (!Profile) {
		glfwSetWindowTitle(window, title);
	}
}

void DE4Exit() {
	engineRunning = false;
}

void DE4SetPhysicsEnabled(bool enabled) {
	physicsEnabled = enabled;
}

void DE4AssignThreadContext() {
	releaseContext = true;
	std::unique_lock<std::mutex> lck(mtx);
	while (releaseContext) { //wait until context released
		cv.wait(lck);
	}
	glfwMakeContextCurrent(window);
}

void DE4ReleaseThreadContext() {
	attachContext = true;
}
#pragma endregion

#pragma region camera controls
void CAMSetPostion(float cam[]) {
	camPos[0] = cam[0];
	camPos[1] = cam[1];
}

void CAMGetPosition(float cam[]) {
	cam[0] = camPos[0];
	cam[1] = camPos[1];
}

void CAMSetVector(float vec[]) {
	camDir[0] = vec[0];
	camDir[1] = vec[1];
}

void CAMGetVector(float vec[]) {
	vec[0] = camDir[0];
	vec[1] = camDir[1];
}

float CAMGetX() {
	return camPos[0];
}

float CAMGetY() {
	return camPos[1];
}

void CAMSetX(float x) {
	camPos[0] = x;
}

void CAMSetY(float y) {
	camPos[1] = y;
}

void CAMMoveX(float vx) {
	camPos[0] += vx;
}

void CAMMoveY(float vy) {
	camPos[1] += vy;
}

void CAMSetDirX(float dx) {
	camDir[0] = dx;
}

void CAMSetDirY(float dy) {
	camDir[1] = dy;
}
#pragma endregion

#pragma region Physics Functions
void PHYSetMode(unsigned int mode)
{
	physicsMode = mode;
}

void PHYSetTerminalVelocity(float vel)
{
	terminalVelocity = vel;
}

void PHYSetGravity(float grav)
{
	gravity = grav;
}

unsigned int PHYAddNoCollide(unsigned int groupA, unsigned int groupB)
{
	std::array<unsigned int, 4> nc;
	nc[0] = noCollideCount;
	noCollideCount++;
	nc[1] = groupA;
	nc[2] = groupB;
	nc[3] = 1;
	noCollides.push_back(nc);
	return nc[0];
}

void PHYRemoveNoCollide(unsigned int id)
{
	unsigned int i = 0;
	while (i < noCollides.size()) {
		if (noCollides.at(i)[0] == id) {
			noCollides.erase(noCollides.begin() + i);
			break;
		}
		i++;
	}
}

void PHYSetNoCollideEnabled(unsigned int id, bool enabled) {
	for (std::array<unsigned int, 4> &group : noCollides) {
		if (group[0] == id) {
			group[3] = enabled;
			return;
		}
	}
}

void PHYClearNoCollides()
{
	noCollides.clear();
}
#pragma endregion

#pragma region Entity Functions
void ENTAssign(unsigned int code)
{
	unsigned int i = 0;
	while (i < Entities.size()) {
		if (Entities.at(i).codeID == code) {
			activeEntity = i;
			break;
		}
		i++;
	}
}

unsigned int ENTCreate()
{
	Entity e;
	e.codeID = entityCount;
	e.friction[0] = 1;
	e.friction[1] = 1;
	e.friction[2] = 1;
	e.friction[3] = 1;
	e.dir[0] = 0;
	e.dir[1] = 0;
	Entities.push_back(e);
	entityCount++;
	ENTAssign(e.codeID);
	return e.codeID;
}

void ENTDestroy(unsigned int code)
{
	unsigned int i = 0;
	while (i < Entities.size()) {
		if (Entities.at(i).codeID == code) {
			Entities.erase(Entities.begin() + i);
			rebuildEntities = i;
			rebuildGUI = i;
			break;
		}
		i++;
	}
}

void ENTDestroyAll()
{
	Entities.clear();
	entityCount = 0;
}

float ENTGetX()
{
	return Entities[activeEntity].x;
}

float ENTGetY()
{
	return Entities[activeEntity].y;
}

float ENTGetWidth()
{
	return Entities[activeEntity].getWidth();
}

float ENTGetHeight()
{
	return Entities[activeEntity].getHeight();
}

void ENTSetX(float x)
{
	Entities[activeEntity].x = x;
}

void ENTSetY(float y)
{
	Entities[activeEntity].y = y;
}

void ENTSetPosition(float x, float y) {
	Entities[activeEntity].x = x;
	Entities[activeEntity].y = y;
}

void ENTSetPosition(unsigned int ent){
	unsigned int i = 0;
	while (i < Entities.size()) {
		if (Entities.at(i).codeID == ent) {
			Entities[activeEntity].x = Entities.at(i).x;
			Entities[activeEntity].y = Entities.at(i).y;
			break;
		}
		i++;
	}
}

void ENTGetDir(float vec[])
{
	vec[0] = Entities[activeEntity].dir[0];
	vec[1] = Entities[activeEntity].dir[1];
}

float ENTGetDirX() {
	return Entities[activeEntity].dir[0];
}

float ENTGetDirY() {
	return Entities[activeEntity].dir[1];
}

void ENTSetDir(float vec[])
{
	Entities[activeEntity].dir[0] = vec[0];
	Entities[activeEntity].dir[1] = vec[1];
}

void ENTSetDir(float dx, float dy) {
	Entities[activeEntity].dir[0] = dx;
	Entities[activeEntity].dir[1] = dy;
}

void ENTSetDirX(float dirx) {
	Entities[activeEntity].dir[0] = dirx;
}

void ENTSetDirY(float diry) {
	Entities[activeEntity].dir[1] = diry;
}

void ENTSetForceMove(bool force) {
	Entities[activeEntity].forceMove = force;
}

void ENTSetFrictionProfile(float top, float bottom, float left, float right) {
	Entities[activeEntity].friction[0] = top;
	Entities[activeEntity].friction[1] = bottom;
	Entities[activeEntity].friction[2] = left;
	Entities[activeEntity].friction[3] = right;
}

void ENTApplyForce(float dx, float dy) {
	Entities[activeEntity].dir[0] = Entities[activeEntity].dir[0] + dx;
	Entities[activeEntity].dir[1] = Entities[activeEntity].dir[1] + dy;
}

void ENTApplyForce(float vec[]) {
	Entities[activeEntity].dir[0] = Entities[activeEntity].dir[0] + vec[0];
	Entities[activeEntity].dir[1] = Entities[activeEntity].dir[1] + vec[1];
}

void ENTSetTileSheet()
{
	Entities[activeEntity].setTileSheet(Tilesheets[activeSheet]);
}

void ENTAddAnimation(unsigned int aniid)
{
	unsigned int i = 0;
	while (i < Animations.size()) {
		if (Animations.at(i).codeID == aniid) {
			Entities[activeEntity].addAnimation(Animations.at(i));
			break;
		}
		i++;
	}
}

void ENTSetTileSheet(unsigned int tileid)
{
	unsigned int i = 0;
	while (i < Tilesheets.size()) {
		if (Tilesheets.at(i).codeID == tileid) {
			Entities[activeEntity].setTileSheet(Tilesheets.at(i));
			break;
		}
		i++;
	}
}

void ENTAddAnimation()
{
	Entities[activeEntity].addAnimation(Animations[activeAnimation]);
}

void ENTRemoveAnimation(unsigned int aniid)
{
	unsigned int i = 0;
	while (i < Animations.size()) {
		if (Animations.at(i).codeID == aniid) {
			Entities[activeEntity].removeAnimation(Animations.at(i));
			break;
		}
		i++;
	}
}

void ENTRemoveAnimation()
{
	Entities[activeEntity].removeAnimation(activeAnimation);
}

void ENTClearAnimations()
{
	Entities[activeEntity].clearAnimations();
}

void ENTStopAllAnimations() {
	Entities[activeEntity].stopAllAnimations();
}

void ENTSetFrame(unsigned int frame)
{
	Entities[activeEntity].setFrame(frame);
}

void ENTSetCollisionGroup(unsigned int group)
{
	Entities[activeEntity].setCollisionGroup(group);
}

unsigned int ENTGetCollisionGroup()
{
	return Entities[activeEntity].getCollisionGroup();
}

void ENTSetMass(float mass)
{
	Entities[activeEntity].setMass(mass);
}

float ENTGetMass()
{
	return Entities[activeEntity].getMass();
}

void ENTSetVisible(bool vis)
{
	Entities[activeEntity].setVisible(vis);
}

bool ENTIsVisible()
{
	return Entities[activeEntity].isVisible();
}

void ENTSetSolid(bool solid)
{
	Entities[activeEntity].setSolid(solid);
}

bool ENTIsSolid()
{
	return Entities[activeEntity].isSolid();
}

void ENTSetScale(float scale)
{
	Entities[activeEntity].setScale(scale);
}

void ENTSetScaleX(float scale) {
	Entities[activeEntity].setScaleX(scale);
}

void ENTSetScaleY(float scale) {
	Entities[activeEntity].setScaleY(scale);
}

float ENTGetScale()
{
	return Entities[activeEntity].getScale();
}

float ENTGetScaleX() {
	return Entities[activeEntity].getScaleX();
}

float ENTGetScaleY() {
	return Entities[activeEntity].getScaleY();
}

void ENTSetOpacity(float opac)
{
	Entities[activeEntity].setOpacity(opac);
}

float ENTGetOpacity()
{
	return Entities[activeEntity].getOpacity();
}

char* ENTGetID()
{
	char* charID = new char[Entities[activeEntity].getID().size() + 1];
	strcpy_s(charID, Entities[activeEntity].getID().size() + 1, Entities[activeEntity].getID().c_str());
	return charID;
}

void ENTSetID(const char id[])
{
	Entities[activeEntity].setID(std::string(id));
}

void ENTSetInvertX(bool invert)
{
	Entities[activeEntity].setInvertX(invert);
}

void ENTSetInvertY(bool invert)
{
	Entities[activeEntity].setInvertY(invert);
}

bool ENTIsInvertX()
{
	return Entities[activeEntity].isInvertX();
}

bool ENTIsInvertY()
{
	return Entities[activeEntity].isInvertY();
}

unsigned int ENTGetIDCode()
{
	return Entities[activeEntity].codeID;
}

void ENTOverridePhysicsSize(float width, float height) {
	Entities[activeEntity].setPhysWidth(width);
	Entities[activeEntity].setPhysHeight(height);
	Entities[activeEntity].setPhysOverride(true);
}

void ENTOverridePhysicsSize() {
	Entities[activeEntity].setPhysWidth(Entities[activeEntity].getWidth());
	Entities[activeEntity].setPhysHeight(Entities[activeEntity].getHeight());
	Entities[activeEntity].setPhysOverride(true);
}

void ENTDisablePhysicsOverride() {
	Entities[activeEntity].setPhysOverride(false);
}

bool ENTInFrame() {
	Entity ent = Entities[activeEntity];
	float gcx = camPos[0];
	float gcy = camPos[1];
	return (((ent.x - gcx) + (ent.getWidth() * globalScale / 2)) > -(resolutionX / 2) && ((ent.x - gcx) + (ent.getWidth() * globalScale / 2)) < (resolutionX / 2) &&
		((ent.y - gcy) + (ent.getHeight() * globalScale / 2)) > -(resolutionY / 2) && ((ent.y - gcy) + (ent.getHeight() * globalScale / 2)) < (resolutionY / 2));
}
#pragma endregion

#pragma region Scene Functions
void SCNAssign(unsigned int code)
{
	unsigned int i = 0;
	while (i < Scenes.size()) {
		if (Scenes.at(i).codeID == code) {
			activeScene = i;
			break;
		}
		i++;
	}
}

unsigned int SCNCreate()
{
	Scene s;
	s.codeID = sceneCount;
	Scenes.push_back(s);
	sceneCount++;
	SCNAssign(s.codeID);
	return s.codeID;
}

void SCNDestroy(unsigned int code)
{
	unsigned int i = 0;
	while (i < Scenes.size()) {
		if (Scenes.at(i).codeID == code) {
			Scenes.erase(Scenes.begin() + i);
			break;
		}
		i++;
	}
}

void SCNDestroyAll()
{
	Scenes.clear();
	sceneCount = 0;
}

void SCNAddEntity()
{
	Scenes[activeScene].addEntity(Entities[activeEntity].codeID, activeEntity);
}

void SCNAddEntity(unsigned int entCode)
{
	unsigned int i = 0;
	while (i < Entities.size()) {
		if (Entities.at(i).codeID == entCode) {
			Scenes[activeScene].addEntity(Entities.at(i).codeID, i);
			break;
		}
		i++;
	}
}

void SCNAddLight()
{
	Scenes[activeScene].addLight(Lights[activeLight].codeID, activeLight);
	if (Scenes[activeScene].Lights.size() > MAX_LIGHTS) {
		std::cerr << "Maximum number of lights exceeded!";
		exit(EXIT_FAILURE);
	}
}

void SCNAddLight(unsigned int lightCode)
{
	unsigned int i = 0;
	while (i < Lights.size()) {
		if (Lights.at(i).codeID == lightCode) {
			Scenes[activeScene].addLight(Lights.at(i).codeID, i);
			break;
		}
		i++;
	}
	if (Scenes[activeScene].Lights.size() > MAX_LIGHTS) {
		std::cerr << "Maximum number of lights exceeded!";
		exit(EXIT_FAILURE);
	}
}

void SCNAddGUIEntity()
{
	Scenes[activeScene].addGUI(Entities[activeEntity].codeID, activeEntity);
}

void SCNAddGUIEntity(unsigned int entCode)
{
	unsigned int i = 0;
	while (i < Entities.size()) {
		if (Entities.at(i).codeID == entCode) {
			Scenes[activeScene].addGUI(entCode, i);
			break;
		}
		i++;
	}
}

void SCNAddGUIText(unsigned int textCode) {
	unsigned int i = 0;
	while (i < Texts.size()) {
		if (Texts.at(i).codeID == textCode) {
			Scenes[activeScene].addGUIText(textCode, i);
			break;
		}
		i++;
	}
}

void SCNAddGUIText() {
	Scenes[activeText].addGUIText(Texts[activeText].codeID, activeText);
}

void SCNAddTrigger()
{
	Scenes[activeScene].addTrigger(Triggers[activeTrigger].codeID, activeTrigger);
}

void SCNAddTrigger(unsigned int trigCode)
{
	unsigned int i = 0;
	while (i < Triggers.size()) {
		if (Triggers.at(i).codeID == trigCode) {
			Scenes[activeScene].addTrigger(Triggers.at(i).codeID, i);
			break;
		}
		i++;
	}
}

void SCNRemoveEntity()
{
	Scenes[activeScene].removeEntity(Entities[activeEntity].codeID);
}

void SCNRemoveEntity(unsigned int entCode)
{
	Scenes[activeScene].removeEntity(entCode);
}

void SCNRemoveLight()
{
	Scenes[activeScene].removeLight(Lights[activeLight].codeID);
}

void SCNRemoveLight(unsigned int lightCode)
{
	Scenes[activeScene].removeLight(lightCode);
}

void SCNRemoveGUIEntity()
{
	Scenes[activeScene].removeGUI(Entities[activeEntity].codeID);
}

void SCNRemoveGUIEntity(unsigned int entCode)
{
	Scenes[activeScene].removeGUI(entCode);
}

void SCNRemoveGUIText(unsigned int textCode) {
	Scenes[activeScene].removeGUIText(textCode);
}

void SCNRemoveGUIText() {
	Scenes[activeScene].removeGUIText(Texts[activeText].codeID);
}

void SCNRemoveTrigger()
{
	Scenes[activeScene].removeTrigger(Triggers[activeTrigger].codeID);
}

void SCNRemoveTrigger(unsigned int trigCode)
{
	Scenes[activeScene].removeTrigger(trigCode);
}

void SCNClearEntities()
{
	Scenes[activeScene].clearEntities();
}

void SCNClearGUI()
{
	Scenes[activeScene].clearGUI();
}

void SCNClearLights()
{
	Scenes[activeScene].clearLights();
}

void SCNClearTriggers()
{
	Scenes[activeScene].clearTriggers();
}

void SCNClearAll()
{
	Scenes[activeScene].clearEntities();
	Scenes[activeScene].clearGUI();
	Scenes[activeScene].clearLights();
	Scenes[activeScene].clearTriggers();
	Scenes[activeScene].clearGUIText();

}
#pragma endregion

#pragma region Animation Functions
void ANIAssign(unsigned int code)
{
	unsigned int i = 0;
	while (i < Animations.size()) {
		if (Animations.at(i).codeID == code) {
			activeAnimation = i;
			break;
		}
		i++;
	}
}

unsigned int ANICreate()
{
	Animation an;
	an.codeID = animationCount;
	animationCount++;
	Animations.push_back(an);
	ANIAssign(an.codeID);
	return an.codeID;
}

void ANIDestroy(unsigned int code)
{
	unsigned int i = 0;
	while (i < Animations.size()) {
		if (Animations.at(i).codeID == code) {
			Animations.erase(Animations.begin() + i);
			break;
		}
		i++;
	}
}

void ANIDestroyAll()
{
	Animations.clear();
	animationCount = 0;
}

void ANISetID(const char id[])
{
	Animations[activeAnimation].setID(std::string(id));
}

char* ANIGetID()
{
	char* charID = new char[Animations[activeAnimation].getID().size() + 1];
	strcpy_s(charID, Animations[activeAnimation].getID().size() + 1, Animations[activeAnimation].getID().c_str());
	return charID;
}

void ANISetFrameSpacing(unsigned int space)
{
	Animations[activeAnimation].setFrameSpacing(space);
}

unsigned int ANIGetFrameSpacing()
{
	return Animations[activeAnimation].getFrameSpacing();
}

void ANISetRunning(bool running)
{
	Animations[activeAnimation].setRunning(running);
}

bool ANIIsRunning()
{
	return Animations[activeAnimation].isRunning();
}

void ANISetSinglePlay(bool single)
{
	Animations[activeAnimation].setSinglePlay(single);
}

bool ANIIsSinglePlay()
{
	return Animations[activeAnimation].isSinglePlay();
}

unsigned int ANIGetCurrentFrame()
{
	return Animations[activeAnimation].getCurrentFrame();
}

unsigned int ANIGetCurrentTile()
{
	return Animations[activeAnimation].getCurrentTile();
}

void ANIAddFrame(unsigned int frame)
{
	Animations[activeAnimation].addFrame(frame);
}

void ANIClearFrames()
{
	Animations[activeAnimation].clearFrames();
}
#pragma endregion

#pragma region tilesheet functions
void TLSAssign(unsigned int codeID)
{
	unsigned int i = 0;
	while (i < Tilesheets.size()) {
		if (Tilesheets.at(i).codeID == codeID) {
			activeSheet = i;
			break;
		}
		i++;
	}
}

unsigned int TLSCreate()
{
	TileSheet s;
	s.codeID = tileSheetCount;
	tileSheetCount++;
	Tilesheets.push_back(s);
	TLSAssign(s.codeID);
	return s.codeID;
}

void TLSDestroy(unsigned int codeID)
{
	unsigned int i = 0;
	while (i < Tilesheets.size()) {
		if (Tilesheets.at(i).codeID == codeID) {
			Tilesheets.erase(Tilesheets.begin() + i);
			break;
		}
		i++;
	}
}

void TLSDestroyAll()
{
	Tilesheets.clear();
	tileSheetCount = 0;
}

void TLSGenSheet(const char url_base[], const char url_def[])
{
	Tilesheets[activeSheet].createSheet(std::string(url_base), std::string(url_def));
}

float TLSGetFrameWidth(unsigned int frame) {
	return Tilesheets[activeSheet].widthList[frame];
}

float TLSGetFrameHeight(unsigned int frame) {
	return Tilesheets[activeSheet].heightList[frame];
}
#pragma endregion

#pragma region Light functions
void LGTAssign(unsigned int codeID)
{
	unsigned int i = 0;
	while (i < Lights.size()) {
		if (Lights.at(i).codeID == codeID) {
			activeLight = i;
			break;
		}
		i++;
	}
}

unsigned int LGTCreate()
{
	Light l;
	l.codeID = lightCount;
	lightCount++;
	Lights.push_back(l);
	LGTAssign(l.codeID);
	return l.codeID;
}

void LGTDestroy(unsigned int codeID)
{
	unsigned int i = 0;
	while (i < Lights.size()) {
		if (Lights.at(i).codeID == codeID) {
			Lights.erase(Lights.begin() + i);
			rebuildLights = i;
			break;
		}
		i++;
	}
}

void LGTDestroyAll()
{
	Lights.clear();
	lightCount = 0;
}

void LGTSetX(float x) {
	Lights[activeLight].x = x;
}

void LGTSetY(float y) {
	Lights[activeLight].y = y;
}

void LGTSetPosition(float x, float y) {
	Lights[activeLight].x = x;
	Lights[activeLight].y = y;
}

void LGTSetType(unsigned int type)
{
	Lights[activeLight].type = (float)type;
}

unsigned int LGTGetType()
{
	return (unsigned int)Lights[activeLight].type;
}

void LGTSetRadius(float radius)
{
	Lights[activeLight].radius = radius;
}

float LGTGetRadius()
{
	return Lights[activeLight].radius;
}

void LGTSetBrightness(float bright)
{
	Lights[activeLight].brightness = bright;
}

float LGTGetBrightness()
{
	return Lights[activeLight].brightness;
}

void LGTSetColor(float red, float green, float blue)
{
	Lights[activeLight].rgb[0] = red;
	Lights[activeLight].rgb[1] = green;
	Lights[activeLight].rgb[2] = blue;
}

void LGTSetID(const char id[]) {
	Lights[activeLight].setID(std::string(id));
}

char* LGTGetID() {
	char* charID = new char[Lights[activeLight].getID().size() + 1];
	strcpy_s(charID, Lights[activeLight].getID().size() + 1, Lights[activeLight].getID().c_str());
	return charID;
}
#pragma endregion

#pragma region Event Handler
void EVTSetUpdate(void(*func)())
{
	std::cout << "Update function set" << std::endl << std::endl;
	fUpdate = func;
}

void EVTSetCollision(void(*func)(unsigned int idA, unsigned int idB))
{
	fCollision = func;
}

void EVTSetKeyPressed(void(*func)(const char ID[]))
{
	fKeyPressed = func;
}

void EVTSetKeyReleased(void(*func)(const char ID[]))
{
	fKeyReleased = func;
}

void EVTSetKeyboardEvent(void (*func)())
{
	fKeyboardEvent = func;
}

void EVTSetMousePressed(void(*func)(unsigned int button, float x, float y))
{
	fMousePressed = func;
}

void EVTSetMouseReleased(void(*func)(unsigned int button, float x, float y))
{
	fMouseReleased = func;
}

void EVTSetPostPhysicsTick(void(*func)())
{
	fPostPhysicsTick = func;
}

void EVTAddKey(const char name[], int key)
{
	KbdEvent* evt = new KbdEvent;
	evt->key = key;
	evt->id = std::string(name);
	keys.push_back(evt);
}

void EVTRemoveKey(const char name[])
{
	unsigned int i = 0;
	while (i < keys.size()) {
		if (std::strcmp(keys[i]->id.c_str(), name) == 0) {
			keys.erase(keys.begin() + i);
			break;
		}
		i++;
	}
}

void EVTClearKeys()
{
	keys.clear();
	keyBreak = true;
}

void EVTGetMousePos(float pos[])
{
	pos[0] = msex;
	pos[1] = msey;
}

void EVTSetTriggerCallback(void(*func)(unsigned int trigID, unsigned int entityID))
{
	fTrigger = func;
}
#pragma endregion

#pragma region Trigger Functions
void TRGAssign(unsigned int codeID)
{
	unsigned int i = 0;
	while (i < Triggers.size()) {
		if (Triggers.at(i).codeID == codeID) {
			activeTrigger = i;
			break;
		}
		i++;
	}
}

unsigned int TRGCreate()
{
	Trigger t;
	t.codeID = triggerCount;
	triggerCount++;
	Triggers.push_back(t);
	TRGAssign(t.codeID);
	return t.codeID;
}

void TRGDestroy(unsigned int codeID)
{
	unsigned int i = 0;
	while (i < Triggers.size()) {
		if (Triggers.at(i).codeID == codeID) {
			Triggers.erase(Triggers.begin() + i);
			rebuildTriggers = i;
			break;
		}
		i++;
	}
}

void TRGDestroyAll() {
	Triggers.clear();
	triggerCount = 0;
}

void TRGSetSize(float width, float height)
{
	Triggers[activeTrigger].width = width;
	Triggers[activeTrigger].height = height;
}

void TRGSetX(float x)
{
	Triggers[activeTrigger].x = x;
}

void TRGSetY(float y)
{
	Triggers[activeTrigger].y = y;
}

void TRGSetPosition(float x, float y) {
	Triggers[activeTrigger].x = x;
	Triggers[activeTrigger].y = y;
}

void TRGSetID(const char id[])
{
	Triggers[activeTrigger].setID(std::string(id));
}

float TRGGetX()
{
	return Triggers[activeTrigger].x;
}

float TRGGetY()
{
	return Triggers[activeTrigger].y;
}

float TRGGetWidth()
{
	return Triggers[activeTrigger].width;
}

float TRGGetHeight()
{
	return Triggers[activeTrigger].height;
}

char* TRGGetID()
{
	char* charID = new char[Triggers[activeTrigger].getID().size() + 1];
	strcpy_s(charID, Triggers[activeTrigger].getID().size() + 1, Triggers[activeTrigger].getID().c_str());
	return charID;
}

void TRGSetEnabled(bool en)
{
	Triggers[activeTrigger].enabled = en;
}

bool TRGIsEnabled()
{
	return Triggers[activeTrigger].enabled;
}
#pragma endregion

#pragma region Sound
unsigned int AUDCreateSource()
{
	Sound s;
	s.codeID = soundCount;
	
	soundCount++;

	ALuint source;

	alGenSources(1, &source);
	getAlError();

	alSource3f(source, AL_VELOCITY, 0, 0, 0);
	getAlError();

	s.setSourceID(source);

	Sounds.push_back(s);
	AUDAssignSource(s.codeID);

	return s.codeID;
}

void AUDAssignSource(unsigned int codeID)
{
	unsigned int i = 0;
	while (i < Sounds.size()) {
		if (Sounds.at(i).codeID == codeID) {
			activeSound = i;
			break;
		}
		i++;
	}
}

void AUDDestroySource(unsigned int id)
{
	unsigned int i = 0;
	while (i < Sounds.size()) {
		if (Sounds.at(i).codeID == id) {
			Sounds.erase(Sounds.begin() + i);
			break;
		}
		i++;
	}
}

void AUDDestroyAllSources()
{
	Sounds.clear();
	soundCount = 0;
}

unsigned int AUDLoadBuffer(const char path[])
{
	//wav variables
	std::uint8_t channels;
    std::int32_t sampleRate;
    std::uint8_t bitsPerSample;
	std::vector<char> soundData;

	//load wav and check for errors
	if (!load_wav(path, channels, sampleRate, bitsPerSample, soundData))
	{
		std::cerr << "ERROR: Could not load wav" << std::endl;
		return 0;
	}

	//create buffer
	ALuint buffer;
	alGenBuffers(1, &buffer);
	getAlError();

	//determine format of WAV
	ALenum format;
	if (channels == 1 && bitsPerSample == 8)
		format = AL_FORMAT_MONO8;
	else if (channels == 1 && bitsPerSample == 16)
		format = AL_FORMAT_MONO16;
	else if (channels == 2 && bitsPerSample == 8)
		format = AL_FORMAT_STEREO8;
	else if (channels == 2 && bitsPerSample == 16)
		format = AL_FORMAT_STEREO16;
	else
	{
		std::cerr << "ERROR: unrecognised wave format: " << channels << " channels, " << bitsPerSample << " bps" << std::endl;
		return 0;
	}

	//load wav into buffer
	alBufferData(buffer, format, soundData.data(), soundData.size(), sampleRate);
	getAlError();
	soundData.clear(); // erase the sound in RAM

	return buffer;
}

void AUDDestroyBuffer(unsigned int id)
{
	ALuint alID = id;
	alDeleteBuffers(1, &alID);
	getAlError();
}

void AUDPlaySound(unsigned int bufferID, bool loop)
{
	ALuint alID = bufferID;
	Sounds[activeSound].setLooping(loop);
	getAlError();
	Sounds[activeSound].play(alID);
	getAlError();
}

void AUDStopSound()
{
	Sounds[activeSound].stop();
	getAlError();
}

void AUDPauseSound()
{
	Sounds[activeSound].pause();
	getAlError();
}

void AUDResetSound()
{
	Sounds[activeSound].reset();
	getAlError();
}

void AUDSetTimeStamp(float time)
{
	Sounds[activeSound].setTimeStamp(time);
	getAlError();
}

void AUDSetLOC(float x, float y)
{
	Sounds[activeSound].setX(x);
	getAlError();
	Sounds[activeSound].setY(y);
	getAlError();
}

void AUDSetX(float x)
{
	Sounds[activeSound].setX(x);
	getAlError();
}

void AUDSetY(float y)
{
	Sounds[activeSound].setY(y);
	getAlError();
}

void AUDSetSpacial(bool spacial)
{
	Sounds[activeSound].setSpatial(spacial);
	getAlError();
}

void AUDSetVolume(float volume)
{
	Sounds[activeSound].setVolume(volume);
	getAlError();
}

void AUDSetBalance(float balance)
{
	Sounds[activeSound].setBalance(balance);
	getAlError();
}

float AUDGetBalance()
{
	return Sounds[activeSound].getBalance();
}

float AUDGetVolume()
{
	return Sounds[activeSound].getVolume();
}

float AUDGetTimeStamp()
{
	return Sounds[activeSound].getTimeStamp();
}

float AUDGetX()
{
	return Sounds[activeSound].getX();
}

float AUDGetY()
{
	return Sounds[activeSound].getY();
}

bool AUDisPlaying()
{
	return Sounds[activeSound].isPlaying();
	getAlError();
}

bool AUDisSpatial()
{
	return Sounds[activeSound].isSpatial();
}
#pragma endregion

#pragma region Map Import
void MAPSetTileFolder(const char path[]) {
	tilePath = std::string(path);
}

void MAPStartTagCallback(void(*func)(const char tag[])) {
	fTagStart = func;
}

void MAPEndTagCallback(void(*func)(const char tag[])) {
	fTagEnd = func;
}

void MAPVariableCallback(void(*func)(const char id[], const char value[])) {
	fTagVariable = func;
}

void MAPEntityCreationCallback(bool(*func)(unsigned int codeID)) {
	fEntityCreated = func;
}

void MAPTriggerCreationCallback(bool(*func)(unsigned int codeID)) {
	fTriggerCreated = func;
}

void MAPLightCreationCallback(bool(*func)(unsigned int codeID)) {
	fLightCreated = func;
}

void MAPGenerate(const char path[], unsigned int sceneID, bool cleanup) {
	//create and open map file
	std::ifstream inputFile;
	inputFile.open(path);
	if (!inputFile.is_open()) {
		logFile << getMillis() << " | Map file failed to open: " << path << std::endl;
		return;
	}

	if (cleanup) {
		for (unsigned int id : cleanupENTs) {
			ENTDestroy(id);
		}
		for (unsigned int id : cleanupTRGs) {
			TRGDestroy(id);
		}
		for (unsigned int id : cleanupLGTs) {
			LGTDestroy(id);
		}
		cleanupENTs.clear();
		cleanupTRGs.clear();
		cleanupLGTs.clear();
	}

	//create map generation data
	std::string input;
	std::vector<std::string> data;
	std::vector<std::string> tags;
	std::vector<std::string> tagVars;
	SCNAssign(sceneID);
	unsigned int mode = 0; //read mode: 0 = tag scan, 1 = text mode
	objectData object;
	bool customTag = false;

	//read map file
	while (std::getline(inputFile, input)) {
		//split data by end bracket (to input multiple tags on the same line)
		stringSplit(input, '>', data);
	}

	//close file
	inputFile.close();
	log("Map file read successfully, compiling: " + std::string(path));

	//scan tag data
	for (unsigned int i = 0; i < data.size(); i++) {
		std::string str = data[i];
		switch (mode) {
			case 0: {
				if (str.data()[0] == '<' && str.data()[1] != '/') { //check if string is an opening tag
					//remove bracket and split tag and variables
					tagVars.clear();
					str.erase(std::remove(str.begin(), str.end(), '<'), str.end());
					stringSplit(str, ' ', tagVars);
					std::string strVar = tagVars[0];
					tags.insert(tags.begin(), strVar);

					//call tag callback if not a standard tag
					if (strVar != "tile" && strVar != "trigger" && strVar != "light") {
						if (fTagStart != nullptr) {
							fTagStart(strVar.data());
						}
						customTag = true;
					} else {
						customTag = false;
					}

					//parse through tagvars and create object
					for (std::string strVar: tagVars) {
						if (customTag) { //process custom tag
							if (strVar.find('=') != std::string::npos) { //check if segment is actually a variable
								//split segment into two halves
								std::vector<std::string> vars;
								stringSplit(strVar, '=', vars);

								//remove quotes from value portion
								vars[1].erase(std::remove(vars[1].begin(), vars[1].end(), '"'), vars[1].end());

								//call variable callback
								if (fTagVariable != nullptr) {
									fTagVariable(vars[0].data(), vars[1].data());
								}
							}
						} else { //process object tag
							if (strVar.find('=') != std::string::npos) { //check if segment is actually a variable
								//split segment into two halves
								std::vector<std::string> vars;
								stringSplit(strVar, '=', vars);

								//remove quotes from value portion
								vars[1].erase(std::remove(vars[1].begin(), vars[1].end(), '"'), vars[1].end());

								//figure out what variable to assign, then parse and store value
								if (vars[0] == "id") {
									object.objectID = vars[1];
								} else if (vars[0] == "frame") {
									object.frame = std::stoi(vars[1]);
								} else if (vars[0] == "skin") {
									object.tileID = vars[1];
								} else if (vars[0] == "solid") {
									object.solid = vars[1] == "true";
								} else if (vars[0] == "mass") {
									object.mass = std::stof(vars[1]);
								} else if (vars[0] == "visible") {
									object.visible = vars[1] == "true";
								} else if (vars[0] == "opacity") {
									object.opacity = std::stof(vars[1]);
								} else if (vars[0] == "collision") {
									object.collision = std::stoi(vars[1]);
								} else if (vars[0] == "invertx") {
									object.invertX = vars[1] == "true";
								} else if (vars[0] == "inverty") {
									object.invertY = vars[1] == "true";
								} else if (vars[0] == "ambient") {
									object.ambient = vars[1] == "true";
								} else if (vars[0] == "brightness") {
									object.brightness = std::stof(vars[1]);
								} else if (vars[0] == "red") {
									object.red = std::stoi(vars[1]) / 255.0f;
								} else if (vars[0] == "green") {
									object.green = std::stoi(vars[1]) / 255.0f;
								} else if (vars[0] == "blue") {
									object.blue = std::stoi(vars[1]) / 255.0f;
								} else if (vars[0] == "radius") {
									object.radius = std::stof(vars[1]);
								} else if (vars[0] == "width") {
									object.width = std::stof(vars[1]);
								} else if (vars[0] == "height") {
									object.height = std::stof(vars[1]);
								}
							}
						}
					}
					mode = 1;
				}
				break;
			}
			case 1: { //read text (coordinates only)
				if (str.data()[0] == '<' && str.data()[1] == '/') { //check if string is closing tag
					//remove bracket and slash
					str.erase(std::remove(str.begin(), str.end(), '<'), str.end());
					str.erase(std::remove(str.begin(), str.end(), '/'), str.end());

					//check if closing tag is the same as opening tag, then remove
					if (tags[0] == str) {
						tags.erase(tags.begin());
						if (fTagEnd != nullptr) {
							fTagEnd(str.data());
						}
					} else {
						error("Tag mismatch error!");
					}

					//exit text mode and break
					mode = 0;
					break;
				} else if (str.data()[0] == '<' && str.data()[1] != '/') { //check if next tag is an open tag
					//rewind and process tag
					i--;
					mode = 0;
					break;
				}

				//skip next step if custom tag
				if (customTag) {
					break;
				}

				//split coordinates into separate xy segments
				std::vector<std::string> coordList;
				stringSplit(str, ';', coordList);
				for (std::string coordSegment : coordList) {
					//split segment into separate coordinates
					std::vector<std::string> coords;
					stringSplit(coordSegment, ',', coords);
					float x = 0.0, y = 0.0;
					x = std::stof(coords[0]);
					y = std::stof(coords[1]);
					
					//use placeholder object to create object (use tag in tags vector to determine witch object to create)
					if (tags[0] == "tile") {
						unsigned int ent = ENTCreate();
						ENTSetID(object.objectID.data());
						//scan tilesheet id
						for (TileSheet sheet : Tilesheets) {
							if (sheet.getID() == object.tileID) {
								TLSAssign(sheet.codeID);
								break;
							}
						}
						ENTSetTileSheet();
						ENTSetFrame(object.frame);
						ENTSetSolid(object.solid);
						ENTSetMass(object.mass);
						ENTSetVisible(object.visible);
						ENTSetOpacity(object.opacity);
						ENTSetCollisionGroup(object.collision);
						ENTSetInvertX(object.invertX);
						ENTSetInvertY(object.invertY);
						ENTSetPosition(x, y);

						if (fEntityCreated != nullptr) {
							if (fEntityCreated(Entities[activeEntity].codeID)) {
								SCNAddEntity();
								cleanupENTs.push_back(ent);
								log("Entity created " + Entities[activeEntity].getID());
							} else {
								ENTDestroy(ent);
							}
						} else {
							SCNAddEntity();
							cleanupENTs.push_back(ent);
							log("Entity created " + Entities[activeEntity].getID());
						}
					} else if (tags[0] == "trigger") {
						unsigned int trg = TRGCreate();
						TRGSetID(object.objectID.data());
						TRGSetSize(object.width, object.height);
						TRGSetPosition(x, y);

						if (fTriggerCreated != nullptr) {
							if (fTriggerCreated(Triggers[activeTrigger].codeID)) {
								SCNAddTrigger();
								cleanupTRGs.push_back(trg);
								log("Trigger created " + Triggers[activeTrigger].getID());
							} else {
								TRGDestroy(trg);
							}
						} else {
							SCNAddTrigger();
							cleanupTRGs.push_back(trg);
							log("Trigger created " + Triggers[activeTrigger].getID());
						}
					} else if (tags[0] == "light") {
						unsigned int lgt = LGTCreate();
						LGTSetID(object.objectID.data());
						if (object.ambient) {
							LGTSetType(DE4_LIGHT_AMBIENT);
						} else {
							LGTSetType(DE4_LIGHT_POINT);
						}
						LGTSetBrightness(object.brightness);
						LGTSetRadius(object.radius);
						LGTSetColor(object.red, object.green, object.blue);
						LGTSetPosition(x, y);

						if (fLightCreated != nullptr) {
							if (fLightCreated(Lights[activeLight].codeID)) {
								SCNAddLight();
								cleanupLGTs.push_back(lgt);
								log("Light created " + Lights[activeLight].getID());
							} else {
								LGTDestroy(lgt);
							}
						} else {
							SCNAddLight();
							cleanupLGTs.push_back(lgt);
							log("Light created " + Lights[activeLight].getID());
						}
					}
				}

				break;
			}
		}
	}
}

void MAPAddCleanupEntity(unsigned int entID) {
	cleanupENTs.push_back(entID);
}

void MAPAddCleanupLight(unsigned int lightID) {
	cleanupLGTs.push_back(lightID);
}

void MAPAddCleanupTrigger(unsigned int trigID) {
	cleanupTRGs.push_back(trigID);
}
#pragma endregion

#pragma region Utils
float UTILRange(unsigned int idA, unsigned int idB) {
	unsigned int i = 0;
	unsigned int entA = 0;
	unsigned int entB = 0;

	while (i < Entities.size()) {
		if (Entities.at(i).codeID == idA) {
			entA = i;
			break;
		}
		i++;
	}
	i = 0;
	while (i < Entities.size()) {
		if (Entities.at(i).codeID == idB) {
			entB = i;
			break;
		}
		i++;
	}

	return range(Entities[entA], Entities[entB]);
}

bool UTILIntersect(unsigned int idA, unsigned int idB) {
	unsigned int i = 0;
	unsigned int entA = 0;
	unsigned int entB = 0;

	while (i < Entities.size()) {
		if (Entities.at(i).codeID == idA) {
			entA = i;
			break;
		}
		i++;
	}
	i = 0;
	while (i < Entities.size()) {
		if (Entities.at(i).codeID == idB) {
			entB = i;
			break;
		}
		i++;
	}

	return intersects(Entities[entA], Entities[entB]);
}

bool UTILIntersect(unsigned int idA, float x, float y) {
	unsigned int i = 0;
	unsigned int entA = 0;

	while (i < Entities.size()) {
		if (Entities.at(i).codeID == idA) {
			entA = i;
			break;
		}
		i++;
	}

	return intersects(Entities[entA], x, y);
}

void UTILSeedRNG() {
	seedRNG();
}

float UTILRandomFloat(float min, float max) {
	return randomFloat(min, max);
}

#pragma endregion

#pragma region Game State
void GMSTAssign(unsigned int id) {
	unsigned int i = 0;
	while (i < GameStates.size()) {
		if (GameStates[i].codeID == id) {
			activeState = i;
			break;
		}
		i++;
	}
}

unsigned int GMSTCreate() {
	GameState e;
	e.codeID = stateCount;
	GameStates.push_back(e);
	stateCount++;
	GMSTAssign(e.codeID);
	return e.codeID;
}

unsigned int GMSTCreate(void(*func)()) {
	GameState e;
	e.codeID = stateCount;
	if (func != nullptr) {
		func();
	}
	GameStates.push_back(e);
	stateCount++;
	GMSTAssign(e.codeID);
	return e.codeID;
}

void GMSTDestroy(unsigned int id) {
	unsigned int i = 0;
	while (i < GameStates.size()) {
		if (GameStates.at(i).codeID == id) {
			GameStates.erase(GameStates.begin() + i);
			break;
		}
		i++;
	}
}

void GMSTDestroyAll() {
	GameStates.clear();
	stateCount = 0;
}

void GMSTSetUpdate(void(*func)()) {
	GameStates[activeState].fUpdate = func;
}

void GMSTSetKeyPress(void(*func)(const char* id)) {
	GameStates[activeState].fKeyPressed = func;
}

void GMSTSetKeyRelease(void(*func)(const char* id)) {
	GameStates[activeState].fKeyReleased = func;
}

void GMSTSetCollision(void(*func)(unsigned int ida, unsigned int idb)) {
	GameStates[activeState].fCollide = func;
}

void GMSTSetKeyboardEvent(void(*func)()) {
	GameStates[activeState].fKeyboardEvent = func;
}

void GMSTSetMousePressed(void(*func)(unsigned int button, float x, float y)) {
	GameStates[activeState].fMousePressed = func;
}

void GMSTSetMouseReleased(void(*func)(unsigned int button, float x, float y)) {
	GameStates[activeState].fMouseReleased = func;
}

void GMSTSetTrigger(void(*func)(unsigned int trigID, unsigned int entID)) {
	GameStates[activeState].fTrigger = func;
}

void GMSTSetOnEnabledChange(void(*func)(bool enabled)) {
	GameStates[activeState].fOnEnableChange = func;
}

void GMSTSetEnabled(bool enabled) {
	GameStates[activeState].setEnabled(enabled);
}

void GMSTDisableAll() {
	for (unsigned int i = 0; i < GameStates.size(); i++) {
		GameStates[i].setEnabled(false);
	}
}
#pragma endregion

#pragma region Text
void TXTAssign(unsigned int id) {
	unsigned int i = 0;
	while (i < Texts.size()) {
		if (Texts[i].codeID == id) {
			activeText = i;
			break;
		}
		i++;
	}
}

unsigned int TXTCreate() {
	Text e;
	e.codeID = textCount;
	e.x = 0;
	e.y = 0;
	e.font = 0;
	e.scale = 1;
	e.visible = true;
	Texts.push_back(e);
	textCount++;
	TXTAssign(e.codeID);
	return e.codeID;
}

void TXTDestroy(unsigned int id) {
	unsigned int i = 0;
	while (i < Texts.size()) {
		if (Texts.at(i).codeID == id) {
			Texts.erase(Texts.begin() + i);
			rebuildTexts = i;
			break;
		}
		i++;
	}
}

void TXTDestroyAll() {
	Texts.clear();
	textCount = 0;
}

void TXTSetText(const char text[]) {
	Texts[activeText].text = (std::string(text));
}

void TXTSetSheet(unsigned int sheet) {
	unsigned int i = 0;
	while (i < Tilesheets.size()) {
		if (Tilesheets.at(i).codeID == sheet) {
			Texts[activeText].sheet = Tilesheets.at(i);
			break;
		}
		i++;
	}
}

void TXTSetID(const char id[]) {
	Texts[activeText].ID = (std::string(id));
}

void TXTSetX(float x) {
	Texts[activeText].x = x;
}

void TXTSetY(float y) {
	Texts[activeText].y = y;
}

void TXTSetPosition(float x, float y) {
	Texts[activeText].x = x;
	Texts[activeText].y = y;
}

void TXTSetScale(float scale) {
	Texts[activeText].scale = scale;
}

void TXTSetFont(unsigned int font) {
	Texts[activeText].font = font;
}

void TXTSetMaxLineWidth(unsigned int width) {
	Texts[activeText].lineWidth = width;
}

void TXTSetVisible(bool visible) {
	Texts[activeText].visible = visible;
}

char* TXTGetText() {
	char* charText = new char[Texts[activeText].text.size() + 1];
	strcpy_s(charText, Texts[activeText].text.size() + 1, Texts[activeText].text.c_str());
	return charText;
}

char* TXTGetID() {
	char* charID = new char[Texts[activeText].ID.size() + 1];
	strcpy_s(charID, Texts[activeText].ID.size() + 1, Texts[activeText].ID.c_str());
	return charID;
}

float TXTGetX() {
	return Texts[activeText].x;
}

float TXTGetY() {
	return Texts[activeText].y;
}

float TXTGetScale() {
	return Texts[activeText].scale;
}

unsigned int TXTGetFont() {
	return Texts[activeText].font;
}

bool TXTIsVisible() {
	return Texts[activeText].visible;
}

float TXTGetWidth() {
	return Texts[activeText].width * Texts[activeText].scale;
}

float TXTGetHeight() {
	return Texts[activeText].height * Texts[activeText].scale;
}
#pragma endregion

#pragma region Cinematics
void CINStart() {
	currentKeyframe = 0;

	//set first keyframe
	framesRemaining = keyframes[0].frames;
	manualWait = keyframes[0].manual;
	timedWait = keyframes[0].timed;

	cinematicsRunning = true;
}

void CINStop() {
	cinematicsRunning = false;
}

void CINContinue() {
	if (manualWait) {
		framesRemaining = 0;
	}
}

void CINJump(unsigned int code) {
	currentKeyframe = code;
	framesRemaining = keyframes[currentKeyframe].frames;
	manualWait = keyframes[currentKeyframe].manual;
	timedWait = keyframes[currentKeyframe].timed;
}

void CINSetCallback(void(*func)(unsigned int code)) {
	cinematicCall = func;
}

void CINAddTimedKeyframe(unsigned int code, unsigned int frames) {
	keyframe kf;
	kf.code = code;
	kf.frames = frames;
	kf.timed = true;
	kf.manual = false;

	keyframes.push_back(kf);
}

void CINAddHybridKeyframe(unsigned int code, unsigned int frames) {
	keyframe kf;
	kf.code = code;
	kf.frames = frames;
	kf.timed = true;
	kf.manual = true;

	keyframes.push_back(kf);
}

void CINAddManualKeyframe(unsigned int code) {
	keyframe kf;
	kf.code = code;
	kf.frames = 1;
	kf.timed = false;
	kf.manual = true;

	keyframes.push_back(kf);
}

void CINClearKeyframes() {
	keyframes.clear();
}

bool CINIsRunning() {
	return cinematicsRunning;
}
#pragma endregion
