/*
 * GameBoard.cpp
 *
 *  Created on: 07.05.2010
 *      Author: TSS
 */

#include "GameBoard.h"
#include "Game.h"
#include "Typedef.h"
#include "Log.h"
#include "Settings.h"
#include <fstream>
#include <sstream>

#define PI 3.14159265

CGameBoard::CGameBoard(CGame& App_) : 
	m_ElapsedTime(0.f),Board(800/2.f-Typedef::boardBreite,600-Typedef::boardHoehe), 
	Ball(Board.GetPosition().x+Typedef::boardBreite/2,600.f-Typedef::boardHoehe-10.f,10.f),//setzt Ball mittig auf Board
	App(App_)
{
	//Variablen setzen
	m_leben=3;
	m_aktuellesLevel=1;
	m_punkte=0;
	//Raender erzeugen
	Rand1=sf::Shape::Rectangle(0,0,800,40,sf::Color(0,0,0));//oben
	Rand2=sf::Shape::Rectangle(0,0,10,600,sf::Color(0,0,0));//links
	Rand3=sf::Shape::Rectangle(800-10,0,800,600,sf::Color(0,0,0));//rechts
	//Bloecke erzeugen
	initLevel();
	loadLevel(1);
	//Board kann bewegt werden
	sleep=0;
	//Ball ruht
	oldState=Starting;
	gameState=Starting;//Ball ruht in der Mitte, mit Leertaste gehts los
	//Anzeigen initialisieren
	m_sLeben=sf::String("",sf::Font::GetDefaultFont(),15);
	m_sPunkte=sf::String("",sf::Font::GetDefaultFont(),15);
	m_sLevel=sf::String("",sf::Font::GetDefaultFont(),15);
	//Anzeigen aktualisieren
	updateLeben();	
	updatePunkte();
	updateLevel();
}

CGameBoard::~CGameBoard(void)
{
}

void CGameBoard::restart(int level)
{
	//Variablen zuruecksetzen
	m_leben=3;
	m_aktuellesLevel=level;
	m_punkte=0;
	//Bloecke erzeugen
	loadLevel(level);
	//Board zuruecksetzen
	Board.SetPosition(800/2.f-30,600-10);
	Board.setFreeze(false);
	//Ball zuruecksetzen
	Ball.SetPosition(Board.GetPosition().x+Typedef::boardBreite/2,600.f-Typedef::boardHoehe-10.f);
	Ball.createRV();
	//Ball ruht
	gameState=Starting;
	sleep=0;
	//Anzeigen zuruecksetzen
	updateLeben();
	updatePunkte();
	updateLevel();
	//Log Eintrag
	CLog::getInst().writeInfo("Spiel wurde zurueckgesetzt.");
}

//Zeichnet das Spiel
void CGameBoard::render()
{
	if(gameState==Paused || gameState==SystemPaused)//bei Pause wird Hintergrund dunkler
	{
		App.Clear(sf::Color(100,100,100));
	}
	else
	{
		//Hellgraue Hintergrundfarbe
		App.Clear(sf::Color(200,200,200));
	}

	//Raender zeichnen
	App.Draw(Rand1);
	App.Draw(Rand2);
	App.Draw(Rand3);

	//Anzeigen zeichnen
	App.Draw(m_sLeben);
	App.Draw(m_sPunkte);
	App.Draw(m_sLevel);

	//Board zeichnen
	App.Draw(Board);

	//Ball zeichnen
	App.Draw(Ball);

	//sichtbare Bloecke zeichnen
	for(int i=0;i<anzahl_bloecke;i++)
	{
		if(Block[i]->isVisible())
			App.Draw(*Block[i]);
	}

	//bei manchen Spielzustaenden wird eine Meldung ausgegeben, z.B. bei Pause oder Sieg
	if(gameState==Paused || gameState==SystemPaused)
	{
		displayPause();
	}
	else if(gameState==Won)
	{
		displaySieg();
	}
	else if(gameState==LevelComplete)
	{
		displayLevelGeschafft();
	}
	else if(gameState==GameOver)
	{
		displayGameOver();
	}
}

