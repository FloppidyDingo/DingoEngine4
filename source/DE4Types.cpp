#pragma once
#include "DE4Types.h"
#include "Utils.h"
#include "GLFW/glfw3.h"
#include "DingoEngine4.h"

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

float Entity::getMass()
{
	return mass;
}

int Entity::getFrame()
{
	return frame;
}

float Entity::getWidth()
{
	return width * scale * scaleX;
}

float Entity::getHeight()
{
	return height * scale * scaleY;
}

float Entity::getPhysWidth() {
	if (physOverride) {
		return physWidth;
	} else {
		return width * scale * scaleX;
	}
}

float Entity::getPhysHeight() {
	if (physOverride) {
		return physHeight;
	} else {
		return height * scale * scaleX;
	}
}

float Entity::getScale()
{
	return scale;
}

float Entity::getScaleX() {
	return this->scaleX;
}

float Entity::getScaleY() {
	return this->scaleY;
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

unsigned int Entity::getAnimation(int index)
{
	return Animations.at(index);
}

void Entity::stopAllAnimations() {
	for (unsigned int i = 0; i < Animations.size(); i++) {
		ANIAssign(Animations[i]);
		ANISetRunning(false);
	}
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

void Entity::setScale(float scale)
{
	this->scale = scale;
}

void Entity::setScaleX(float scale) {
	this->scaleX = scale;
}

void Entity::setScaleY(float scale) {
	this->scaleY = scale;
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
	Animations.push_back(ani.codeID);
}

void Entity::removeAnimation(Animation ani)
{
	unsigned int i = 0;
	while (i < Animations.size()) {
		if (Animations[i] == ani.codeID) {
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

void Entity::setPhysOverride(bool ov) {
	physOverride = ov;
}

void Entity::setPhysWidth(float pw) {
	this->physWidth = pw;
}

void Entity::setPhysHeight(float ph) {
	this->physHeight = ph;
}

/*Update Entity
	Call behavior function
	check animations
*/
void Entity::update()
{
	if (fBehavior != NULL) {
		fBehavior(Entity::codeID);
	}
	for (unsigned int i = 0; i < Animations.size(); i++) {
		ANIAssign(Animations[i]);
		if (ANIIsRunning()) {
			setFrame(ANIGetCurrentTile());
			return;
		}
	}
}

int Entity::getTextureID()
{
	return sheet.textureID;
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
	if (this->running == running) {
		return;
	}
	this->running = running;
	if (running && this->singlePlay) {
		this->currentFrame = currentFrame;
		this->currentTile = 0;
	}
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
	return idList[currentTile];
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
	if (stop) {
		stop = false;
		running = false;
	}
	if (running) {
		if (currentFrame == frameSpacing) {
			currentFrame = -1;
			currentTile++;
			if (singlePlay && currentTile == idList.size() - 1) {
				stop = true;
			} else if (!singlePlay && currentTile == idList.size()) {
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
void Scene::addEntity(unsigned int id, unsigned int index)
{
	entry e;
	e.id = id;
	e.index = index;
	Entities.push_back(e);
}

void Scene::removeEntity(unsigned int id)
{
	unsigned int i = 0;
	while (i < Entities.size()) {
		if (Entities[i].id == id) {
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

void Scene::addGUI(unsigned int id, unsigned int index)
{
	entry e;
	e.id = id;
	e.index = index;
	GUI.push_back(e);
}

void Scene::removeGUI(unsigned int id)
{
	unsigned int i = 0;
	while (i < GUI.size()) {
		if (GUI[i].id == id) {
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

void Scene::addLight(unsigned int id, unsigned int index)
{
	entry l;
	l.id = id;
	l.index = index;
	Lights.push_back(l);
}

void Scene::removeLight(unsigned int id)
{
	unsigned int i = 0;
	while (i < Lights.size()) {
		if (Lights.at(i).id == id) {
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

void Scene::addTrigger(unsigned int id, unsigned int index)
{
	entry t;
	t.id = id;
	t.index = index;
	Triggers.push_back(t);
}

void Scene::removeTrigger(unsigned int id)
{
	unsigned int i = 0;
	while (i < Triggers.size()) {
		if (Triggers.at(i).id == id) {
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

void Scene::addGUIText(unsigned int id, unsigned int index) {
	entry t;
	t.id = id;
	t.index = index;
	Texts.push_back(t);
}

void Scene::removeGUIText(unsigned int id) {
	unsigned int i = 0;
	while (i < Texts.size()) {
		if (Texts.at(i).id == id) {
			Texts.erase(Texts.begin() + i);
			break;
		}
		i++;
	}
}

void Scene::clearGUIText() {
	Texts.clear();
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

	//register image as an OPENGL texture
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
	log("Building atlas " + this->ID + " ID " + std::to_string(textureID));
	this->textureID = textureID;

	int phase = 0;
	int stx; //subimage starting x coord
	int sty; //subimage starting y coord
	int spriteCount = 0; //sprite count, used for logging only
	for (unsigned int y1 = 0; y1 < height; y1++) {
		for (unsigned int x1 = 0; x1 < width; x1++) {
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
						for (unsigned int y2 = 0; y2 < height; y2++) {//Search down Y axis for last point
							if (boundsData[(y2 * width) + x1] > 0) {
								edy = y2; //save ending y value
								boundsData[(y2 * width) + x1] = 0;
								break;
							}
						}
						if (edy == 0) { //validate final point was actually found
							std::string message = "Error in bounds file, Point 3 not found (Sprite ";
							message += std::to_string(spriteCount);
							message += "), sprites may be missing";
							error(message);
							phase = 0;
						}
						else { //create frame coordinates
							phase = 0;

							//calulate frame width and height
							int subWidth = (edx - stx) + 1;
							int subHeight = (edy - sty) + 1;
							edy++;

							//calculate frame UV coordinates
							atlasTile tile;
							tile.tlu = ((float)stx + 0.001) / (float)width;
							tile.tlv = (float)sty / (float)height;
							tile.tru = ((float)(edx + 1) - 0.001) / (float)width;
							tile.trv = (float)sty / (float)height;
							tile.blu = ((float)stx + 0.001) / (float)width;
							tile.blv = (float)edy / (float)height;
							tile.bru = ((float)(edx + 1) - 0.001) / (float)width;
							tile.brv = (float)edy / (float)height;
							
							//add frame to atlas
							this->atlas.push_back(tile);
							widthList.push_back((float)subWidth);
							heightList.push_back((float)subHeight);
							spriteCount++;
						}
					}
					break;
				}
			}
		}
		if (phase == 1) { //Throw error if reach the end of X coord while in second phase (Second point obviously missing)
			std::string message = "Error in bounds file, Point 2 not found (Sprite ";
			message += std::to_string(spriteCount);
			message += "), sprites may be missing";
			error(message);
			phase = 0;
		}
		
	}
	delete[] imageData;
	delete[] boundsData;
	log(std::to_string(spriteCount) + " sprites made");
}
#pragma endregion

//Sound---------------------------------------------------------------------------------------
#pragma region Sound

float SFXVolume;
float musVolume;

void Sound::setSourceID(ALuint source)
{
	Sound::sourceID = source;
}

void Sound::setVolume(float volume)
{
	Sound::volume = volume;
	if (type) {
		alSourcef(Sound::sourceID, AL_GAIN, volume * musVolume);
	} else {
		alSourcef(Sound::sourceID, AL_GAIN, volume * SFXVolume);
	}
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

void Sound::setMusic(bool music) {
	this->type = music;
	this->setVolume(this->volume);
}

bool Sound::isMusic() {
	return this->type;
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

void Sound::setTimeStamp(float time){

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

void settingSetSFXVolume(float volume) {
	SFXVolume = volume;
}

float settingGetSFXVolume() {
	return SFXVolume;
}

void settingSetMusicVolume(float volume) {
	musVolume = volume;
}

float settingGetMusicVolume() {
	return musVolume;
}

#pragma endregion

//Light---------------------------------------------------------------------------------------
#pragma region Light

std::string Light::getID() {
	return this->ID;
}

void Light::setID(std::string id) {
	this->ID = id;
}
#pragma endregion

//Trigger-------------------------------------------------------------------------------------
#pragma region Trigger

std::string Trigger::getID() {
	return this->ID;
}

void Trigger::setID(std::string id) {
	this->ID = id;
}
#pragma endregion

//Game State----------------------------------------------------------------------------------
void GameState::setEnabled(bool enabled) {
	if (this->fOnEnableChange != nullptr) {
		fOnEnableChange(enabled);
	}
	this->enabled = enabled;
}

bool GameState::isEnabled() {
	return this->enabled;
}
