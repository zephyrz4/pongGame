/*
 * Singleplayer.cpp
 *
 *  Created on: Nov 1, 2012
 *      Author: Cam
 */

#include "Singleplayer.h"
#include "Ball.h"
#include "Paddle.h"
#include "HumanPaddle.h"
#include "ComputerPaddle.h"
#include "util.h"
#include "Timer.h"
#include <ctime>
#include <stdlib.h>

Singleplayer::Singleplayer(SDL_Surface *screen) :
		Game(screen) {

	setState(INIT);
}

Singleplayer::~Singleplayer() {

	// Paddles
	for (vector<Paddle*>::iterator it = paddles.begin(); it != paddles.end();
			++it) {

		delete *it;

	}

	// Balls
	for (vector<Ball*>::iterator it = balls.begin(); it != balls.end(); ++it) {

		delete *it;

	}

}

bool Singleplayer::init() {

	// Setup background image
	setBackground(load_image("images/background.png"));
	apply_surface(0, 0, getBackground(), getScreen());

	// Setup score
	score[0] = 0;
	score[1] = 0;

	// Set sound for collision
	setCollisionSound("sounds/pong.wav");

	// Setup escape menu
	getGamemenu()->addButton((SCREEN_WIDTH / 2) - 50, (SCREEN_HEIGHT / 2) - 60,
				100, 40, "images/Resume.png", PLAYING);
	getGamemenu()->addButton((SCREEN_WIDTH / 2) - 50, (SCREEN_HEIGHT / 2) - 20,
			100, 40, "images/Quit.png", QUIT);

	// Setup text image
	scoreMessage = new Text((SCREEN_WIDTH / 2) - 100, 0, "Score : 0 - 0",
			"fonts/Allcaps.ttf", 28, 255, 255, 255);

	// Add the ball
	balls.push_back(
			new Ball(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 200.0, 10,
					"images/ball.png"));
	// Add human paddle
	paddles.push_back(
			new HumanPaddle(5, SCREEN_HEIGHT / 2, 0, 0, 10, 10,
					"images/paddle.png", SDLK_UP, SDLK_DOWN));
	// Add computer paddle
	paddles.push_back(
			new ComputerPaddle(615, SCREEN_HEIGHT / 2, 0, 0, 10, 10,
					"images/paddle.png", 0, &balls));

	// Finished
	setState(PLAYING);

	return true;
}

void Singleplayer::run() {

	SDL_Event event;

	LogWrite(toString(), "game.log");

	// FPS test
	int frame = 0;
	Timer fps;
	Timer update;
	Timer delta;
	Timer menuDelay;
	if (getState() == INIT) {

		if (!init()) {

			LogWrite("failed to init game", "game.log");

			return;

		}

	}

	update.start();
	fps.start();
	delta.start();

	while (getState() != QUIT) {

		if (getState() == MENU)
			displayMenu();

		//While there's an event to handle
		while (getState() == PLAYING) {

			SDL_PollEvent(&event);

			//If the user has Xed out the window
			if (event.type == SDL_QUIT) {
				//Quit the program
				setState(QUIT);
			}

			// Escape menu

			Uint8 *keystate = SDL_GetKeyState(NULL);

			if (keystate[SDLK_ESCAPE]) {
				setState(MENU);
				menuDelay.start();

				while (menuDelay.get_ticks() < 100)
					;

				menuDelay.stop();
			}
			switch (event.user.code) {

			case QUIT:
				setState(QUIT);
				break;
			default:
				break;

			}

			this->update(&event, delta.get_ticks());
			delta.start();
			draw();

			//Update the screen
			if (SDL_Flip(getScreen()) == -1) {
				return;
			}
			SDL_Delay(0);
			frame++;

			//If a second has passed since the caption was last updated
			if (update.get_ticks() > 1000) {
				//The frame rate as a string
				std::stringstream caption;

				//Calculate the frames per second and create the string
				caption << "Pong - FPS: " << frame / (fps.get_ticks() / 1000.f);
				//Reset the caption
				SDL_WM_SetCaption(caption.str().c_str(), NULL);

				//Restart the update timer
				update.start();
			}

		}
	}

}

void Singleplayer::draw() {

	apply_surface(0, 0, getBackground(), getScreen());

	// Draw score

	scoreMessage->draw(getScreen());

	// Draw paddles
	for (vector<Paddle*>::iterator it = paddles.begin(); it != paddles.end();
			++it) {

		(*it)->draw(getScreen());

	}

	// Draw balls
	for (vector<Ball*>::iterator it = balls.begin(); it != balls.end(); ++it) {

		(*it)->draw(getScreen());

	}

}