//Bewegt das Spiel
void CGameBoard::move()
{
	//Zeit setzen
	m_ElapsedTime = App.GetFrameTime();

	if(gameState==Paused)//Wenn Spiel pausiert wird es nicht bewegt
		return;

	if(gameState==Won)//Wenn Spiel gewonnen ist wird es auch nicht bewegt
		return;

	if(gameState==GameOver)//Wenn Spiel verloren ist wird es auch nicht bewegt
		return;

	if(gameState==SystemPaused)//Wenn Spiel pausiert ist wird es auch nicht bewegt
		return;

	if(sleep)//Wenn sleep groesser als 0 wird das Spielfeld auch nicht bewegt
	{
		sleep-=m_ElapsedTime;
		if(sleep<=0)//Wenn das Einfrieren vorbei ist wird das Spiel zurueckgesetzt
		{
			reset();//setzt Board in die Mitte, zieht Leben ab, etc.
		}
		return;
	}

	//Board bewegen
	if(!Board.isFreezed())//Board wird eingefroren, wenn Ball nach unten abprallt, man also schon verloren hat
	{
		
		if(CSettings::steuerungTastatur)
		{
			//Steuerung mit Tastatur
			if (App.GetInput().IsKeyDown(sf::Key::Left))  Board.Move(-Typedef::boardSpeed * m_ElapsedTime, 0);
			if (App.GetInput().IsKeyDown(sf::Key::Right)) Board.Move( Typedef::boardSpeed * m_ElapsedTime, 0);
		}
		else
		{
			//Steuerung mit Maus
			Board.SetX(App.GetInput().GetMouseX()-Typedef::boardBreite/2);
		}

		Board.checkBoardStillInRange();
	}

	//Wenn GameState==Starting "klebt" der Ball am Board
	if(gameState==Starting)
	{
		Ball.SetX(Board.GetPosition().x+Typedef::boardBreite/2);
	}

	//Ball bewegen
	if(gameState==Running)
	{

		Ball.Move(Typedef::ballSpeed*Ball.rv.x*m_ElapsedTime,Typedef::ballSpeed*Ball.rv.y*m_ElapsedTime);

		//Kollisionen von Ball ueberpruefen
		this->checkKollisionBallMitRand(Ball);
		this->checkKollisionBallMitBoard(Ball,Board);

		//Kollision mit allen Bloecken ueberpruefen
		for(int i=0;i<anzahl_bloecke;i++)
		{
			if(!Block[i]->isVisible())//nur mit noch sichtbaren Bloecken testen
				continue;
			switchKollisionBallMitBlock(Ball,*Block[i],checkKollisionBallMitBlock(Ball,*Block[i]));
		}

		Ball.correctRV();//ggf. RV korrigieren wenn y-Bewegung zu klein
	}

	//Wenn noch ein Block sichtbar ist, wird die Funktion hier verlassen!!!
	if(!CBlock::alleZerstoert())
		return;

	//wird nur ausgefuehrt, wenn alle Bloecke weg sind
	if(spielfeld.size()==m_aktuellesLevel)//alle Level gewonnen
	{
		gameState=Won;
	}
	else//1 Sekunde pausieren und dann naechstes Level laden
	{
		gameState=LevelComplete;
		sleep=1.f;
		m_aktuellesLevel++;
	}
}

//pausiert das Spiel
void CGameBoard::pause()
{
	if(gameState!=Won && gameState!=GameOver)
	{
		oldState=gameState;//alter Status wird gespeichert, sonst wuerde z.B. der Status Starting ueberschrieben werden
		gameState=Paused;
	}
}

//beendet die Pause
void CGameBoard::resume()
{
	if(gameState!=Won && gameState!=GameOver)
		gameState=oldState;
}

//gibt true zurueck, wenn der Status auf Running steht
bool CGameBoard::isRunning()
{
	return (gameState==Running);
}

//Ball faengt an sich zu bewegen
void CGameBoard::start()
{
	if(gameState!=Won && gameState!=GameOver && !sleep)
	{
		gameState=Running;
		//Ball.SetPosition(Board.GetPosition().x+Typedef::boardBreite/2,600.f-Typedef::boardHoehe-10.f);
	}
}

