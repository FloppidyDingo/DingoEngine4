#pragma once

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
