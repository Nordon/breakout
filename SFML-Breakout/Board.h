/*
 * Board.h
 *
 *  Created on: 07.05.2010
 *      Author: TSS
 */

#ifndef BOARD_H_
#define BOARD_H_

#include <SFML/Graphics.hpp>

class CBoard :
	public sf::Shape
{
public:
	CBoard(float posx,float posy);//Koordinaten der Ecke links oben
	~CBoard(void);
	sf::Rect<float> GetBoardRect();
	void checkBoardStillInRange();
	bool isFreezed();
	void setFreeze(bool status);
private:
	const float m_breite;
	const float m_hoehe;
	bool m_freeze;
};

#endif /* BOARD_H_ */