//true wenn Spiel von Benutzer pausiert
bool CGameBoard::isPaused()
{
	return (gameState==Paused);
}

//zeigt den Text "Pause" an
void CGameBoard::displayPause()
{
	sf::String Pause;
	Pause.SetText("Pause");
	Pause.SetFont(sf::Font::GetDefaultFont());
	Pause.SetSize(50);
	Pause.SetColor(sf::Color(0,0,255));
	sf::Rect<float> rect(Pause.GetRect());
	Pause.Move(400-(rect.Right-rect.Left)/2.f,300-(rect.Bottom-rect.Top)/2.f);

	App.Draw(Pause);
}

//zeigt den Text "Gewonnen" an
void CGameBoard::displaySieg()
{
	sf::String Sieg;
	Sieg.SetText("Gewonnen");
	Sieg.SetFont(sf::Font::GetDefaultFont());
	Sieg.SetSize(50);
	Sieg.SetColor(sf::Color(0,0,255));
	sf::Rect<float> rect(Sieg.GetRect());
	Sieg.Move(400-(rect.Right-rect.Left)/2.f,300-(rect.Bottom-rect.Top)/2.f);

	App.Draw(Sieg);
}

//zeigt den Text "Level Geschafft" an
void CGameBoard::displayLevelGeschafft()
{
	sf::String lGeschafft;
	lGeschafft.SetText("Level Geschafft");
	lGeschafft.SetFont(sf::Font::GetDefaultFont());
	lGeschafft.SetSize(50);
	lGeschafft.SetColor(sf::Color(0,0,255));
	sf::Rect<float> rect(lGeschafft.GetRect());
	lGeschafft.Move(400-(rect.Right-rect.Left)/2.f,300-(rect.Bottom-rect.Top)/2.f);

	App.Draw(lGeschafft);
}

//zeigt den Text "Game Over" an
void CGameBoard::displayGameOver()
{
	sf::String GameOver;
	GameOver.SetText("Game Over");
	GameOver.SetFont(sf::Font::GetDefaultFont());
	GameOver.SetSize(50);
	GameOver.SetColor(sf::Color(0,0,255));
	sf::Rect<float> rect(GameOver.GetRect());
	GameOver.Move(400-(rect.Right-rect.Left)/2.f,300-(rect.Bottom-rect.Top)/2.f);

	sf::String Continue;
	Continue.SetText("Drücken Sie \"c\", um das Spiel fortzusetzen.");
	Continue.SetFont(sf::Font::GetDefaultFont());
	Continue.SetSize(25);
	Continue.SetColor(sf::Color(0,0,255));
	sf::Rect<float> rect2(Continue.GetRect());
	Continue.Move(400-(rect2.Right-rect.Left)/2.f,350-(rect2.Bottom-rect.Top)/2.f);

	App.Draw(GameOver);
	App.Draw(Continue);
}

//laedt bestimmtes Level aus Array
void CGameBoard::loadLevel(unsigned int level)
{
	if(level>spielfeld.size() || level<=0)//Fehler, Level exisitiert nicht
		return;
	//alte Bloecke loeschen
	for(unsigned int i=0;i<Block.size();i++)
	{
		delete Block[i];
		Block[i]=0;
	}
	Block.clear();
	for(unsigned int i=0;i<spielfeld[level-1].size();++i)
	{
		if(spielfeld[level-1][i])//Wenn auf 0 gesetzt, wird der Block nicht erzeugt
		{
			Block.push_back(new CBlock(i%17*45.f+20,i/17*15.f+45,spielfeld[level-1][i]));
		}
	}
	anzahl_bloecke=static_cast<int>(Block.size());
	Ball.createRV();
}

