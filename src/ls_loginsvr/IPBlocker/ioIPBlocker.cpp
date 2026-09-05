#include "stdafx.h"
#include "cFile.h"
#include "ioIPBlocker.h"

  
BOOL tokenize(const std::string& str, const std::string& delimiters, std::vector<UINT>& tokens)
{
	tokens.clear();

	string::size_type lastPos = str.find_first_not_of(delimiters, 0);
	string::size_type pos = str.find_first_of(delimiters, lastPos);
	
	std::string token;
	while (string::npos != pos || string::npos != lastPos)
	{
		token = str.substr(lastPos, pos - lastPos);

		char *ptr = NULL;
		UINT value = strtoul( token.c_str(), &ptr, 10);
		tokens.push_back( value );

		lastPos = str.find_first_not_of(delimiters, pos);

		pos = str.find_first_of(delimiters, lastPos);
	}
	return (tokens.size() > 0) ? TRUE : FALSE;
}


ioIPBlocker::ioIPBlocker(void)
{
	Init();
}

ioIPBlocker::~ioIPBlocker(void)
{
	Destroy();
}

void ioIPBlocker::Init()
{
	m_IPs.reserve(1024);
}

void ioIPBlocker::Destroy()
{
}

BOOL ioIPBlocker::Load(const char* fileName, const BOOL range)
{
	// 영역검사 여부
	m_range = range;

	cFileReader file;
	if(!file.Open( fileName ))		return FALSE;

	DWORD length = file.GetFileSize(); 
	BYTE *buffer = new BYTE[length+1];
	if(!buffer) return FALSE;

	ZeroMemory(buffer, length+1);
	if(!file.Read(buffer, length))	return FALSE;

	std::string text = reinterpret_cast<char*>(buffer);
	if(!Tokenize(text, "\r\n"))		return FALSE;
	return TRUE;
}

BOOL ioIPBlocker::Find(const char* IP)
{
	ioIP blockIP;
	for(IP_LIST::iterator it = m_IPs.begin() ; it != m_IPs.end() ; ++it)
	{
		blockIP = *it;
		if(blockIP == IP)
		{
			Debug(_T("%s is blocked\r\n"), IP);
			return TRUE;
		}
	}
	Debug(_T("%s is accepted\r\n"), IP);
	return FALSE;
}

BOOL ioIPBlocker::IsActive()
{
	return (m_IPs.size() != 0) ? TRUE : FALSE;
}

std::vector<UINT> TEMP;
BOOL ioIPBlocker::Tokenize(const std::string& str, const std::string& delimiters)
{
	m_IPs.clear();

	string::size_type lastPos = str.find_first_not_of(delimiters, 0);
	string::size_type pos = str.find_first_of(delimiters, lastPos);
	
	std::string token;
	while (string::npos != pos || string::npos != lastPos)
	{
		token = str.substr(lastPos, pos - lastPos);
		
		if(m_range)
		{
			TEMP.clear();
			tokenize(token, "\t", TEMP);

			m_IPs.push_back(ioIP(TEMP[0], TEMP[1]));
		}
		else
		{
			ioIP temp = token;
			m_IPs.push_back(temp);
		}
		lastPos = str.find_first_not_of(delimiters, pos);

		pos = str.find_first_of(delimiters, lastPos);
	}
	return (m_IPs.size() > 0) ? TRUE : FALSE;
}