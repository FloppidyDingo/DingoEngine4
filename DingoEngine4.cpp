/*
Things to do:
	implement fKeyboardEvent
	finish Utils
	redo keyboard input
	Switch to GLFW (GLUT sucks)

Things to Add/change/fix:
	Built-in map generator (XML tagged, will finally allow us to make a GOD DAMN LEVEL EDITOR)
	Create a support library (Animation timer)
	Add particle system
*/

#pragma once

#include "pch.h"
#include "DingoEngine4.h"
#include "DE4Types.h"
#include "Utils.h"
#include "shaders.h"

#include <iostream>
#include <algorithm>
#include <array>
#include <math.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <chrono>
#include <al.h>
#include <alc.h>
#include <Windows.h>

//I have no idea what this is but it seems important------------------------------------------------
#pragma region important crap
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

//scene management
Scene scene;
std::vector<unsigned int> entities;
std::vector<unsigned int> gui;
std::vector<unsigned int> lights;
std::vector<unsigned int> triggers;

//physics variables
unsigned int physicsMode;
float gravity;
float terminalVelocity;
float camPos[2];
float camDir[2];
std::vector<std::array<unsigned int,3>> noCollides;
unsigned int noCollideCount = 0;

//profiler
std::string profilerInfo; //The profiler string
int fps; //Frames per second
int fpsTemp;
long long fpsStart; //FPS counter timer start
long long fpsTimer; //fps timer
int physObjects; //Number of objects with physics
int drawCalls; //Number of draw calls
int textureSwaps; //Number of texture swaps
bool Profile; //Profiler enable
int frame = 0; //total number of frames

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
void (*fTrigger)(const char ID[], unsigned int entityID);
void (*fCollision)(unsigned int idA, unsigned int idB);
void (*fKeyPressed)(const char ID[]);
void (*fKeyReleased)(const char ID[]);
void (*fKeyboardEvent)();
void (*fMousePressed)(unsigned int button, int x, int y);
void (*fMouseReleased)(unsigned int button, int x, int y);
void (*fPostPhysicsTick)();
int msex;
int msey;
bool msepress;
bool mserelease;
int msebtn;
int kbdpresses;
int kbdreleases;
unsigned char kbdkey[128]; //list of the indexes of activated registered keys
unsigned char kbdreleasekey[128];
std::vector<KbdEvent*> keys;
#pragma endregion