//legt Array an, aus dem die Level geladen werden koennen
void CGameBoard::initLevel()
{
	//Anzahl der Level bestimmen
	int anzahlLevel=0;
	std::ifstream levelCounter;
	std::stringstream dateiname;
	dateiname << "Level" << anzahlLevel+1 << ".lev";
	levelCounter.open(dateiname.str().c_str());
	while(levelCounter.is_open())//Geht solange Dateien durch bis die gewuenschte Datei nicht mehr existiert
	{
		anzahlLevel++;
		std::stringstream dateiname;
		dateiname << "Level" << anzahlLevel+1 << ".lev";
		levelCounter.close();
		levelCounter.open(dateiname.str().c_str());
	}

	spielfeld.resize(anzahlLevel);

	for(unsigned int i=0;i<spielfeld.size();i++)//geht alle Level durch und schreib Aufbau in das Array spielfeld
	{
		//Erzeugt Dateinamen der zu oeffnenden Datei
		std::stringstream dateiname;
		dateiname << "Level" << i+1 << ".lev";

		std::ifstream level;
		level.open(dateiname.str().c_str());
		char ch;
		//Fuellt Array
		while(level.get(ch))
		{
			if(spielfeld[i].size()>=391 && ch!=',')//Begrenzung bei Anzahl der moeglichen Steine (bei laengeren Dateien wuerden diese unendlich weit nach unten weitergehen)
			{
				CLog::getInst().writeWarning("Eingelesene Leveldatei \""+std::string(dateiname.str())+"\" hat zu viele Eintraege.");
				break;
			}
			if(ch=='0' || ch=='1' || ch=='2')
			{
				spielfeld[i].push_back(ch-48);
			}
		}
		level.close();
		if(spielfeld[i].size()<=0)//Level-Datei war leer
		{
			CLog::getInst().writeWarning("Eingelesene Leveldatei \""+std::string(dateiname.str())+"\" ist leer.");
		}
	}

	if(spielfeld.size()==0)
	{
		CLog::getInst().writeError("Es konnten keine Level-Daten gefunden werden.");
		App.Close();
	}
}

//wenn das Fenster den Fokus verliert, wird das Spiel pausiert
void CGameBoard::systemPause()
{
	if(gameState!=Won && gameState!=GameOver)
	{
		oldState=gameState;
		gameState=SystemPaused;
	}
}

void CGameBoard::updateLeben()
{
	std::stringstream stream;
	stream << "Leben: " << m_leben;
	m_sLeben.SetText(stream.str());
	m_sLeben.SetPosition(10,10);
}

void CGameBoard::updatePunkte()
{
	std::stringstream stream;
	stream << "Punkte: " << m_punkte;
	m_sPunkte.SetText(stream.str());
	sf::Rect<float> rect(m_sPunkte.GetRect());
	m_sPunkte.SetPosition(800/2.f-(rect.Right-rect.Left)/2.f,10);//mittig platzieren
}

void CGameBoard::updateLevel()
{
	std::stringstream stream;
	stream << "Level: " << m_aktuellesLevel;
	m_sLevel.SetText(stream.str());
	sf::Rect<float> rect(m_sLevel.GetRect());
	m_sLevel.SetPosition(790-(rect.Right-rect.Left),10);//rechts platzieren
}

void CGameBoard::reset()
{
	sleep=0;
	if(gameState==LevelComplete || gameState==LostLife)
	{
		Board.SetPosition(800/2.f-30,600-10);
		Ball.SetPosition(Board.GetPosition().x+Typedef::boardBreite/2,600.f-Typedef::boardHoehe-10.f);
		Board.setFreeze(false);
		updateLevel();
		if(gameState==LevelComplete)
		{
			loadLevel(m_aktuellesLevel);
		}
		else if(gameState==LostLife)
		{
			m_leben--;
			updateLeben();
			if(m_leben==0)
			{
				gameState=GameOver;
			}
		}
		if(gameState!=GameOver)
			gameState=Starting;
	}
}

