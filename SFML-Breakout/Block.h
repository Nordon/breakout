/*
 * Block.h
 *
 *  Created on: 07.05.2010
 *      Author: TSS
 */

#ifndef BLOCK_H_
#define BLOCK_H_

#include <SFML/Graphics.hpp>

class CBlock :
	public sf::Shape
{
public:
	CBlock(float posx=0,float posy=0,int leben=1);
	~CBlock(void);
	bool isVisible();
	void hit();
	sf::Rect<float> GetBlockRect();
	static bool alleZerstoert();
	
private:
	sf::Rect<float> m_rect;
	bool m_visible;
	int m_leben;
	static int counter;//Anzahl der noch sichtbaren Bloecke
};

#endif /* BLOCK_H_ */
