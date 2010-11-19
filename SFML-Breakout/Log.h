#pragma once

#include <string>
#include <fstream>

class CLog
{
private:
	CLog(void);
public:
	~CLog(void);
	bool write(std::string eintrag);
	bool writeWarning(std::string eintrag);
	bool writeInfo(std::string eintrag);
	bool writeError(std::string eintrag);
	void close();
	bool init();
	static CLog& getInst();
private:
	std::ofstream log;
	static CLog inst;
};