int CGameBoard::checkKollisionBallMitBlock(CBall& Ball, CBlock& Block)
{
	sf::Rect<float> ballRect = Ball.GetBallRect();
	sf::Rect<float> blockRect = Block.GetBlockRect();
	if(!ballRect.Intersects(blockRect))//auf jeden Fall keine Kollision
	{
		//sollte ggf. durch performantere Erkennung ersetzt werden, da es in jedem Frame mit allen Bloecken ausgeführt wird
		return 0;
	}
	else
	{
		//Wenn Mittelpunkt zwischen Rect-Grenzen rechts und links und Ball-Bewegung nach unten, dann Kollision an oberer Kante
		if(Ball.GetPosition().x>=blockRect.Left && Ball.GetPosition().x<=blockRect.Right && Ball.rv.y>0)
		{
			//Kollision oben
			return 1;
		}
		else if(Ball.GetPosition().y>=blockRect.Top && Ball.GetPosition().y <= blockRect.Bottom && Ball.rv.x <0)
		{
			//Kollision rechts
			return 2;
		}
		else if(Ball.GetPosition().x>=blockRect.Left && Ball.GetPosition().x<=blockRect.Right && Ball.rv.y<0)
		{
			//Kollision unten
			return 3;
		}
		else if(Ball.GetPosition().y>=blockRect.Top && Ball.GetPosition().y <= blockRect.Bottom && Ball.rv.x >0)
		{
			//Kollision links
			return 4;
		}
		else if(Ball.GetPosition().x>blockRect.Right && Ball.GetPosition().y < blockRect.Top && sqrt(pow(Ball.GetPosition().x-blockRect.Right,2.f)+pow(Ball.GetPosition().y-blockRect.Top,2.f))<=Ball.GetRadius())
		{
			//Kollision an rechter oberer Ecke
			return 5;
		}
		else if(Ball.GetPosition().x>blockRect.Right && Ball.GetPosition().y > blockRect.Bottom && sqrt(pow(Ball.GetPosition().x-blockRect.Right,2.f)+pow(Ball.GetPosition().y-blockRect.Bottom,2.f))<=Ball.GetRadius())
		{
			//Kollision an rechter unterer Ecke
			return 6;
		}
		else if(Ball.GetPosition().x<blockRect.Left && Ball.GetPosition().y > blockRect.Bottom && sqrt(pow(Ball.GetPosition().x-blockRect.Left,2.f)+pow(Ball.GetPosition().y-blockRect.Bottom,2.f))<=Ball.GetRadius())
		{
			//Kollision an linker unterer Ecke
			return 7;
		}
		else if(Ball.GetPosition().x<blockRect.Left && Ball.GetPosition().y < blockRect.Top && sqrt(pow(Ball.GetPosition().x-blockRect.Left,2.f)+pow(Ball.GetPosition().y-blockRect.Top,2.f))<=Ball.GetRadius())
		{
			//Kollision an linker oberer Ecke
			return 8;
		}
		else
		{
			return 0;
			//doch keine Kollision
		}
	}
}

void CGameBoard::switchKollisionBallMitBlock(CBall& Ball, CBlock& Block, int kollision)
{
	//Punkte erhoehen
	if(kollision)
	{
		m_punkte+=10;
		updatePunkte();
	}
	else
	{
		return;
	}
	sf::Rect<float>	blockRect=Block.GetBlockRect();
	switch(kollision)
	{
	case 0: return;
			break;
	case 1:	
	case 3:	Ball.rv.y=-Ball.rv.y;
			Block.hit();
			break;
	case 2:	
	case 4:	Ball.rv.x=-Ball.rv.x;
			Block.hit();
			break;
	case 5:	setNewBallRvAfterBlockCornerCollision(Ball,Ball.GetPosition().x-blockRect.Right,Ball.GetPosition().y-blockRect.Top);
			Block.hit();
			break;
	case 6:	setNewBallRvAfterBlockCornerCollision(Ball,Ball.GetPosition().x-blockRect.Right,Ball.GetPosition().y-blockRect.Bottom);
			Block.hit();
			break;
	case 7:	setNewBallRvAfterBlockCornerCollision(Ball,Ball.GetPosition().x-blockRect.Left,Ball.GetPosition().y-blockRect.Bottom);
			Block.hit();
			break;
	case 8:	setNewBallRvAfterBlockCornerCollision(Ball,Ball.GetPosition().x-blockRect.Left,Ball.GetPosition().y-blockRect.Top);
			Block.hit();
			break;
	}
	while(checkKollisionBallMitBlock(Ball,Block))
	{
		Ball.Move(Typedef::ballSpeed*m_ElapsedTime*Ball.rv.x,Typedef::ballSpeed*m_ElapsedTime*Ball.rv.y);
	}
}

