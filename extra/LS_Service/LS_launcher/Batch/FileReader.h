#pragma once
#include <fstream>

using namespace std;

class FileReader
{
public:
	FileReader(void);
	~FileReader(void);

private:
	void Init();
	void Destroy();
	
public:
	BOOL OpenFile( TCHAR* fileName );

	BOOL GetLine( char* line, int len );

private:
	TCHAR		m_fileName[_MAX_FNAME];
	ifstream	m_readFile;
};

