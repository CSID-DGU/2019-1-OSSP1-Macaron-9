/*
 * StateManager.cpp
 *
 *  Created on: 9.12.2015
 *      Author: axelw
 */

#include "StateManager.h"

#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_scancode.h>
#include <SDL2/SDL_timer.h>
#include <iomanip>
#include <sstream>

#include "../InputManager.h"
#include "../SDLContext.h"
#include "../Config/ConfigHandler.h"
#include "EndlessGameState.h"
#include "TitleScreen.h"
#include "FadeTransition.h"

StateManager::StateManager() :
_currentState(nullptr),
_newState(nullptr),
_switchState(false),
SDL(SDLContext::getInstance()),
input(InputManager::getInstance()),
_lastTick(0),
_frameTime(16),
_running(true),
_framesRun(0),
_startTime(SDL_GetTicks()),
_lastFrame(0),
_avgFps(0),
_showFps(false) {
    _p1keys = ConfigHandler::getInstance().getKeyConfig(1);
    _p2keys = ConfigHandler::getInstance().getKeyConfig(2);
    _currentState = new TitleScreen();
}

StateManager& StateManager::getInstance() {
    static StateManager instance;
    return instance;
}

void StateManager::run() {
    bool tick;
    bool render;
    while (_running) {
        tick = (SDL_GetTicks() - _lastTick >= _frameTime);		//SDL_GetTicks : SDL을 초기화 한 이후의 시간(ms 단위)
        render = (SDL_GetTicks() - _lastFrame >= 5);
        //200 fps cap
        if (tick) {
            input.poll();
            if (input._quit) {
                //TODO: State transitions?
                _running = false;
                break;
            } 
            if (input.keyPressed(SDL_SCANCODE_LALT)			// ALT + Return(Enter) : fullscreen
                    && input.keyDown(SDL_SCANCODE_RETURN)) {
                SDL.toggleFullscreen();
            }
            if (input.keyPressed(SDL_SCANCODE_LCTRL)		// Ctrl + F : fps 나타내기
                    && input.keyDown(SDL_SCANCODE_F)) {
                _showFps = !_showFps;
            }
            _currentState->tick();
        }
        if (render) {
            SDL_Texture* t = _currentState->render();

            if (_showFps) {
                SDL_SetRenderTarget(SDL.getRenderer(), t);
                showFps();
            }

            SDL.renderTextureToWindow(t);
            _lastFrame = SDL_GetTicks();
            ++_framesRun;
        }
        if (tick) {
            _lastTick = SDL_GetTicks();
        }
        if (_switchState) {
            delete _currentState;
            _currentState = _newState;
            _switchState = false;
        }

    }
}

void StateManager::switchToState(State* state) {
    _newState = state;
    _switchState = true;
}

float StateManager::getAvgFps() const {				
    return _avgFps;
}

State* StateManager::getCurrentState() const {
    return _currentState;
}

void StateManager::returnToTitle() {
    if (!dynamic_cast<FadeTransition*>(_currentState)) {
        switchToState(new FadeTransition([]() {
            return new TitleScreen;
        }));
    }
}

const KeyConfig& StateManager::getP1keys() const {		// 첫 번째 사용자의 키

    return _p1keys;
}

void StateManager::setKeys(KeyConfig keys, int player) {	// 두 사용자의 키 설정
    switch (player) {
        case 1:
            _p1keys = keys;
            break;
        case 2:
            _p2keys = keys;

        default:
            break;
    }
}

const KeyConfig& StateManager::getP2keys() const {		// 두 번째 사용자의 키

    return _p2keys;
}

void StateManager::showFps() {		// fps 나타내기
    std::ostringstream ss;
    _avgFps = _framesRun / ((SDL_GetTicks() - _startTime) / 1000.f);
    if (_avgFps > 2000000) {

        _avgFps = 0;
    }
    ss << std::setprecision(5) << _avgFps;
    SDL.renderText(ss.str(),{0, 0, 0}, SDL._fontPs, 0, 0);
}

void StateManager::goBack() {

    _currentState->goBack();
}

void StateManager::quit() {		// 멈춤
    _running = false;
}