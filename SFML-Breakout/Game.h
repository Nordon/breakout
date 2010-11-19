/*
 * CGame.h
 *
 *  Created on: 07.05.2010
 *      Author: TSS
 */

#ifndef GAME_H_
#define GAME_H_

#include "GameBoard.h"

class CGame :
	public sf::RenderWindow
{
public:
	CGame(sf::VideoMode vMode,const std::string& title);
	~CGame(void);
	int exec();
private:
	CGameBoard gameBoard;
};


#endif /* GAME_H_ */
