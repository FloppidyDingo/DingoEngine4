//TODO
// Toggle button
// Text (still, scrolling, and RPG style)
// Slider
//

#include <stdlib.h>
#include <stdio.h>
#include <iostream>

#include "DE4UIHelper.h"
#include "DingoEngine4.h"

#pragma region buttons
void DE4Button::setEntity(unsigned int entID, unsigned int idle, unsigned int hover, unsigned int click)
{
	entityID = entID;
	idleFrame = idle;
	hoverFrame = hover;
	clickFrame = click;
}

void DE4Button::update()
{
	int mousePos[2];
	EVTGetMousePos(mousePos);
	ENTAssign(entityID);
	int mseX = mousePos[0];
	int mseY = mousePos[1];
	int entWidth = ENTGetWidth();
	int entHeight = ENTGetHeight();
	int entX = ENTGetX();
	int entY = ENTGetY();
	if ((mseX < (entX + (entWidth / 2))) && (mseX > (entX - (entWidth / 2))) && (mseY > (entY - (entHeight / 2))) && (mseY < (entY + (entHeight / 2))) && ENTIsVisible()) {
		if (!click) {
			ENTSetFrame(hoverFrame);
		}
		hover = true;
	} else {
		ENTSetFrame(idleFrame);
		hover = false;
		click = false;
	}
}

void DE4Button::setClickFunc(void(*func)()) {
	fClick = func;
}

void DE4Button::clickPress() {
	if (hover) {
		click = true;
		ENTAssign(entityID);
		ENTSetFrame(clickFrame);
	}
}

void DE4Button::clickRelease() {
	if (hover) {
		ENTAssign(entityID);
		ENTSetFrame(hoverFrame);
		if (click && (fClick != nullptr)) {
			fClick();
			hover = false;
		}
	}
}

unsigned int DE4Button::getID() {
	return entityID;
}
#pragma endregion

void DE4Text::setX(float x) {
	this->x = x;
	updateNeeded = true;
}

void DE4Text::setY(float y) {
	this->y = y;
	updateNeeded = true;
}

void DE4Text::setFont(int font) {
	this->font = font;
	updateNeeded = true;
}

void DE4Text::setFontSize(float fontSize) {
	this->fontSize = fontSize;
	updateNeeded = true;
}

float DE4Text::getX() {
	return this->x;
}

float DE4Text::getY() {
	return this->y;
}

int DE4Text::getFont() {
	return this->font;
}

float DE4Text::getFontSize() {
	return this->fontSize;
}

void DE4Text::setLineHeight(int height) {
	this->lineHeight = height;
	updateNeeded = true;
}

int DE4Text::getLineHeight() {
	return lineHeight;
}

void DE4Text::setTileSheet(unsigned int tileSheet) {
	this->tile = tileSheet;
	updateNeeded = true;
}

unsigned int DE4Text::getTileSheet() {
	return this->tile;
}

void DE4Text::update() {
	if (updateNeeded) {
		curX = 0;
		curY = 0;
		SCNAssign(scene);

		//remove all entities on entity list from scene
		for (unsigned int id : entityList) {
			SCNRemoveGUI(id);
		}
		entityList.clear();

		//loop through string
		for (int index = 0; index < text.size(); index++) {
			//extract tile ID from character
			int c = text.data()[index];

			//check if character is newline
			if (c == '\n') {
				//newline if character is newline
				curX = 0;
				curY -= lineHeight;
			} else {
				//convert character to tile ID
				c -= 0x20;

				//generate entity
				unsigned int entity = ENTCreate();
				ENTSetTileSheet(tile);
				ENTSetFrame(c + (font * 95));
				ENTSetScale(fontSize);

				//position entity
				ENTSetPosition(curX + x, curY + y);
				curX += ENTGetWidth();

				//add entity to scene and entity list
				entityList.push_back(entity);
				SCNAddGUI();
			}
		}
	}
	updateNeeded = false;
	
}

void DE4Text::setText(std::string text) {
	//setup variables
	this->text = text;
	updateNeeded = true;
}

std::string DE4Text::getText() {
	return this->text;
}

void DE4Text::registerScene(unsigned int scene) {
	this->scene = scene;
}
