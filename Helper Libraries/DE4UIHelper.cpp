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
