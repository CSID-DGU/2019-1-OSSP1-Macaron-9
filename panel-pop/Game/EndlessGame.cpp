/*
 * EndlessGame.cpp
 *
 *  Created on: 24.2.2016
 *      Author: axelw
 */

#include "EndlessGame.h"

#include <SDL2/SDL_timer.h>

#include "../Config/ConfigHandler.h"
#include "BoardEventHandler.h"
#include "GameEventHandler.h"

class BoardEventHandler;

EndlessGame::EndlessGame(GameEventHandler* geh) :
Game(geh),
_board(this),
_finalLevel(ConfigHandler::getInstance().getEndlessFinalLevel()), // new
_highScore(ConfigHandler::getInstance().getEndlessHighScore()) {

}

EndlessGame::~EndlessGame() {
    delete _board.getEventHandler();
}

void EndlessGame::reset() {
   // _board._mode = Board::Endless;		////////////////////////////////

    _state = State::RUNNING;
    _startTime = SDL_GetTicks();
    _board = _board.reset();
    _panic = false;
    _eventHandler->gameReset();
}

void EndlessGame::tick() {

 //   _board._mode = Board::Endless;		////////////////////////////////

    if (_board.getState() == Board::COUNTDOWN) {
        _eventHandler->countdown(getTime());
    }

    if (_state == State::RUNNING) {
        ++_ticksRun;
        _board.tick();
        if (!_panic && _board.isPanic()) {
            _panic = true;
            _eventHandler->panicBegin();
        }
        if (_panic && !_board.isPanic()) {
            _panic = false;
            _eventHandler->panicEnd();
        }

        if (_board.getScore() > _highScore) {
            _highScore = _board.getScore();
        }

	if ((11 - _board.getStackRaiseTicks()) > _finalLevel) {	// new
	    _finalLevel = (11 - _board.getStackRaiseTicks()); 
	}

        handleEnd();
        _eventHandler->tickEnd();
    }
}

Board& EndlessGame::getBoard(int) {
    return _board;
}

int EndlessGame::getHighScore() const {
    return _highScore;
}

int EndlessGame::getFinalLevel() const { // new
    return _finalLevel;
}

void EndlessGame::handleEnd() {
    if (_board.getState() == Board::GAME_OVER) {

	ConfigHandler::getInstance().setEndlessFinalLevel(_finalLevel); // new

        ConfigHandler::getInstance().setEndlessHighScore(_highScore);
        ConfigHandler::getInstance().saveConfig();

        _pausedTime = SDL_GetTicks() - _startTime;
        _state = State::ENDED;
    }
}
