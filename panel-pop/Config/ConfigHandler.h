/*
 * ConfigHandler.h
 *
 *  Created on: 11.2.2016
 *      Author: axelw
*/

#ifndef CONFIG_CONFIGHANDLER_H_
#define CONFIG_CONFIGHANDLER_H_

#include <boost/property_tree/ptree.hpp>
#include <SDL2/SDL_scancode.h>
#include "KeyConfig.h"

class ConfigHandler {
public:

	static ConfigHandler& getInstance();

	const char* CONFIG_FILENAME = "panelpop.ini";
	const KeyConfig DEFAULT_KEYS = {
			SDL_SCANCODE_UP, SDL_SCANCODE_DOWN, SDL_SCANCODE_LEFT,
			SDL_SCANCODE_RIGHT, SDL_SCANCODE_X, SDL_SCANCODE_Z, SDL_SCANCODE_Q }; // new. SD_SCANCODE_Q

	bool loadConfig();
	bool saveConfig();

	KeyConfig getKeyConfig(int player);
	void setKeyConfig(KeyConfig, int player);
	void setFullscreen(bool);
	void setMusicVolume(int);
	void setSfxVolume(int);

	bool getFullscreen();
	int getMusicVolume();
	int getSfxVolume();

	int getEndlessFinalLevel(); // new
	void setEndlessFinalLevel(int); // new
	
	/* -------- P1 Item -------- */
	int getNumberOfP1Bomb();
	void setNumberOfP1Bomb(int);
	int getNumberOfP1Cross();
	void setNumberOfP1Cross(int);
	int getNumberOfP1SameColor();
	void setNumberOfP1SameColor(int);

	/* -------- P2 Item -------- */
	int getNumberOfP2Bomb();
	void setNumberOfP2Bomb(int);
	int getNumberOfP2Cross();
	void setNumberOfP2Cross(int);
	int getNumberOfP2SameColor();
	void setNumberOfP2SameColor(int);
	
	
	int getEndlessHighScore();
	void setEndlessHighScore(int);

private:
	ConfigHandler();
	ConfigHandler(ConfigHandler const&) = delete;
	void operator=(ConfigHandler const&) = delete;

	boost::property_tree::ptree _settingsTree;
};

#endif /* CONFIG_CONFIGHANDLER_H_ */
