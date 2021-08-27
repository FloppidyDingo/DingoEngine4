#pragma once
#include "DE4Types.h"
#include "Utils.h"
#include "GLFW/glfw3.h"
#include <string.h>
#include <iostream>


/*
Definitions of the base engine data types
*/

//Entity Class--------------------------------------------------------------------------------
#pragma region Entity
bool Entity::isActive()
{
	return active;
}

bool Entity::isVisible()
{
	return visible;
}

bool Entity::isSolid()
{
	return solid;
}

TileSheet Entity::getTileSheet()
{
	return sheet;
}

std::string Entity::getID()
{
	return ID;
}

void Entity::getDirection(float vec[])
{
	vec[0] = dir[0];
	vec[1] = dir[1];
}

float Entity::getMass()
{
	return mass;
}

int Entity::getFrame()
{
	return frame;
}

int Entity::getWidth()
{
	return width;
}

int Entity::getHeight()
{
	return height;
}

float Entity::getScale()
{
	return scale;
}

float Entity::getOpacity()
{
	return opacity;
}

int Entity::getCollisionGroup()
{
	return collisionGroup;
}

bool Entity::isInvertX()
{
	return invertX;
}

bool Entity::isInvertY()
{
	return invertY;
}

Animation Entity::getAnimation(int index)
{
	return Animations.at(index);
}

void Entity::setActive(bool act){
	this->active = act;
}

void Entity::setVisible(bool vis){
	this->visible = vis;
}

void Entity::setSolid(bool sol)
{
	this->solid = sol;
}

void Entity::setTileSheet(TileSheet& tilesheet)
{
	this->sheet = tilesheet;
	setFrame(0);
}

void Entity::setFrame(int tileFrame)
{
	this->frame = tileFrame;
	this->width = sheet.widthList[frame];
	this->height = sheet.heightList[frame];
}

void Entity::setMass(float mass)
{
	this->mass = mass;
}

void Entity::setID(std::string id)
{
	this->ID = id;
}

void Entity::setDirection(float vec[])
{
	dir[0] = vec[0];
	dir[1] = vec[1];
}

void Entity::setDirection(float x, float y)
{
	dir[0] = x;
	dir[1] = y;
}

void Entity::setScale(float scale)
{
	this->scale = scale;
}

void Entity::setOpacity(float opacity)
{
	this->opacity = opacity;
}

void Entity::setCollisionGroup(int group)
{
	this->collisionGroup = group;
}

void Entity::addAnimation(Animation ani)
{
	Animations.push_back(ani);
}

void Entity::removeAnimation(Animation ani)
{
	unsigned int i = 0;
	while (i < Animations.size()) {
		Animation a1 = Animations.at(i);
		if (a1.codeID == ani.codeID) {
			Animations.erase(Animations.begin() + i);
			break;
		}
		i++;
	}
}

void Entity::removeAnimation(int index)
{
	Animations.erase(Animations.begin() + index);
}

void Entity::clearAnimations()
{
	Animations.clear();
}

void Entity::setBehavior(void(*func)(unsigned int id))
{
	fBehavior = func;
}

void Entity::setInvertX(bool invert)
{
	invertX = invert;
}

void Entity::setInvertY(bool invert)
{
	invertY = invert;
}

/*Update Entity
	Call behavior function
*/
void Entity::update()
{
	if (fBehavior != NULL) {
		fBehavior(Entity::codeID);
	}
	for (unsigned int i = 0; i < Animations.size(); i++) {
		if (Animations[i].isRunning()) {
			setFrame(Animations[i].getCurrentFrame());
		}
	}
}

int Entity::getTextureID()
{
	return sheet.idList[frame];
}

#pragma endregion

//Animation-----------------------------------------------------------------------------------
#pragma region Animation
void Animation::setID(std::string id)
{
	this->ID = id;
}

void Animation::setFrameSpacing(int space)
{
	this->frameSpacing = space;
}

void Animation::setRunning(bool running)
{
	this->running = running;
}

void Animation::setSinglePlay(bool singlePlay)
{
	this->singlePlay = singlePlay;
}

std::string Animation::getID()
{
	return ID;
}

int Animation::getFrameSpacing()
{
	return frameSpacing;
}

int Animation::getCurrentFrame()
{
	return currentFrame;
}

int Animation::getCurrentTile()
{
	return currentTile;
}

bool Animation::isRunning()
{
	return running;
}

bool Animation::isSinglePlay()
{
	return singlePlay;
}

