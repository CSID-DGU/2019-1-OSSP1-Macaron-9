/*
 * Renderer.h
 *
 *  Created on: Aug 16, 2015
 *      Author: axel
 */

#ifndef RENDERER_H_
#define RENDERER_H_

#include "../SDLContext.h"
#include <SDL2/SDL.h>

class Renderer {
public:
	Renderer();
	virtual ~Renderer();		//가상함수
	//Inherit to get access to the SDL renderer.
	SDLContext& _SDLContext;
	SDL_Texture* _texture;
	SDL_Renderer* _SDLRenderer;
	SDL_Texture* _spriteSheet;			// sprite : 한 장씩 독립된 이미지의 단위
};

#endif /* RENDERER_H_ */