void CGameBoard::setNewBallRvAfterBlockCornerCollision(CBall& Ball,float x_diff, float y_diff)
{
	float laenge_temp=sqrt(pow(x_diff,2.f)+pow(y_diff,2.f));
	//normierter Vektor zwischen Ecke und Mittelpunkt des Kreises
	sf::Vector2f verbindungEckeMittelpunkt=sf::Vector2f(x_diff/laenge_temp,y_diff/laenge_temp);
	//umgekehrter RV des Balls
	sf::Vector2f RVumgekehrt=-Ball.rv;
	//berechnet den Winkel des Verbindungsvektors zur Horizontalen
	float winkelNormalenVektor=acos(verbindungEckeMittelpunkt.x);
	if(verbindungEckeMittelpunkt.y>0.f)
	{
		//wenn der y-Wert des Vektors positiv ist, ist der Winkel > 180°, acos gibt aber nur Werte von 0-180 zurueck
		winkelNormalenVektor=static_cast<float>(2*PI-winkelNormalenVektor);
	}
	//berechnungen analog zu oben
	float ballWinkel=acos(RVumgekehrt.x);
	if(RVumgekehrt.y>0.f)
	{
		ballWinkel=static_cast<float>(2*PI-ballWinkel);
	}
	//Der Winkel des neuen RV setzt sich aus den beiden anderen Winkeln zusammen
	float neuerWinkel=2*winkelNormalenVektor-ballWinkel;
	Ball.rv.x=cos(neuerWinkel);
	Ball.rv.y=sqrt(-pow(Ball.rv.x,2.f)+1);
	//cos behandelt alle Winkel, als wuerden sie zwischen 0 und 180 liegen, deshalb manuelle Anpassung
	if(neuerWinkel>PI)//Wenn der neue Winkel ueber 180 Grad liegt, fliegt der Ball nach unten, also ist y positiv
	{
		Ball.rv.y=fabs(Ball.rv.y);
	}
	else
	{
		Ball.rv.y=-fabs(Ball.rv.y);
	}
}

void CGameBoard::checkKollisionBallMitRand(CBall& Ball)
{
	sf::Rect<float> ball = Ball.GetBallRect();

	//oberer Rand
	if(ball.Top<40.f)
	{
		Ball.rv.y=-Ball.rv.y;//einfach y-Wert umkehren
		float y_temp=ball.Top;
		while(y_temp<40.f)//prueft ob der Ball noch "im" Rand liegt
		{
			Ball.Move(Typedef::ballSpeed*m_ElapsedTime*Ball.rv.x,Typedef::ballSpeed*m_ElapsedTime*Ball.rv.y);//Schiebt Ball aus Rand raus, verhindert "steckenbleiben"
			y_temp+=Typedef::ballSpeed*m_ElapsedTime*Ball.rv.y;
		}
		//der Ball wird solange weitergeschoben, bis er nicht mehr im Rand liegt
	}
	//rechter Rand
	if(ball.Right>790.f)
	{
		Ball.rv.x=-Ball.rv.x;
		float x_temp=ball.Right;
		while(x_temp>790.f)
		{
			Ball.Move(Typedef::ballSpeed*m_ElapsedTime*Ball.rv.x,Typedef::ballSpeed*m_ElapsedTime*Ball.rv.y);
			x_temp+=Typedef::ballSpeed*m_ElapsedTime*Ball.rv.x;
		}
	}
	//linker Rand
	if(ball.Left<10.f)
	{
		Ball.rv.x=-Ball.rv.x;
		float x_temp=ball.Left;
		while(x_temp<10.f)
		{
			Ball.Move(Typedef::ballSpeed*m_ElapsedTime*Ball.rv.x,Typedef::ballSpeed*m_ElapsedTime*Ball.rv.y);
			x_temp+=Typedef::ballSpeed*m_ElapsedTime*Ball.rv.x;
		}
	}
}