void Animation::addFrame(int tileIndex)
{
	idList.push_back(tileIndex);
}

void Animation::clearFrames()
{
	idList.clear();
}

int Animation::update()
{
	if (running) {
		if (currentFrame == frameSpacing) {
			currentFrame = -1;
			currentTile++;
			if (currentTile > idList.size() - 1) {
				if (singlePlay) {
					running = false;
				}
				currentTile = 0;
			}
			return idList.at(currentTile);
		}
		currentFrame++;
	}
	return -1;
}
#pragma endregion

//Scene---------------------------------------------------------------------------------------
#pragma region Scene
void Scene::addEntity(Entity e)
{
	Entities.push_back(e);
}

void Scene::removeEntity(Entity e)
{
	unsigned int i = 0;
	while (i < Entities.size()) {
		Entity a1 = Entities.at(i);
		if (a1.codeID == e.codeID) {
			Entities.erase(Entities.begin() + i);
			break;
		}
		i++;
	}
}

void Scene::clearEntities()
{
	Entities.clear();
}

void Scene::addGUI(Entity e)
{
	GUI.push_back(e);
}

void Scene::removeGUI(Entity e)
{
	unsigned int i = 0;
	while (i < GUI.size()) {
		Entity a1 = GUI.at(i);
		if (a1.codeID == e.codeID) {
			GUI.erase(GUI.begin() + i);
			break;
		}
		i++;
	}
}

void Scene::clearGUI()
{
	GUI.clear();
}

void Scene::addLight(Light l)
{
	Lights.push_back(l);
}

void Scene::removeLight(Light l)
{
	unsigned int i = 0;
	while (i < Lights.size()) {
		Light a1 = Lights.at(i);
		if (a1.codeID == l.codeID) {
			GUI.erase(GUI.begin() + i);
			break;
		}
		i++;
	}
}

void Scene::clearLights()
{
	Lights.clear();
}

void Scene::addTrigger(Trigger t)
{
	Triggers.push_back(t);
}

void Scene::removeTrigger(Trigger t)
{
	unsigned int i = 0;
	while (i < Triggers.size()) {
		Trigger a1 = Triggers.at(i);
		if (a1.codeID == t.codeID) {
			Triggers.erase(Triggers.begin() + i);
			break;
		}
		i++;
	}
}

void Scene::clearTriggers()
{
	Triggers.clear();
}

std::vector<Entity> Scene::getEntities()
{
	return Entities;
}

std::vector<Entity> Scene::getGUI()
{
	return GUI;
}

std::vector<Light> Scene::getLights()
{
	return Lights;
}

std::vector<Trigger> Scene::getTriggers()
{
	return Triggers;
}
#pragma endregion

