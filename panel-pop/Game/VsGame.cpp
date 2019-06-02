/*
 * Game.cpp
 *
 *  Created on: Aug 16, 2015
 *      Author: axel
 */

#include "VsGame.h"
#include <SDL2/SDL_timer.h>

#include "../Config/ConfigHandler.h"
#include "BoardEventHandler.h"
#include "GameEventHandler.h"
#include "GarbageBlock.h"

#include "../Rendering/BoardRenderer.h"
#include <iostream>
#include <math.h>
#include <stddef.h>
#include <SDL2/SDL_blendmode.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <cstdint>

#include "Block.h"
#include "GarbageBlock.h"
#include "../SDLContext.h"



VsGame::VsGame(GameEventHandler* geh) :
Game(geh),
_board0(this),
_board1(this),
_p1MatchPoints(0), // new
_p2MatchPoints(0), // new
_p1Points(0),
_p2Points(0) { // new
}

void VsGame::handleEnd() {
    if (_board0.getState() == Board::GAME_OVER || _board1.getState() == Board::GAME_OVER) {
        if (_board0.getState() == Board::RUNNING) {
            _board0.win();
            ++_p1MatchPoints;
	    _board0._bomb++; // new
	    _board0._cross++; // new
	    _board0._same_color++; // new
	    ConfigHandler::getInstance().setNumberOfP1Bomb(_board0._bomb); // new
	    ConfigHandler::getInstance().setNumberOfP1Cross(_board0._cross); // new
	    ConfigHandler::getInstance().setNumberOfP1SameColor(_board0._same_color); // new
	    ConfigHandler::getInstance().saveConfig(); // new

        } else if (_board1.getState() == Board::RUNNING) {
            _board1.win();
            ++_p2MatchPoints;
	    _board1._bomb++; // new
	    _board1._cross++; // new
	    _board1._same_color++; // new
	    ConfigHandler::getInstance().setNumberOfP2Bomb(_board1._bomb); // new
	    ConfigHandler::getInstance().setNumberOfP2Cross(_board1._cross); // new
	    ConfigHandler::getInstance().setNumberOfP2SameColor(_board1._same_color); // new
	    ConfigHandler::getInstance().saveConfig(); // new
        }

        if (_p1MatchPoints >= MATCH_POINTS) {
            _p1MatchPoints = 0;
            _p2MatchPoints = 0;
            ++_p1Points;
        }
        if (_p2MatchPoints >= MATCH_POINTS) {
            _p1MatchPoints = 0;
            _p2MatchPoints = 0;
            ++_p2Points;
        }

        _pausedTime = SDL_GetTicks() - _startTime;
        if (_p1Points == 3) { // new
	    _p1Points = 0;
	    _p2Points = 0;
	   
	    _state = State::P1WON;
	/* ----------- reset number of item ----------- */
	    ConfigHandler::getInstance().setNumberOfP1Bomb(0); // new
    	    ConfigHandler::getInstance().setNumberOfP1Cross(0); // new
    	    ConfigHandler::getInstance().setNumberOfP1SameColor(0); // new
    	    ConfigHandler::getInstance().setNumberOfP2Bomb(0); // new
    	    ConfigHandler::getInstance().setNumberOfP2Cross(0); // new
    	    ConfigHandler::getInstance().setNumberOfP2SameColor(0); // new

    	    ConfigHandler::getInstance().saveConfig(); // new  
	/* ----------- reset number of item ----------- */
	}
        else if (_p2Points == 3) { // new
	    _p1Points = 0;
	    _p2Points = 0;
	   
	    _state = State::P2WON;
	/* ----------- reset number of item ----------- */
	    ConfigHandler::getInstance().setNumberOfP1Bomb(0); // new
    	    ConfigHandler::getInstance().setNumberOfP1Cross(0); // new
    	    ConfigHandler::getInstance().setNumberOfP1SameColor(0); // new
    	    ConfigHandler::getInstance().setNumberOfP2Bomb(0); // new
    	    ConfigHandler::getInstance().setNumberOfP2Cross(0); // new
    	    ConfigHandler::getInstance().setNumberOfP2SameColor(0); // new
	/* ----------- reset number of item ----------- */
    	    ConfigHandler::getInstance().saveConfig(); // new
	}
	else _state = State::ENDED;
    }
}

