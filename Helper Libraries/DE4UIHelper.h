#pragma once
#include <stdlib.h>
#include <string>
#include <vector>

class DE4Button {
	private:
		unsigned int entityID;
		unsigned int idleFrame;
		unsigned int hoverFrame;
		unsigned int clickFrame;
		bool hover;
		bool click;

		void (*fClick)();

	public:
		void setEntity(unsigned int entID, unsigned int idle, unsigned int hover, unsigned int click);
		void update();
		void setClickFunc(void (*func)());
		void clickPress();
		void clickRelease();

		unsigned int getID();
};

class DE4Text {
	private:
		int curX;
		int curY;
		float x;
		float y;
		unsigned int tile;
		int font;
		float fontSize = 1;
		int lineHeight = 8;
		bool updateNeeded;
		std::string text;
		std::vector<unsigned int> entityList;
		unsigned int scene;

	public:
		void setX(float x);
		void setY(float y);
		void setFont(int font);
		void setFontSize(float fontSize);
		float getX();
		float getY();
		int getFont();
		float getFontSize();
		void setLineHeight(int height);
		int getLineHeight();
		void setTileSheet(unsigned int tileSheet);
		unsigned int getTileSheet();

		void update();
		void setText(std::string text);
		std::string getText();
		void registerScene(unsigned int scene);
};
