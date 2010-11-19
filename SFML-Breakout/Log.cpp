#include "Log.h"

CLog::CLog(void)
{
	log = std::ofstream("log.html");
	if(log.is_open())
	{
		log << "<html>" << std::endl << "<head><title>BreakOut-Log</title></head>" << std::endl << "<body>" << std::endl;
	}
}

CLog::~CLog(void)
{
	log << "</body>\n</html>";
	log.close();
}

CLog CLog::inst = CLog();

bool CLog::write(std::string eintrag)
{
	log << "<p>" << eintrag << "</p>" << std::endl;
	return true;
}

bool CLog::writeInfo(std::string eintrag)
{
	log << "<p><font color=\"#00CC00\"><b>Info:</b></font> " << eintrag << "</p>" << std::endl;
	return true;
}

bool CLog::writeWarning(std::string eintrag)
{
	log << "<p><font color=\"#FFB200\"><b>Warnung:</b></font> " << eintrag << "</p>" << std::endl;
	return true;
}

bool CLog::writeError(std::string eintrag)
{
	log << "<p><font color=\"#FF0000\"><b>Fehler:</b></font> " << eintrag << "</p>" << std::endl;
	return true;
}

CLog &CLog::getInst()
{
	return inst;
}