void VsGame::tick() {
_board0._stackRaiseTicks =10;		///////////////////
_board1._stackRaiseTicks=10;
   // _board0._mode = Board::VS;			////////////////////////////////////
    //_board1._mode = Board::VS;			////////////////////////////////////

	    _board0._bomb = ConfigHandler::getInstance().getNumberOfP1Bomb(); // new
	    _board0._cross = ConfigHandler::getInstance().getNumberOfP1Cross(); // new
	    _board0._same_color = ConfigHandler::getInstance().getNumberOfP1SameColor(); // new
	    _board1._bomb = ConfigHandler::getInstance().getNumberOfP2Bomb(); // new
	    _board1._cross = ConfigHandler::getInstance().getNumberOfP2Cross(); // new
	    _board1._same_color = ConfigHandler::getInstance().getNumberOfP2SameColor(); // new

    if (_board0.getState() == Board::COUNTDOWN) {
        _eventHandler->countdown(getTime());
    }

    if (_state == State::RUNNING) {
        ++_ticksRun;
        handleGarbageSpawning(_board0, _board1);
        handleGarbageSpawning(_board1, _board0);
        _board0.tick();
        _board1.tick();

        if (!_panic && (_board0.isPanic() || _board1.isPanic())) {
            _panic = true;
            _eventHandler->panicBegin();
        }
        if (_panic && ((!_board0.isPanic() && !_board1.isPanic()))) {
            _panic = false;
            _eventHandler->panicEnd();
        }

        handleEnd();
        _eventHandler->tickEnd();
    }
}

int VsGame::getP1MatchPoints() const {
    return _p1MatchPoints;
}

int VsGame::getP1Points() const {
    return _p1Points;
}

int VsGame::getP2MatchPoints() const {
    return _p2MatchPoints;
}

int VsGame::getP2Points() const {
    return _p2Points;
}

Board& VsGame::getBoard(int id) {
    switch (id) {
        case 0:
            return _board0;
            break;
        default:
            return _board1;
            break;
    }
}

void VsGame::handleGarbageSpawning(Board& b1, Board& b2) {

    int combo = b1.getTickMatched();
    int chain = b1.getLastChain() - 1;
    if (chain > 12) {
        chain = 12;
    }

    if (b1.isTickChainEnd()) {
        b2.queueGarbage(true, chain, GarbageBlockType::NORMAL);
    }

    if (combo >= 4 && combo <= 7) {
        b2.queueGarbage(false, combo - 1, GarbageBlockType::NORMAL); // b2.queueGarbage(false, combo - 1, GarbageBlockType::NORMAL);
    } else if (combo == 8) {
        b2.queueGarbage(false, 3, GarbageBlockType::NORMAL);
        b2.queueGarbage(false, 4, GarbageBlockType::NORMAL);
    } else if (combo >= 9 && combo <= 10) {
        b2.queueGarbage(false, combo - 5, GarbageBlockType::NORMAL);
        b2.queueGarbage(false, combo - 5, GarbageBlockType::NORMAL);
    } else if (combo == 11) {
        b2.queueGarbage(false, 5, GarbageBlockType::NORMAL);
        b2.queueGarbage(false, 6, GarbageBlockType::NORMAL);
    } else if (combo >= 12) {
        for (int i = 0; i < 5 - combo; ++i)
            b2.queueGarbage(true, 1, GarbageBlockType::NORMAL); // b2.queueGarbage(true, 1, GarbageBlockType::NORMAL);
    }
}

void VsGame::reset() {
    _state = State::RUNNING;
    _startTime = SDL_GetTicks();
    _board0 = _board0.reset();
    _board1 = _board1.reset();
    _panic = false;
    _eventHandler->gameReset();
}

VsGame::~VsGame() {
    delete _board0.getEventHandler();
    delete _board1.getEventHandler();
}
