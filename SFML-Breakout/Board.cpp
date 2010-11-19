/*
 * Board.cpp
 *
 *  Created on: 07.05.2010
 *      Author: TSS
 */

#include "Board.h"
#include "Typedef.h"

CBoard::CBoard(float posx,float posy) :  m_breite(Typedef::boardBreite),m_hoehe(Typedef::boardHoehe)
{
	this->AddPoint(0,0,sf::Color(0,0,0));
	this->AddPoint(m_breite,0,sf::Color(0,0,0));
	this->AddPoint(m_breite,m_hoehe,sf::Color(0,0,0));
	this->AddPoint(0,m_hoehe,sf::Color(0,0,0));

	this->SetPosition(posx,posy);

	this->EnableOutline(false);

	m_freeze=false;
}

CBoard::~CBoard(void)
{
}

sf::Rect<float> CBoard::GetBoardRect()
{
	return sf::Rect<float>(this->GetPosition().x,this->GetPosition().y,this->GetPosition().x+m_breite,this->GetPosition().y+m_hoehe);
}


void CBoard::checkBoardStillInRange()
{
	if(this->GetBoardRect().Left<10) this->SetX(10);
	if(this->GetBoardRect().Right>800-10) this->SetX(800-10-m_breite);
}

bool CBoard::isFreezed()
{
	return m_freeze;
}

void CBoard::setFreeze(bool status)
{
	m_freeze=status;
}
