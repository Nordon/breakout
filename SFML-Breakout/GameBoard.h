/*
 * GameBoard.h
 *
 *  Created on: 07.05.2010
 *      Author: TSS
 */

#ifndef GAMEBOARD_H_
#define GAMEBOARD_H_

#include <SFML/Graphics.hpp>
#include <vector>
#include "Block.h"
#include "Board.h"
#include "Ball.h"

class CGame;

class CGameBoard
{
public:
	CGameBoard(CGame& App_);
	~CGameBoard(void);
	void restart(int level=1);//Spiel neu starten, Parameter: das zu startende Level kann uebergeben werden, z.B. nach GameOver, um das aktuelle Level neu zu laden aber Punkte zu resetten
	void cont();//ruft restart mit aktuellen Level auf (nach GameOver)
	void render();
	void move();
	void pause();//pausieren
	void resume();//Ende der Pause
	void systemPause();//pausieren bei Fensterwechsel
	void start();
	bool isRunning();
	bool isPaused();
	bool isGameOver();
	enum GameBoardState {Starting, Running, Paused, SystemPaused, LevelComplete, LostLife, Won, GameOver};
private:
	sf::Shape Rand1,Rand2,Rand3;
	std::vector<CBlock> Block;
	float m_ElapsedTime;
	CBoard Board;
	CBall Ball;
	CGame& App;
	sf::String m_sLeben;
	sf::String m_sPunkte;
	sf::String m_sLevel;
	float sleep;
	void displayPause();
	void displaySieg();
	void displayLevelGeschafft();
	void displayGameOver();
	std::vector<std::vector<char> > spielfeld;//enthaelt Aufbau der Level, 2D Array
	void initLevel();
	void loadLevel(unsigned int level);
	void updateLeben();
	void updatePunkte();
	void updateLevel();
	int anzahl_bloecke;
	GameBoardState gameState,oldState;
	unsigned int m_leben;
	unsigned int m_aktuellesLevel;
	unsigned int m_punkte;
	void reset();
	int checkKollisionBallMitBlock(CBall& Ball, CBlock& Block);
	void switchKollisionBallMitBlock(CBall& Ball, CBlock& Block, int kollision);
	void setNewBallRvAfterBlockCornerCollision(CBall& Ball,float x_diff,float y_diff);
	void checkKollisionBallMitRand(CBall& Ball);
	void checkKollisionBallMitBoard(CBall& Ball,CBoard &board);
	bool ballIntersectsBoard(CBall& Ball, CBoard& board);
	void setNewBallRvAfterBoardCollision(CBall& Ball,CBoard& board,float x);
};

#endif /* GAMEBOARD_H_ */