void Singleplayer::update(SDL_Event *event, Uint32 ticks) {

	// Update paddles
	for (vector<Paddle*>::iterator it = paddles.begin(); it != paddles.end();
			++it) {

		(*it)->update(event, ticks);

	}
	// Update balls
	for (vector<Ball*>::iterator it = balls.begin(); it != balls.end(); ++it) {

		(*it)->update(event, ticks);

	}

	// Check if balls collide with any paddle
	for (vector<Ball*>::iterator it = balls.begin(); it != balls.end(); ++it) {

		for (vector<Paddle*>::iterator it2 = paddles.begin();
				it2 != paddles.end(); ++it2) {

			if ((*it)->checkCollision(*it2)) {

				collision->playSound(0);
				handleCollision(*it2, *it);

			}

		}

	}

	checkScore();

}

void Singleplayer::handleCollision(Paddle* paddle, Ball* ball) {

	// Paddle info
	float padX = paddle->getX();
	float padY = paddle->getY();
	float padVX = paddle->getVelx();
	float padVY = paddle->getVely();
	int padW = paddle->getImage()->w;
	int padH = paddle->getImage()->h;
	// Ball info
	float ballX = ball->getX();
	float ballY = ball->getY();
	float ballVX = ball->getVelx();
	float ballVY = ball->getVely();
	int ballW = ball->getImage()->w;
	int ballH = ball->getImage()->h;

	// Is ball left or right of paddle?
	if (ballX > padX) {

		// Move ball out of the paddle
		if (ballX < padX + padW)
			ballX = padW + padX;

	}
	// Ball is to the left
	else {

		// Move ball out of the paddle
		if (ballX + ballW > padX)
			ballX = padX - ballW;

	}

	// Handle new velocities

	// Ball is moving left-right only
	if (ballVY == 0) {

		if (ballVX > 0)
			ballVX = -(ballVX + 10);
		else
			ballVX = -(ballVX - 10);

	}
	// Ball is moving up-down only
	else if (ballVX == 0) {

		if (ballVX > 0)
			ballVY = -(ballVY + 10);
		else
			ballVY = -(ballVY - 10);

		// Ball is moving in both direction
	} else {

		if (ballVX > 0)
			ballVX = -(ballVX + 10);
		else
			ballVX = -(ballVX - 10);

		/*		if (ballVY > 0) {

		 if (padVY > 0)
		 ballVY = ballVY + padVY * .1;
		 else
		 ballVY = ballVY + -(padVY * .1);

		 } else {

		 if (padVY > 0)
		 ballVY = ballVY - padVY * .1;
		 else
		 ballVY = ballVY + padVY * .1;

		 }*/

		ballVY = ballVY + padVY * .1;

	}

	ball->setX(ballX);
	ball->setY(ballY);
	ball->setVelx(ballVX);
	ball->setVely(ballVY);

}

void Singleplayer::checkScore() {

	for (vector<Ball*>::iterator it = balls.begin(); it != balls.end(); ++it) {

		if ((*it)->getX() <= 0 || (*it)->getX() >= SCREEN_WIDTH - 40) {

			if ((*it)->getX() <= 0)
				score[1]++;
			else
				score[0]++;

			std::stringstream newMsg;

			newMsg << "Score: " << score[0] << "-" << score[1];

			scoreMessage->setText(newMsg.str());

			// Score message
			scoreMessage->update(NULL, 0);

			int xVel, yVel;

			srand(time(NULL));
			xVel = rand() % 200 + 120;
			yVel = rand() % 50 + 1;

			if (score[1] > score[0])
				xVel = -xVel;

			delete *it;
			balls.erase(it);
			balls.push_back(
					new Ball(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, xVel, yVel,
							"images/ball.png"));

		}

	}

}

void Singleplayer::addBall(Ball *newball) {

	balls.push_back(newball);

}

void Singleplayer::addPaddle(Paddle *newpaddle) {

	paddles.push_back(newpaddle);
}

void Singleplayer::setScore(int h, int c) {

	score[0] = h;
	score[1] = c;

}

void Singleplayer::setScoreMessage(Text *msg) {

	scoreMessage = msg;

}

void Singleplayer::setCollisionSound(char *filename) {

	collision = new Sound(filename);

}

void Singleplayer::displayMenu() {

	SDL_Event event;
	Timer menuDelay;

	LogWrite(toString(), "game.log");
	LogWrite(getGamemenu()->toString(), "game.log");

	while (getState() == MENU) {

		SDL_PollEvent(&event);

		//If the user has Xed out the window
		if (event.type == SDL_QUIT) {
			//Quit the program
			setState(QUIT);
		}

		Uint8 *keystate = SDL_GetKeyState(NULL);
		if (keystate[SDLK_ESCAPE]) {
			setState(PLAYING);

			menuDelay.start();

			while (menuDelay.get_ticks() < 100)
				;

			menuDelay.stop();
		}
		switch (event.user.code) {

		case PLAYING:
			setState(PLAYING);
			break;

		case QUIT:
			setState(QUIT);
			break;
		default:
			break;

		}

		getGamemenu()->update(&event);
		getGamemenu()->draw(getScreen());

		//Update the screen
		if (SDL_Flip(getScreen()) == -1) {
			return;
		}

	}
	SDL_Delay(1);

}
