/*
 * Singleplayer.h
 *
 *  Created on: Nov 1, 2012
 *      Author: Cam
 */

#ifndef SINGLEPLAYER_H_
#define SINGLEPLAYER_H_
#include "game.h"
#include "Ball.h"
#include "Paddle.h"
#include "ComputerPaddle.h"
#include <vector>
#include "Text.h"
#include "Sound.h"

enum singleStates {

	PLAYING,
	PAUSE

};


class Singleplayer : public Game {
public:

	Singleplayer(SDL_Surface *screen);
	virtual ~Singleplayer();
	virtual bool init();  // This will setup everything before the game actually loads
	virtual void run(); // This will initially open the menu and go from there
	virtual void draw(); // This will redraw the window
	virtual void update(SDL_Event* event, Uint32 ticks); // This will update eveyrthing in the game
	virtual void handleCollision(Paddle *paddle, Ball *ball); // This will update the ball if it collides with a paddle
	virtual void checkScore();
	virtual void addBall( Ball *newball ) ;

	virtual void addPaddle( Paddle *newpaddle ) ;

	virtual void setScore( int h, int c ) ;

	virtual void setScoreMessage( Text *msg ) ;

	virtual void setCollisionSound( char *filename ) ;

	virtual void displayMenu();
private:

	std::vector<Ball*> balls ;
	std::vector<Paddle*> paddles ;
	// 0 - Human : 1 - Computer
	unsigned int score[2];
	Text *scoreMessage ;
	
	Sound *collision ;
};

#endif /* SINGLEPLAYER_H_ */
