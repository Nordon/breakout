#include "SettingsFile.h"
#include "Log.h"
#include <fstream>
#include <algorithm>
#include <cctype>


CSettingsFile::CSettingsFile(std::string datei)
{
	initialisieren();
	if(!datei.empty())
		einlesen(datei);
}

CSettingsFile::~CSettingsFile(void)
{
}

size_t CSettingsFile::anzahlWerte()
{
	if(eingelesen)
		return eintrag.size();
	else
		return 0;
}

void CSettingsFile::initialisieren()
{
	eingelesen=false;
}

bool CSettingsFile::einlesen(std::string datei)
{
	std::string buffer,tmp;
	std::ifstream config(datei.c_str());
    if(config.is_open())//Wenn Datei geoeffnet werden kann
    {
		while(!config.eof())
        {
			char * buffer2=new char[256];
			config.getline(buffer2,255);
			buffer2[255]='\0';
			buffer=buffer2;
			delete[] buffer2;
			if(buffer.find("//")!=buffer.npos)//schneidet Kommentare ab
                buffer.erase(buffer.find("//"));

            tmp=buffer;
            if(buffer.find('=')!=buffer.npos)//Wenn das Zeichen = vorkommt
            {
                buffer.erase(buffer.find('='));//schneidet bei '=' ab
				std::transform(buffer.begin(), buffer.end(), buffer.begin(), toupper);
                //buffer = buffer.toUpper();//nicht case-sensitiv
				buffer.erase(std::remove_if(buffer.begin(), buffer.end(), std::isspace), buffer.end());//entfernt Whitespace
				tmp.erase(0,tmp.find('=')+1);
				tmp.erase(std::remove_if(tmp.begin(), tmp.end(), std::isspace), tmp.end());//entfernt Whitespace

				eintrag[buffer]=tmp;
            }
            else//kein '=' im Eintrag
            {
                //ignorieren
            }
        }
        config.close();
		eingelesen=true;
		return true;
    }
    else
    {
		CLog::getInst().writeWarning("Die Datei \""+datei+"\" konnte nicht geoeffnet werden.");
        return false;
    }
}

std::string CSettingsFile::returnWert(std::string name)
{
	if(eingelesen)
	{
		std::transform(name.begin(), name.end(), name.begin(), toupper);
		return eintrag.find(name)->second;
	}
	else
	{
		return "";
	}
}

bool CSettingsFile::isEingelesen()
{
	return eingelesen;
}

bool CSettingsFile::neuEinlesen(std::string datei)
{
	if(!datei.empty())
	{	
		eingelesen=false;
		eintrag.clear();
		return einlesen(datei);
	}
	else
	{
		return false;
	}
}