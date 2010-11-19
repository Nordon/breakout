/*
 * Ball.cpp
 *
 *  Created on: 07.05.2010
 *      Author: TSS
 */

#include "Ball.h"
#include "Typedef.h"
#include "Block.h"
#include "Board.h"
#include "Log.h"

#define PI 3.14159265

CBall::CBall(float mittelpunktx,float mittelpunkty,float radius_) : sf::Shape(sf::Shape::Circle(0,0,radius_,sf::Color(0,0,0))), radius(radius_)
{
	this->SetPosition(mittelpunktx,mittelpunkty);
	this->createRV();
}

CBall::~CBall(void)
{
}

sf::Rect<float> CBall::GetBallRect()
{
	return sf::Rect<float>(this->GetPosition().x-10,this->GetPosition().y-10,this->GetPosition().x+10,this->GetPosition().y+10);
}

float CBall::GetRadius()
{
	return radius;
}

void CBall::createRV()
{
	sf::Vector2f vektor(0,0);
	float x_tmp = sf::Randomizer::Random(-1.f, 1.f);
	float y_tmp=-sqrt(1.f-pow(x_tmp,2.f));//muss beim Startvektor negativ sein, denn der Ball klebt auf dem Board und soll beim abfeuern nach oben fliegen
	this->rv.x=x_tmp;
	this->rv.y=y_tmp;
}

void CBall::correctRV()
{
	if(this->rv.y>=0 && this->rv.y<0.1)
	{
		this->rv.y=0.1f;
		if(this->rv.x>=0)
			this->rv.x=0.9949874371f;
		else
			this->rv.x=-0.9949874371f;
	}
	else if(this->rv.y<0 && this->rv.y>-0.1)
	{
		this->rv.y=-0.1f;
		if(this->rv.x>=0)
			this->rv.x=0.9949874371f;
		else
			this->rv.x=-0.9949874371f;
	}
}