void CGameBoard::checkKollisionBallMitBoard(CBall& Ball,CBoard &board)
{
	sf::Rect<float> ball = Ball.GetBallRect();
	if(ball.Bottom>590.f)
	{
		if(this->ballIntersectsBoard(Ball,board))
		{
			sf::Rect<float> boardRect = board.GetBoardRect();
			float mitteBoard=boardRect.Right-(boardRect.Right-boardRect.Left)/2.f;//berechnet x Wert der Board-Mitte
			float x_temp=Ball.GetPosition().x-mitteBoard;//berechnet Different zwischen Ball- und Board-Mittelpunkt
			this->setNewBallRvAfterBoardCollision(Ball,board,x_temp);
			while(this->ballIntersectsBoard(Ball,board))//schiebt Ball aus dem Board raus
			{
				Ball.Move(Typedef::ballSpeed*m_ElapsedTime*Ball.rv.x,Typedef::ballSpeed*m_ElapsedTime*Ball.rv.y);
			}
		}
	}
	else
	{
		//gameover=false;
	}
	if(ball.Bottom>=600.f)
	{
		gameState=LostLife;
		//gameover=true;
		Ball.createRV();
		board.setFreeze(false);
		sleep=1.f;//pausiert das komplette Spiel fuer 1 Sekunde, dadurch wird sichtbar dass das Board den Ball verfehlt hat
	}
}

bool CGameBoard::ballIntersectsBoard(CBall& Ball,CBoard& board)
{
	sf::Rect<float> ball = Ball.GetBallRect();
	sf::Rect<float> blockRect = board.GetBoardRect();
	if(!ball.Intersects(blockRect))//auf jeden Fall keine Kollision
	{
		return false;
	}
	else if((Ball.GetPosition().x>=blockRect.Left && Ball.GetPosition().x<=blockRect.Right && Ball.rv.y>0) ||//obere Kante
		(Ball.GetPosition().x>blockRect.Right && Ball.GetPosition().y < blockRect.Top && sqrt(pow(Ball.GetPosition().x-blockRect.Right,2.f)+pow(Ball.GetPosition().y-blockRect.Top,2.f))<=Ball.GetRadius()) ||//rechte obere Ecke
		(Ball.GetPosition().x<blockRect.Left && Ball.GetPosition().y < blockRect.Top && sqrt(pow(Ball.GetPosition().x-blockRect.Left,2.f)+pow(Ball.GetPosition().y-blockRect.Top,2.f))<=Ball.GetRadius()))//linke obere Ecke
	{
		return true;
	}
	else
	{
		return false;
	}
}

void CGameBoard::setNewBallRvAfterBoardCollision(CBall& Ball,CBoard& board,float x)
{
	sf::Rect<float> boardRect = board.GetBoardRect();
	float boardBreite=boardRect.Right-boardRect.Left;
	float x_temp=x/(boardBreite/2.f+10.f/sqrt(2.f))*sqrt(2.f)/2;
	sf::Vector2f n=sf::Vector2f(x_temp,-sqrt(1-pow(x_temp,2.f)));
	sf::Vector2f RVumgekehrt=-Ball.rv;
	float winkelNormalenVektor=acos(n.x);
	float ballWinkel=acos(RVumgekehrt.x);
	float neuerWinkel=2*winkelNormalenVektor-ballWinkel;
	Ball.rv.x=cos(neuerWinkel);
	Ball.rv.y=-sqrt(-pow(Ball.rv.x,2.f)+1);
	if(neuerWinkel>PI)//Wenn der neue Winkel ueber 180 Grad liegt, fliegt der Ball nach unten, also Spiel vorbei
	{
		if(fabs(x)>(boardBreite/2.f+10.f/sqrt(2.f)))
		{
			//Vektor wird gesetzt (y>0, Ball fliegt nach unten)
			Ball.rv.y=-Ball.rv.y;
			//Board bewegt sich nicht mehr, bis Ball den unteren Rand berührt hat
			board.setFreeze(true);
		}
		else
		{
			if(x>0)
			{
				Ball.rv.x=0.9949874371f;
				Ball.rv.y=-0.1f;
			}
			else
			{
				Ball.rv.x=-0.9949874371f;
				Ball.rv.y=-0.1f;
			}
		}
		return;
	}
	//gameover=false;
}

bool CGameBoard::isGameOver()
{
	return (gameState==GameOver);
}

void CGameBoard::cont()
{
	restart(m_aktuellesLevel);
}
