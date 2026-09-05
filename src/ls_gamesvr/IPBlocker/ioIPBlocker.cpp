#include "stdafx.h"
#include "cFile.h"
#include "ioIP.h"
#include "ioIPBlocker.h"

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

BOOL ioIPBlocker::Load(const char* fileName)
{
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

BOOL ioIPBlocker::Find(char* IP)
{
	
	/*ioIP blockIP;
	for(IP_LIST::iterator it = m_IPs.begin() ; it != m_IPs.end() ; ++it)
	{
		blockIP = *it;
		if(blockIP == IP)
		{
			return TRUE;
		}
	}*/
	
	
	ioIP userIP = IP;
	ioIP blockIP;
	for(IP_LIST::iterator it = m_IPs.begin() ; it != m_IPs.end() ; ++it)
	{
		blockIP = *it;
		if( blockIP == userIP )
		{
			return TRUE;
		}
	}
	
	return FALSE;
}

BOOL ioIPBlocker::IsActive()
{
	return (m_IPs.size() != 0) ? TRUE : FALSE;
}

BOOL ioIPBlocker::Tokenize(const std::string& str, const std::string& delimiters)
{
	m_IPs.clear();

	string::size_type lastPos = str.find_first_not_of(delimiters, 0);
	string::size_type pos = str.find_first_of(delimiters, lastPos);
	
	std::string token;
	while (string::npos != pos || string::npos != lastPos)
	{
		token = str.substr(lastPos, pos - lastPos);

		ioIP temp = token;
		m_IPs.push_back(temp);

		lastPos = str.find_first_not_of(delimiters, pos);

		pos = str.find_first_of(delimiters, lastPos);
	}
	return (m_IPs.size() > 0) ? TRUE : FALSE;
}