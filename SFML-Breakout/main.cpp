#include <SFML/Graphics.hpp>
#include "Game.h"
#include "Log.h"

int main()
{
    // Create the main rendering window
    CGame App(sf::VideoMode(800, 600, 32), "SFML-BreakOut 0.9.2");
	//App.SetFramerateLimit(0);
	App.UseVerticalSync(true);
	return App.exec();
}
