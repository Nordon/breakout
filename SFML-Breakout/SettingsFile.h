#pragma once

#include <string>
#include <map>

class CSettingsFile
{
public:
	CSettingsFile(std::string datei="");
	~CSettingsFile(void);
	bool isEingelesen();
	bool neuEinlesen(std::string datei);
	size_t anzahlWerte();
	std::string returnWert(std::string name);

private:
	void initialisieren();
	bool einlesen(std::string datei);
	std::map<std::string,std::string> eintrag;
	bool eingelesen;
};
