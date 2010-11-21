/*
 * Block.cpp
 *
 *  Created on: 07.05.2010
 *      Author: TSS
 */

#include "Block.h"
#include "Typedef.h"
#include "Log.h"

CBlock::CBlock(float posx, float posy, int leben) : m_leben(leben)
{
	sf::Color color;
	//setzt Farbe je nach Art des Blockes, die Arten unterscheiden sich durch Anzahl der benoetigten Treffer
	switch(m_leben)
	{
	case 1:	color=sf::Color::Black;
			break;
	case 2:	color=sf::Color::Red;
			break;
	default:color=sf::Color::Black;
			break;
	}
	this->AddPoint(0,0,color);
	this->AddPoint(40,0,color);
	this->AddPoint(40,10,color);
	this->AddPoint(0,10,color);

	this->EnableOutline(false);

	this->SetPosition(posx,posy);

	m_rect=sf::Rect<float>(posx,posy,posx+40,posy+10);

	m_visible=true;

	counter++;
}

int CBlock::counter = 0;

bool CBlock::alleZerstoert()
{
	return !counter;
}

CBlock::~CBlock(void)
{
	if(m_visible)//wenn Block noch sichtbar wenn er zerstoert wird (z.B bei Game Over)
	{
		counter--;
	}
}

bool CBlock::isVisible()
{
	return m_visible;
}

void CBlock::hit()
{
	//zieht dem Block ein Leben ab
	m_leben--;
	if(m_leben==0)//Block zerstoert
	{
		counter--;
		m_visible=false;
	}
	else//Block hat jetzt eine andere Farbe
	{
		switch(m_leben)
		{
		case 1:	this->SetColor(sf::Color::Black);
				break;
		case 2:	this->SetColor(sf::Color::Red);
				break;
		default:this->SetColor(sf::Color::Black);
				break;
		}
	}
}

sf::Rect<float> CBlock::GetBlockRect()
{
	//z.B. fuer die Kollisionserkennung
	return m_rect;
}
