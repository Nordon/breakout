/*
 * CGame.cpp
 *
 *  Created on: 07.05.2010
 *      Author: TSS
 */

#include "Game.h"
#include "Log.h"
#include "SettingsFile.h"
#include "Settings.h"

CGame::CGame(sf::VideoMode vMode, const std::string& title): sf::RenderWindow(vMode,title),gameBoard(*this)
{
	CLog::getInst().writeInfo("Objekte wurden initialisiert.");
	CSettingsFile config("config.ini");
	if(!config.returnWert("Steuerung").compare("Tastatur"))
	{
		CSettings::steuerungTastatur=true;
	}
}

CGame::~CGame(void)
{
}

int CGame::exec()//main Loop
{
    // Start game loop
    while (this->IsOpened())
    {
        // Process events
        sf::Event Event;
        while (this->GetEvent(Event))
        {
            // Close window : exit
            if (Event.Type == sf::Event::Closed)
			{
				CLog::getInst().writeInfo("Spiel wird beendet.");
                this->Close();
			}

			//Ball bewegt sich nach Druck auf Leertaste
			if(Event.Type == sf::Event::KeyPressed && Event.Key.Code == sf::Key::Space && !gameBoard.isRunning() && !gameBoard.isPaused())
			{
				//std::cout << "Spiel gestartet." << std::endl;
				gameBoard.start();
			}

			//Ball bewegt sich nach Druck auf Linke Maustaste
			if(Event.Type == sf::Event::MouseButtonPressed && Event.MouseButton.Button == sf::Mouse::Left && !gameBoard.isRunning() && !gameBoard.isPaused())
			{
				//std::cout << "Spiel gestartet." << std::endl;
				gameBoard.start();
			}

			//Taste 'R' sorgt für Neustart
			if(Event.Type == sf::Event::KeyPressed && Event.Key.Code == sf::Key::R)
			{
				gameBoard.restart();
			}

			//'P' pausiert das Spiel
			if(Event.Type == sf::Event::KeyPressed && Event.Key.Code == sf::Key::P)
			{
				if(gameBoard.isPaused())
					gameBoard.resume();
				else
					gameBoard.pause();
			}

			//Spiel wird pausiert, wenn das Fenster den Fokus verliert (nicht, wenn Benutzer das Spiel vorher manuell pausiert hat)
			if(Event.Type == sf::Event::LostFocus)
			{
				if(!gameBoard.isPaused())
					gameBoard.systemPause();
			}

			//Spiel wird nach GameOver im aktuellen Level aber ohne Punkte forgesetzt
			if(Event.Type == sf::Event::KeyPressed && Event.Key.Code == sf::Key::C && gameBoard.isGameOver())
			{
				gameBoard.cont();
			}

			//Spiel wird fortgesetzt, wenn Fenster den Fokus wiederbekommt (nicht, wenn Benutzer das Spiel vorher manuell pausiert hat)
			if(Event.Type == sf::Event::GainedFocus)
			{
				if(!gameBoard.isPaused())
					gameBoard.resume();
			}

			/*if(Event.Type == sf::Event::MouseMoved)
				std::cout << this->GetInput().GetMouseX() << std::endl;*/
        }

		gameBoard.move();
		gameBoard.render();

        // Display window contents on screen
        this->Display();
		sf::Sleep(0.001f);
    }

    return EXIT_SUCCESS;
}