//Tilesheet-----------------------------------------------------------------------------------
#pragma region Tilesheet
std::string TileSheet::getID() {
	return this->ID;
}
void TileSheet::createSheet(std::string url_base, std::string url_def)
{
	//split file name by slash mark, use last index to find file name
	std::vector<std::string> fileSplit;
	stringSplit(url_base, '/', fileSplit);
	this->ID = fileSplit[fileSplit.size() - 1];

	//load base and bounds images
	unsigned int width = 0;
	unsigned int height = 0;
	loadImage(imageData, width, height, url_base.c_str(), false);
	loadImage(boundsData, width, height, url_def.c_str(), true);
	unsigned char* subImage;
	int phase = 0;
	int stx; //subimage starting x coord
	int sty; //subimage starting y coord
	for (int y1 = 0; y1 < height; y1++) {
		for (int x1 = 0; x1 < width; x1++) {
			switch (phase) {
				case 0: { //initial search phase
					if (boundsData[(y1 * width) + x1] > 0) {
						//move to search part 2 phase when dot is found
						boundsData[(y1 * width) + x1] = 0; //clear point so it is not counted again
						//save starting coords
						stx = x1;
						sty = y1;
						phase = 1;
					}
					break;
				}
				case 1: {// search part 2 phase
					if (boundsData[(y1 * width) + x1] > 0) {
						//move to Y scan when second point is found
						boundsData[(y1 * width) + x1] = 0;
						int edx = x1; //save ending x value
						int edy = 0;
						for (int y2 = 0; y2 < height; y2++) {//Search down Y axis for last point
							if (boundsData[(y2 * width) + x1] > 0) {
								edy = y2; //save ending y value
								boundsData[(y2 * width) + x1] = 0;
								break;
							}
						}
						if (edy == 0) { //validate final point was actually found
							std::cerr << "Error in bounds file, Point 3 not found. Sprites not generated" <<std::endl;
							error("Error in bounds file, Point 3 not found. Sprites not generated");
							return;
						}
						else { //crop base image using starting and ending coordinates
							phase = 0;
							//multiply x coordinates by 4 to scale to the base and subimages RGBA format
							//stx = stx * 4;
							//edx = edx * 4;
							int subWidth = (edx - stx) + 1;
							int subHeight = (edy - sty) + 1;
							subImage = new unsigned char[subWidth * subHeight * 4];
							for (int y2 = 0; y2 < subHeight; y2++) { //copy image data from main image to subimage
								for (int x2 = 0; x2 < subWidth; x2 ++) {
									int mainIndex = (((stx + (x2)) + (width * (sty + y2)))) * 4;
									int subIndex = ((x2 * 4) + (subWidth * y2 * 4));
									subImage[subIndex] = imageData[mainIndex];
									subImage[subIndex + 1] = imageData[mainIndex + 1];
									subImage[subIndex + 2] = imageData[mainIndex + 2];
									subImage[subIndex + 3] = imageData[mainIndex + 3];
								}
							}
							
							//register subimage as an OPENGL texture
							unsigned int  textureID;
							glGenTextures(1, &textureID);
							glBindTexture(GL_TEXTURE_2D, textureID);
							glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
							glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
							glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
							glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
							glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, subWidth, subHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, subImage);
							delete[] subImage;
							//add ID to ID list
							idList.push_back(textureID);
							widthList.push_back(subWidth);
							heightList.push_back(subHeight);
							log("Texture generated " + this->ID + " ID " + std::to_string(textureID));
						}
					}
					break;
				}
			}
		}
		if (phase == 1) { //Throw error if reach the end of X coord while in second phase (Second point obviously missing)
			error("Error in bounds file, Point 2 not found. Sprites not generated");
			return;
		}
		
	}
	delete[] imageData;
	delete[] boundsData;
}
#pragma endregion

//Sound---------------------------------------------------------------------------------------
#pragma region Sound

void Sound::setSourceID(ALuint source)
{
	Sound::sourceID = source;
}

void Sound::setVolume(float volume)
{
	Sound::volume = volume;
	alSourcef(Sound::sourceID, AL_GAIN, volume);
}

void Sound::setBalance(float balance)
{
	Sound::balance = balance;
	if (!Sound::spatial) {
		alSource3f(Sound::sourceID, AL_POSITION, balance, 0, 0);
	}
}

void Sound::setX(float x)
{
	Sound::x = x;
	if (Sound::spatial == true) {
		alSource3f(Sound::sourceID, AL_POSITION, x, Sound::y, 0);
	}
}

void Sound::setY(float y)
{
	Sound::y = y;
	if (Sound::spatial == true) {
		alSource3f(Sound::sourceID, AL_POSITION, Sound::x, y, 0);
	}
}

void Sound::setLooping(bool loop)
{
	Sound::loop = loop;
	alSourcei(Sound::sourceID, AL_LOOPING, loop);
}

void Sound::setSpatial(bool spatial)
{
	Sound::spatial = spatial;
	if (spatial) {
		alSource3f(Sound::sourceID, AL_POSITION, Sound::x, Sound::y, 0);
	}
	else {
		alSource3f(Sound::sourceID, AL_POSITION, Sound::balance, 0, 0);
	}
}

void Sound::setTimeStamp(float time)
{
}

float Sound::getVolume()
{
	return Sound::volume;
}

float Sound::getBalance()
{
	return Sound::balance;
}

float Sound::getX()
{
	return Sound::x;
}

float Sound::getY()
{
	return Sound::y;
}

float Sound::getTimeStamp()
{
	return 0.0f;
}

bool Sound::isLooping()
{
	return Sound::loop;
}

bool Sound::isSpatial()
{
	return Sound::spatial;
}

bool Sound::isPlaying()
{
	ALint state;
	alGetSourcei(Sound::sourceID, AL_SOURCE_STATE, &state);
	if (state == AL_PLAYING) {
		return true;
	}
	return false;
}

void Sound::play(ALuint buffer)
{
	alSourcei(Sound::sourceID, AL_BUFFER, buffer);
	alSourcePlay(Sound::sourceID);
}

void Sound::stop()
{
	alSourceStop(Sound::sourceID);
}

void Sound::pause()
{
	alSourcePause(Sound::sourceID);
}

void Sound::reset()
{
	alSourceRewind(Sound::sourceID);
}

#pragma endregion
