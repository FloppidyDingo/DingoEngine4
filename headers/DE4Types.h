#pragma once
#include <string>
#include <vector>
#include <algorithm>
#include "al.h"

struct KbdEvent {
	unsigned int key = 0;
	std::string id = "";
};

struct entry {
	unsigned int id;
	unsigned int index;
};

struct atlasTile {
	float tlu;
	float tlv;
	float tru;
	float trv;
	float blu;
	float blv;
	float bru;
	float brv;
};


//Common data types used in DE4
#pragma region Tilesheet
class TileSheet {
private:
	unsigned char* imageData = 0;
	unsigned char* boundsData = 0;
	std::string ID;

public:
	unsigned int codeID;
	unsigned int textureID;
	std::string getID();
	void createSheet(std::string url_base, std::string url_def);
	std::vector<atlasTile> atlas;
	std::vector<float> widthList;
	std::vector<float> heightList;
};

class GameState {
private:
	bool enabled = false;

public:
	unsigned int codeID;

	void (*fUpdate)();
	void (*fKeyPressed)(const char* id);
	void (*fKeyReleased)(const char* id);
	void (*fCollide)(unsigned int ida, unsigned int idb);
	void (*fKeyboardEvent)();
	void (*fMousePressed)(unsigned int button, float x, float y);
	void (*fMouseReleased)(unsigned int button, float x, float y);
	void (*fTrigger)(unsigned int trigID, unsigned int entID);
	void (*fOnEnableChange)(bool enabled);

	void setEnabled(bool enabled);
	bool isEnabled();
};
#pragma endregion

#pragma region Light
class Light {
private:
	std::string ID;

public:
	unsigned int codeID;
	float type;
	float radius = 1;
	float brightness = 1;
	float x = 0;
	float y = 0;
	float rgb[3];
	std::string getID();
	void setID(std::string id);
};
#pragma endregion

#pragma region Animation
class Animation{
private:
	std::string ID;
	std::vector<int> idList;
	int frameSpacing = 1;
	int currentFrame = 0;
	unsigned int currentTile = 0;
	bool running = false;
	bool singlePlay = true;
	bool stop = false;

public:
	unsigned int codeID;
	void setID(std::string id);
	void setFrameSpacing(int space);
	void setRunning(bool running);
	void setSinglePlay(bool singlePlay);
	std::string getID();
	int getFrameSpacing();
	int getCurrentFrame();
	int getCurrentTile();
	bool isRunning();
	bool isSinglePlay();
	void addFrame(int tileIndex);
	void clearFrames();

	int update();
};
#pragma endregion

#pragma region Entity
class Entity {
private:
	TileSheet sheet;
	std::string ID;
	float mass = 0;
	bool visible = true;
	bool solid = false;
	bool active = true;
	bool physOverride = false;
	int frame = 0;
	float width;
	float height;
	float physWidth;
	float physHeight;
	float scale = 1;
	float scaleX = 1;
	float scaleY = 1;
	float opacity = 1;
	int collisionGroup = 0;
	bool invertX = false;
	bool invertY = false;
	std::vector<unsigned int> Animations;
	void (*fBehavior)(unsigned int id);

public:
	bool forceMove = false;
	float x = 0;
	float y = 0;
	float dir[2];
	float friction[4];
	unsigned int codeID;
	bool isActive();
	bool isVisible();
	bool isSolid();
	TileSheet getTileSheet();
	std::string getID();
	float getMass();
	int getFrame();
	float getWidth();
	float getHeight();
	float getPhysWidth();
	float getPhysHeight();
	float getScale();
	float getScaleX();
	float getScaleY();
	float getOpacity();
	int getCollisionGroup();
	bool isInvertX();
	bool isInvertY();
	unsigned int getAnimation(int index);
	void stopAllAnimations();

	void setActive(bool act);
	void setVisible(bool vis);
	void setSolid(bool sol);
	void setTileSheet(TileSheet& tilesheet);
	void setFrame(int tileFrame);
	void setMass(float mass);
	void setID(std::string id);
	void setScale(float scale);
	void setScaleX(float scale);
	void setScaleY(float scale);
	void setOpacity(float opacity);
	void setCollisionGroup(int group);
	void addAnimation(Animation ani);
	void removeAnimation(Animation ani);
	void removeAnimation(int index);
	void clearAnimations();
	void setBehavior(void (*func)(unsigned int id));
	void setInvertX(bool invert);
	void setInvertY(bool invert);
	void setPhysOverride(bool ov);
	void setPhysWidth(float pw);
	void setPhysHeight(float ph);

	void update();
	int getTextureID();
};
#pragma endregion

#pragma region Trigger
class Trigger {
private:
	std::string ID;

public:
	std::string getID();
	void setID(std::string id);
	unsigned int codeID;
	float x = 0;
	float y = 0;
	float width = 0;
	float height = 0;
	bool enabled = false;
};
#pragma endregion

#pragma region Scene
class Scene {

public:
	std::vector<entry> Entities;
	std::vector<entry> GUI;
	std::vector<entry> Lights;
	std::vector<entry> Triggers;
	std::vector<entry> Texts;
	unsigned int codeID;
	void addEntity(unsigned int id, unsigned int index);
	void removeEntity(unsigned int id);
	void clearEntities();
	void addGUI(unsigned int id, unsigned int index);
	void removeGUI(unsigned int id);
	void clearGUI();
	void addLight(unsigned int id, unsigned int index);
	void removeLight(unsigned int id);
	void clearLights();
	void addTrigger(unsigned int id, unsigned int index);
	void removeTrigger(unsigned int id);
	void clearTriggers();
	void addGUIText(unsigned int id, unsigned int index);
	void removeGUIText(unsigned int id);
	void clearGUIText();

};
#pragma endregion

#pragma region Sound
class Sound {
private:
	float volume = 0;
	float balance = 0;
	float type;
	float x = 0;
	float y = 0;
	bool loop = false;
	bool spatial = false;
public:
	ALuint sourceID;
	void setSourceID(ALuint source);
	unsigned int codeID;
	void setVolume(float volume);
	void setBalance(float balance);
	void setX(float x);
	void setY(float y);
	void setMusic(bool music);
	bool isMusic();
	void setLooping(bool loop);
	void setSpatial(bool spatial);
	void setTimeStamp(float time);
	float getVolume();
	float getBalance();
	float getX();
	float getY();
	float getTimeStamp();
	bool isLooping();
	bool isSpatial();
	bool isPlaying();
	void play(ALuint buffer);
	void stop();
	void pause();
	void reset();
};

void settingSetSFXVolume(float volume);
float settingGetSFXVolume();
void settingSetMusicVolume(float volume);
float settingGetMusicVolume();
#pragma endregion

#pragma region Text
class Text {
	public:
		unsigned int codeID;
		unsigned int lineWidth = 0;
		float x;
		float y;
		unsigned int font;
		float scale;
		TileSheet sheet;
		std::string ID;
		std::string text;
		float lineHeight;
		unsigned int lineCount;
		float width;
		float height;
		bool visible;
		
	private:
		
};
#pragma endregion