//Update each frame
/*
When updating:
	-Update all animations
	-populate Engine with all object IDs from scenes, place addresses in lists
	-Run Event Handler (mouse/keyboard events, update function)
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
	
	if (kbdtmppresses > 0) {
		for (int i = 0; i < kbdtmppresses; i++) {
			if (fKeyPressed != nullptr) {
				fKeyPressed(keys[kbdtmpprs[i]]->id.c_str());
			}
		}
	}
	if (kbdtmpreleases > 0) {
		for (int i = 0; i < kbdtmpreleases; i++) {
			if (fKeyReleased != nullptr) {
				fKeyReleased(keys[kbdtmprls[i]]->id.c_str());
			}
		}
	}
	if (msepress) {
		if (fMousePressed != nullptr) {
			fMousePressed(msebtn, msex, msey);
		}
	} else if (mserelease) {
		if (fMouseReleased != nullptr) {
			fMouseReleased(msebtn, msex, msey);
		}
	}

	double msdx, msdy;
	glfwGetCursorPos(window, &msdx, &msdy);
	msex = (msdx - (windowX / 2)) * ((float)resolutionX / (float)windowX);
	msey = -(msdy - (windowY / 2)) * ((float)resolutionY / (float)windowY);

	//call update function
	fUpdate();
	long long userEnd = getMillis();
	#pragma endregion

	//populate active object lists from scene
	#pragma region list population
	entities.clear();
	unsigned int si = 0;
	while (si < scene.getEntities().size()) {
		unsigned int code = scene.getEntities().at(si).codeID;
		unsigned int i = 0;
		while (i < Entities.size()) {
			if (Entities.at(i).codeID == code) {
				entities.push_back(i);
				break;
			}
			i++;
		}
		si++;
	}
	gui.clear();
	si = 0;
	while (si < scene.getGUI().size()) {
		unsigned int code = scene.getGUI().at(si).codeID;
		unsigned int i = 0;
		while (i < Entities.size()) {
			if (Entities.at(i).codeID == code) {
				gui.push_back(i);
				break;
			}
			i++;
		}
		si++;
	}
	lights.clear();
	si = 0;
	while (si < scene.getLights().size()) {
		unsigned int code = scene.getLights().at(si).codeID;
		unsigned int i = 0;
		while (i < Lights.size()) {
			if (Lights.at(i).codeID == code) {
				lights.push_back(i);
				break;
			}
			i++;
		}
		si++;
	}
	triggers.clear();
	si = 0;
	while (si < scene.getTriggers().size()) {
		unsigned int code = scene.getTriggers().at(si).codeID;
		unsigned int i = 0;
		while (i < Triggers.size()) {
			if (Triggers.at(i).codeID == code) {
				triggers.push_back(i);
				break;
			}
			i++;
		}
		si++;
	}
	#pragma endregion

	//Update objects
	#pragma region Object update
	for (unsigned int i = 0; i < Animations.size(); i++) {
		Animations[i].update();
	}
	for (unsigned int i = 0; i < entities.size(); i++) {
		Entities[entities[i]].update();
	}
	#pragma endregion

	//physics calculations
	#pragma region physics
	long long physicsStart = getMillis();
	if (Profile) {
		physObjects = 0;
		for (unsigned int i : entities) {
			if (Entities[i].getMass() != 0 || Entities[i].isSolid()) {
				physObjects++;
			}
		}
	}

	//gravity
	if (physicsMode == DE4_PLATFORMER) {
		for (unsigned int i : entities) {
			if (Entities[i].dir[1] < terminalVelocity) {
				Entities[i].dir[1] -= gravity * Entities[i].getMass();
			}
		}
	}
	
	//collisions
	for (unsigned int i : entities) {
		if ((Entities[i].dir[0] != 0) || (Entities[i].dir[1] != 0)) {
			for (unsigned int i2 : entities) {
				Entity e1 = Entities[i];  //moving entity
				Entity e2 = Entities[i2]; //solid entity
				/*
				Conditions for collision:
					-the solid block is within a radius that is equal to the moving vector
					-moving entity has a velocity > 0 (checked previously)
					-the code IDs do not match
					-the combination of collision groups are not in a no-collide
					-both entities are solid
				*/
				//check the noCollide list
				bool noCollide = false;
				for (std::array<unsigned int, 3> group : noCollides) {
					noCollide = ((group[1] == e1.getCollisionGroup()) && (group[2] == e2.getCollisionGroup())) || ((group[1] == e2.getCollisionGroup()) && (group[2] == e1.getCollisionGroup()));
				}
				//Check and exexute physics calculations
				if ((e1.codeID != e2.codeID) && !noCollide && e1.isSolid() && e2.isSolid() && 
					(range(e1, e2) < (fabsf(e1.x) + e1.getWidth() * 2)) && (range(e1, e2) < (fabsf(e1.y) + e1.getWidth() * 2))) {
					/*
					Collision detection
						-Generate rectangle using moving's velocity
						-generate recatngle with solid's dimensions
						-compare to see if they intersect
						-call collision detection if hit
						-change moving's x or y velocity to not hit solid
					*/
					//moving rectangle
					float ax1 = e1.x - ((e1.getWidth() * globalScale / 2) * clamp(e1.dir[0] * 1000, -1.0, 1.0));
					float ay1 = e1.y + ((e1.getHeight() * globalScale / 2) * clamp(e1.dir[1] * 1000, -1.0, 1.0));
					float ax2 = e1.dir[0];
					float ay2 = e1.dir[1];
					//solid rectangle
					float bx1 = e2.x - (e2.getWidth() * globalScale / 2);
					float by1 = e2.y + (e2.getHeight() * globalScale / 2);
					float bx2 = e2.x + (e2.getWidth() * globalScale / 2);
					float by2 = e2.y - (e2.getHeight() * globalScale / 2);

					if (ax1 < bx2 && ax2 > bx1 && ay1 > by2 && ay2 < by1) {
						//intersect detected
						fCollision(e1.codeID, e2.codeID);
						if (e1.x > e2.x) {
							e1.dir[0] = clamp(e1.dir[0], e2.x + (e2.getWidth() * globalScale / 2), FLT_MAX);
						}else if (e1.x < e2.x) {
							e1.dir[0] = clamp(e1.dir[0], -FLT_MAX, e2.x - (e2.getWidth() * globalScale / 2));
						}
						if (e1.y > e2.y) {
							e1.dir[1] = clamp(e1.dir[1], e2.y + (e2.getHeight() * globalScale / 2), FLT_MAX);
						}
						else if (e1.y < e2.y) {
							e1.dir[1] = clamp(e1.dir[1], -FLT_MAX, e2.y - (e2.getHeight() * globalScale / 2));
						}
					}
				}
			}
		}
	}
	
	//movement
	for (unsigned int i : entities) {
		Entities[i].x += Entities[i].dir[0];
		Entities[i].y += Entities[i].dir[1];
	}

	//post physics tick
	if (fPostPhysicsTick != nullptr) {
		fPostPhysicsTick();
	}
	
	long long physicsEnd = getMillis();
	#pragma endregion

	//camera
	camPos[0] += camDir[0];
	camPos[1] += camDir[1];

	//trigger calculation
	#pragma region triggers
	for (unsigned int ti : triggers) {
		Trigger t = Triggers[ti];
		for (unsigned int ei : entities) {
			Entity e = Entities[ei];
			if (inBetween(e.x, t.x - (t.width / 2), t.x + (t.width / 2)) && 
				inBetween(e.y, t.y - (t.height / 2), t.y + (t.height / 2)) && t.enabled) {
				char* charID = new char[t.id.size() + 1];
				strcpy_s(charID, t.id.size() + 1, t.id.c_str());
				fTrigger(charID, e.codeID);
				delete[] charID;
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
	for (int i = 0; i < lights.size(); i ++) {
		Light l = Lights[lights[i]];
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
	//inversion variables
	int invX = 1;
	int invY = 1;
	
	#pragma endregion
	//assign shader
	glUseProgram(ENTShader);
	int prevID = -1;
	for (unsigned int e : entities) {
		Entity ent = Entities[e];
		//check if entity is in the screen and visible
		bool render = (((ent.x - gcx) + (ent.getWidth() * globalScale / 2)) > -(resolutionX / 2) || ((ent.x - gcx) + (ent.getWidth() * globalScale / 2)) < (resolutionX / 2) ||
			((ent.y - gcy) + (ent.getHeight() * globalScale / 2)) > -(resolutionY / 2) || ((ent.y - gcy) + (ent.getHeight() * globalScale / 2)) < (resolutionY / 2)) && ent.isVisible();
		if (render) 
		{
			drawCalls++;
			//check if texture ID is already in use, if not set it to the correct one
			if (ent.getTextureID() != prevID) {
				prevID = ent.getTextureID();
				textureSwaps++;
			}
			//compute invert x and invert y scales
			if (ent.isInvertX()) {
				invX = -1;
			}
			else {
				invX = 1;
			}
			if (ent.isInvertX()) {
				invY = -1;
			}
			else {
				invY = 1;
			}
			//add tile data to pass to the shader
			//send vertex data
			vertData = {
				//position
				((ent.x - (ent.getWidth() * globalScale * ent.getScale() / 2)) * invX), ((ent.y + (ent.getHeight() * globalScale * ent.getScale() / 2)) * invY), 0,
				((ent.x + (ent.getWidth() * globalScale * ent.getScale() / 2)) * invX), ((ent.y + (ent.getHeight() * globalScale * ent.getScale() / 2)) * invY), 0,
				((ent.x + (ent.getWidth() * globalScale * ent.getScale() / 2)) * invX), ((ent.y - (ent.getHeight() * globalScale * ent.getScale() / 2)) * invY), 0,
				((ent.x - (ent.getWidth() * globalScale * ent.getScale() / 2)) * invX), ((ent.y - (ent.getHeight() * globalScale * ent.getScale() / 2)) * invY), 0,
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
	for (unsigned int e : gui) {
		Entity ent = Entities[e];
		if (ent.isVisible()) {
			drawCalls++;
			//check if texture ID is already in use, if not set it to the correct one
			if (ent.getTextureID() != prevID) {
				prevID = ent.getTextureID();
				textureSwaps++;
			}
			//add tile data to pass to the shader
			//send vertex data
			vertData = {
				//position
				((ent.x - (ent.getWidth() * globalScale * ent.getScale() / 2))* invX), ((ent.y + (ent.getHeight() * globalScale * ent.getScale() / 2))* invY), 0,
				((ent.x + (ent.getWidth() * globalScale * ent.getScale() / 2))* invX), ((ent.y + (ent.getHeight() * globalScale * ent.getScale() / 2))* invY), 0,
				((ent.x + (ent.getWidth() * globalScale * ent.getScale() / 2))* invX), ((ent.y - (ent.getHeight() * globalScale * ent.getScale() / 2))* invY), 0,
				((ent.x - (ent.getWidth() * globalScale * ent.getScale() / 2))* invX), ((ent.y - (ent.getHeight() * globalScale * ent.getScale() / 2))* invY), 0,
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
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glUseProgram(0);

	glfwSwapBuffers(window);
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
			" | TOBJ: " + std::to_string(entities.size()) + //Total objects
			" | POBJ: " + std::to_string(physObjects) + //Physics objects
			" | DCS: " + std::to_string(drawCalls) + //Draw Calls
			" | TSWP: " + std::to_string(textureSwaps) + //Texture Swaps
			" | TTOT: " + std::to_string(totalEnd - totalStart) + //Total frame time
			" | TPHYS: " + std::to_string(physicsEnd - physicsStart) + //Physics time
			" | TUSE: " + std::to_string(userEnd - userStart) + //Update and event handler time
			" | TDRAW: " + std::to_string(drawEnd - drawStart) + //Update and event handler time
			" | FPS: " + std::to_string(fps); //frames per second
		glfwSetWindowTitle(window, profilerInfo.c_str());
	}
	#pragma endregion
}

#pragma region event handler backend
void kbdhandle(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (fKeyboardEvent != nullptr) {
		fKeyboardEvent();
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
	//check some things to make sure nothing will completely explode
	std::cout << "Dingo Engine 4 Version " << DE4_VERSION << "\n";
	if (fUpdate == NULL) {
		std::cout << "Update loop not attached!!!";
	}

	//initialise variables
	Profile = profile;
	resolutionX = resx;
	windowX = resx;
	windowY = resy;
	resolutionY = resy;
	FrameRate = framerate;
	globalScale = 1;

	//start glfw
	if (!glfwInit()){
		exit(EXIT_FAILURE);
	}	

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	window = glfwCreateWindow(resx, resy, "", NULL, NULL);

	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	GLuint err = glewInit();
	if (err != GLEW_OK)
	{
		std::cout << "GL initialization error 0x" + err;
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
		exit(EXIT_FAILURE);
	}
	GUIShader = glCreateProgram();
	glAttachShader(GUIShader, guivertex);
	glAttachShader(GUIShader, guifragment);
	glLinkProgram(GUIShader);
	if (!getLinkError(GUIShader, GL_LINK_STATUS)) {
		std::cerr << "GUI shader failed to link" << std::endl;
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
		exit(EXIT_FAILURE);
	}
	ENTShader = glCreateProgram();
	glAttachShader(ENTShader, guivertex);
	glAttachShader(ENTShader, entfragment);
	glLinkProgram(ENTShader);
	if (!getLinkError(ENTShader, GL_LINK_STATUS)) {
		std::cerr << "ENT shader failed to link" << std::endl;
		exit(EXIT_FAILURE);
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

	//start audio engine
	ALCdevice* openALDevice = alcOpenDevice(nullptr);
	if (!openALDevice)
	{
		std::cerr << "Audio failed to start" << std::endl;
	}

	//create AL context
	ALCcontext* openALContext;
	openALContext = alcCreateContext(openALDevice, nullptr);
	getAlError();

	//make context current
	ALCboolean contextMadeCurrent = false;
	contextMadeCurrent = alcMakeContextCurrent(openALContext);
	getAlError();

	//execute assigned init function
	init();
	int tempx, tempy;
	glfwGetFramebufferSize(window, &tempx, &tempy);
	windowResize(window, tempx, tempy);
	while (!glfwWindowShouldClose(window))
	{
		frameUpdate();
		glfwPollEvents();
	}

	glfwTerminate();
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
				std::cerr << "AL_INVALID_NAME: a bad name (ID) was passed to an OpenAL function";
				break;
			}
			case AL_INVALID_ENUM: {
				std::cerr << "AL_INVALID_ENUM: an invalid enum value was passed to an OpenAL function";
				break;
			}
			case AL_INVALID_VALUE: {
				std::cerr << "AL_INVALID_VALUE: an invalid value was passed to an OpenAL function";
				break;
			}
			case AL_INVALID_OPERATION: {
				std::cerr << "AL_INVALID_OPERATION: the requested operation is not valid";
				break;
			}
			case AL_OUT_OF_MEMORY: {
				std::cerr << "AL_OUT_OF_MEMORY: the requested operation resulted in OpenAL running out of memory";
				break;
			}
			default: {
				std::cerr << "UNKNOWN AL ERROR: " << error;
			}
		}
		std::cerr << std::endl;
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
#pragma endregion

//DLL Implimentation ---------------------------------------------------------------------------------------
void DE4SetScene(unsigned int sceneID)
{
	unsigned int i = 0;
	while (i < Scenes.size()) {
		Scene a1 = Scenes.at(i);
		if (a1.codeID == sceneID) {
			scene = Scenes.at(i);
			break;
		}
		i++;
	}
}

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

DE4_API void PHYSetCamPostion(float cam[])
{
	camPos[0] = cam[0];
	camPos[1] = cam[1];
}

void PHYGetCamPosition(float cam[])
{
	cam[0] = camPos[0];
	cam[1] = camPos[1];
}

void PHYSetCamVector(float vec[])
{
	camDir[0] = vec[0];
	camDir[1] = vec[1];
}

void PHYGetCamVector(float vec[])
{
	vec[0] = camDir[0];
	vec[1] = camDir[1];
}

unsigned int PHYAddNoCollide(unsigned int groupA, unsigned int groupB)
{
	std::array<unsigned int, 3> nc;
	nc[0] = noCollideCount;
	noCollideCount++;
	nc[1] = groupA;
	nc[2] = groupB;
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

unsigned int* PHYGetNoCollide(unsigned int id)
{
	unsigned int i = 0;
	while (i < noCollides.size()) {
		if (noCollides.at(i)[0] == id) {
			unsigned int* ar = new unsigned int[3];
			ar[0] = noCollides.at(i)[0];
			ar[1] = noCollides.at(i)[1];
			ar[2] = noCollides.at(i)[2];
			return ar;
			break;
		}
		i++;
	}
	return nullptr;
}

void PHYClearNoCollides()
{
	noCollides.clear();
}
#pragma endregion

#pragma region GPU Functions
void GPUSetFullScreen(bool fullScreen)
{
	glfwSetWindowMonitor(window, fullScreen ? glfwGetPrimaryMonitor() : NULL, 0, 0, resolutionX, resolutionY, GLFW_DONT_CARE);
}

void GPUSetGlobalScale(float scale)
{
	globalScale = scale;
}

float GPUGetGlobalScale()
{
	return globalScale;
}

void GPUSetTitle(const char title[])
{
	if (!Profile) {
		glfwSetWindowTitle(window, title);
	}
}
#pragma endregion

#pragma region Entity Functions
void ENTAssign(unsigned int code)
{
	unsigned int i = 0;
	while (i < Entities.size()) {
		if (Entities.at(i).codeID == code) {
			activeEntity = Entities.at(i).codeID;
			break;
		}
		i++;
	}
}

unsigned int ENTCreate()
{
	Entity e;
	e.codeID = entityCount;
	Entities.push_back(e);
	entityCount++;
	activeEntity = e.codeID;
	return e.codeID;
}

void ENTDestroy(unsigned int code)
{
	unsigned int i = 0;
	while (i < Entities.size()) {
		if (Entities.at(i).codeID == code) {
			Entities.erase(Entities.begin() + i);
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

int ENTGetWidth()
{
	return Entities[activeEntity].getWidth() * globalScale;
}

int ENTGetHeight()
{
	return Entities[activeEntity].getHeight() * globalScale;
}

void ENTSetX(float x)
{
	Entities[activeEntity].x = x;
}

void ENTSetY(float y)
{
	Entities[activeEntity].y = y;
}

void ENTGetDir(float vec[])
{
	Entities[activeEntity].getDirection(vec);
}

void ENTSetDir(float vec[])
{
	Entities[activeEntity].setDirection(vec);
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

float ENTGetScale()
{
	return Entities[activeEntity].getScale();
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
#pragma endregion

#pragma region Scene Functions
void SCNAssign(unsigned int code)
{
	unsigned int i = 0;
	while (i < Scenes.size()) {
		if (Scenes.at(i).codeID == code) {
			activeScene = Scenes.at(i).codeID;
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
	activeScene = s.codeID;
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
	Scenes[activeScene].addEntity(Entities[activeEntity]);
}

void SCNAddEntity(unsigned int entCode)
{
	unsigned int i = 0;
	while (i < Entities.size()) {
		if (Entities.at(i).codeID == entCode) {
			Scenes[activeScene].addEntity(Entities.at(i));
			break;
		}
		i++;
	}
}

void SCNAddLight()
{
	Scenes[activeScene].addLight(Lights[activeLight]);
	if (Scenes[activeScene].getLights().size() > MAX_LIGHTS) {
		std::cerr << "Maximum number of lights exceeded!";
		exit(EXIT_FAILURE);
	}
}

void SCNAddLight(unsigned int lightCode)
{
	unsigned int i = 0;
	while (i < Lights.size()) {
		if (Lights.at(i).codeID == lightCode) {
			Scenes[activeScene].addLight(Lights.at(i));
			break;
		}
		i++;
	}
	if (Scenes[activeScene].getLights().size() > MAX_LIGHTS) {
		std::cerr << "Maximum number of lights exceeded!";
		exit(EXIT_FAILURE);
	}
}

void SCNAddGUI()
{
	Scenes[activeScene].addGUI(Entities[activeEntity]);
}

void SCNAddGUI(unsigned int entCode)
{
	unsigned int i = 0;
	while (i < Entities.size()) {
		if (Entities.at(i).codeID == entCode) {
			Scenes[activeScene].addGUI(Entities.at(i));
			break;
		}
		i++;
	}
}

void SCNAddTrigger()
{
	Scenes[activeScene].addTrigger(Triggers[activeTrigger]);
}

void SCNAddTrigger(unsigned int trigCode)
{
	unsigned int i = 0;
	while (i < Triggers.size()) {
		if (Triggers.at(i).codeID == trigCode) {
			Scenes[activeScene].addTrigger(Triggers.at(i));
			break;
		}
		i++;
	}
}

void SCNRemoveEntity()
{
	Scenes[activeScene].removeEntity(Entities[activeEntity]);
}

void SCNRemoveEntity(unsigned int entCode)
{
	unsigned int i = 0;
	while (i < Entities.size()) {
		if (Entities.at(i).codeID == entCode) {
			Scenes[activeScene].removeEntity(Entities.at(i));
			break;
		}
		i++;
	}
}

void SCNRemoveLight()
{
	Scenes[activeScene].removeLight(Lights[activeLight]);
}

void SCNRemoveLight(unsigned int lightCode)
{
	unsigned int i = 0;
	while (i < Lights.size()) {
		if (Lights.at(i).codeID == lightCode) {
			Scenes[activeScene].removeLight(Lights.at(i));
			break;
		}
		i++;
	}
}

void SCNRemoveGUI()
{
	Scenes[activeScene].removeGUI(Entities[activeEntity]);
}

void SCNRemoveGUI(unsigned int entCode)
{
	unsigned int i = 0;
	while (i < Entities.size()) {
		if (Entities.at(i).codeID == entCode) {
			Scenes[activeScene].removeGUI(Entities.at(i));
			break;
		}
		i++;
	}
}

void SCNRemoveTrigger()
{
	Scenes[activeScene].removeTrigger(Triggers[activeTrigger]);
}

void SCNRemoveTrigger(unsigned int trigCode)
{
	unsigned int i = 0;
	while (i < Triggers.size()) {
		if (Triggers.at(i).codeID == trigCode) {
			Scenes[activeScene].removeTrigger(Triggers.at(i));
			break;
		}
		i++;
	}
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
}
#pragma endregion

#pragma region Animation Functions
void ANIAssign(unsigned int code)
{
	unsigned int i = 0;
	while (i < Animations.size()) {
		if (Animations.at(i).codeID == code) {
			activeAnimation = Animations.at(i).codeID;
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
	activeAnimation = an.codeID;
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
			activeSheet = Tilesheets.at(i).codeID;
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
	activeSheet = s.codeID;
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
#pragma endregion

#pragma region Light functions
void LGTAssign(unsigned int codeID)
{
	unsigned int i = 0;
	while (i < Lights.size()) {
		if (Lights.at(i).codeID == codeID) {
			activeLight = Lights.at(i).codeID;
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
	activeLight = l.codeID;
	return l.codeID;
}

void LGTDestroy(unsigned int codeID)
{
	unsigned int i = 0;
	while (i < Lights.size()) {
		if (Lights.at(i).codeID == codeID) {
			Lights.erase(Lights.begin() + i);
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

void LGTSetType(unsigned int type)
{
	Lights[activeLight].type = type;
}

unsigned int LGTGetType()
{
	return Lights[activeLight].type;
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

void EVTSetMousePressed(void(*func)(unsigned int button, int x, int y))
{
	fMousePressed = func;
}

void EVTSetMouseReleased(void(*func)(unsigned int button, int x, int y))
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

void EVTAddSpecial(const char name[], int key)
{
	KbdEvent* evt = new KbdEvent;
	evt->special = key;
	evt->id = std::string(name);
	keys.push_back(evt);
}

void EVTClearKeys()
{
	keys.clear();
}

void EVTGetMousePos(int pos[])
{
	pos[0] = msex;
	pos[1] = msey;
}

void EVTSetTriggerCallback(void(*func)(const char ID[], unsigned int entityID))
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
			activeTrigger = Triggers.at(i).codeID;
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
	activeTrigger = t.codeID;
	return t.codeID;
}

void TRGDestroy(unsigned int codeID)
{
	unsigned int i = 0;
	while (i < Triggers.size()) {
		if (Triggers.at(i).codeID == codeID) {
			Triggers.erase(Triggers.begin() + i);
			break;
		}
		i++;
	}
}

void TRGSetSize(unsigned int width, unsigned int height)
{
	Triggers[activeTrigger].width = width;
	Triggers[activeTrigger].height = height;
}

void TRGSetX(unsigned int x)
{
	Triggers[activeTrigger].x = x;
}

void TRGSetY(unsigned int y)
{
	Triggers[activeTrigger].y = y;
}

void TRGSetID(const char id[])
{
	Triggers[activeTrigger].id = std::string(id);
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
	char* charID = new char[Triggers[activeTrigger].id.size() + 1];
	strcpy_s(charID, Triggers[activeTrigger].id.size() + 1, Triggers[activeTrigger].id.c_str());
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
	activeSound = s.codeID;

	ALuint source;

	alGenSources(1, &source);
	getAlError();

	alSource3f(source, AL_VELOCITY, 0, 0, 0);
	getAlError();

	s.setSourceID(source);

	Sounds.push_back(s);

	return s.codeID;
}

void AUDAssignSource(unsigned int codeID)
{
	unsigned int i = 0;
	while (i < Sounds.size()) {
		if (Sounds.at(i).codeID == codeID) {
			activeSound = Sounds.at(i).codeID;
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