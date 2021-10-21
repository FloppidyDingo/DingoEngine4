#pragma once
#include <stdlib.h>
#include <string>
#include <vector>

#pragma region button
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
#pragma endregion

#pragma region text
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
		bool visible = true;
		std::string text;
		std::vector<unsigned int> entityList;
		unsigned int scene;

	public:
		void setX(float x);
		void setY(float y);
		void setPosition(float x, float y);
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
		void setVisible(bool visible);
		bool isVisible();

		void update();
		void setText(std::string text);
		std::string getText();
		void registerScene(unsigned int scene);
};
#pragma endregion

class DE4UIManager {
	public:
		void registerText(DE4Text& text);
		void registerButton(DE4Button& button);
		void update();

	private:
		std::vector<DE4Text*> textList;
		std::vector<DE4Button*> buttonList;
};

