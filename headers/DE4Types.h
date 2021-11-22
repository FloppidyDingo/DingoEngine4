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

//Common data types used in DE4
#pragma region Tilesheet
class TileSheet {
private:
	unsigned char* imageData;
	unsigned char* boundsData;
	std::string ID;

public:
	unsigned int codeID;
	std::string getID();
	void createSheet(std::string url_base, std::string url_def);
	std::vector<unsigned int> idList;
	std::vector<unsigned int> widthList;
	std::vector<unsigned int> heightList;
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
	float mass;
	bool visible = true;
	bool solid;
	bool active = true;
	int frame;
	int width;
	int height;
	float scale = 1;
	float opacity = 1;
	int collisionGroup;
	bool invertX = false;
	bool invertY = false;
	std::vector<unsigned int> Animations;
	void (*fBehavior)(unsigned int id);

public:
	float x = 0;
	float y = 0;
	float dir[2];
	unsigned int codeID;
	bool isActive();
	bool isVisible();
	bool isSolid();
	TileSheet getTileSheet();
	std::string getID();
	void getDirection(float vec[]);
	float getMass();
	int getFrame();
	int getWidth();
	int getHeight();
	float getScale();
	float getOpacity();
	int getCollisionGroup();
	bool isInvertX();
	bool isInvertY();
	unsigned int getAnimation(int index);

	void setActive(bool act);
	void setVisible(bool vis);
	void setSolid(bool sol);
	void setTileSheet(TileSheet& tilesheet);
	void setFrame(int tileFrame);
	void setMass(float mass);
	void setID(std::string id);
	void setDirection(float vec[]);
	void setDirection(float x, float y);
	void setScale(float scale);
	void setOpacity(float opacity);
	void setCollisionGroup(int group);
	void addAnimation(Animation ani);
	void removeAnimation(Animation ani);
	void removeAnimation(int index);
	void clearAnimations();
	void setBehavior(void (*func)(unsigned int id));
	void setInvertX(bool invert);
	void setInvertY(bool invert);

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

void setSFXVolume(float volume);
float getSFXVolume();
void setMusicVolume(float volume);
float getMusicVolume();
#pragma endregion
