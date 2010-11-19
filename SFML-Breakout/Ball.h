/*
 * Ball.h
 *
 *  Created on: 07.05.2010
 *      Author: TSS
 */

#ifndef BALL_H_
#define BALL_H_

#include <SFML/Graphics.hpp>

class CBoard;
class CBlock;

class CBall :
	public sf::Shape
{
public:
	CBall(float mittelpunktx,float mittelpunkty,float radius_);
	sf::Vector2f rv;
	sf::Rect<float> GetBallRect();
	float GetRadius();
	void createRV();
	void correctRV();//wenn abs(y)<0.1 dann bewegt sich der Ball nicht genug nach unten bzw. oben
	~CBall(void);
private:
	float radius;
};

#endif /* BALL_H_ */
