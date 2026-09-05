#include "stdafx.h"
#include "cFile.h"
#include "ioIP.h"

using namespace std;

ioIP::ioIP(void)
{
	Init();
}

ioIP::ioIP(const string IP)
{
	Init();
	SetIP(IP);
}

ioIP::~ioIP(void)
{
	Destroy();
}

void ioIP::Init()
{
	m_subnet = 0;
	ZeroMemory(m_IP, sizeof(m_IP));
	ZeroMemory(m_IPex, sizeof(m_IPex));
}

void ioIP::Destroy()
{
}

//operator
ioIP& ioIP::operator=(const ioIP& IP)
{
	this->m_subnet = IP.m_subnet;
	CopyMemory(m_IP, IP.m_IP, sizeof(m_IP));
	CopyMemory(m_IPex, IP.m_IPex, sizeof(m_IPex));
	return *this;
}

ioIP& ioIP::operator=(const string& IP)
{
	SetIP(IP);
	return *this;
}

bool ioIP::operator==(const std::string& IP)
{
	static TOKENS tokens;
	tokens.clear();
	Tokenize(IP, ".", tokens);

	if(tokens.size() == 4)
	{
		if((m_IP[0] > tokens[0]) || (m_IPex[0] < tokens[0])) return false;
		if((m_IP[1] > tokens[1]) || (m_IPex[1] < tokens[1])) return false;
		if((m_IP[2] > tokens[2]) || (m_IPex[2] < tokens[2])) return false;
		if((m_IP[3] > tokens[3]) || (m_IPex[3] < tokens[3])) return false;
		return true;
	}
	return false;
}

bool ioIP::operator==(const ioIP& IP) const
{
	if((m_IP[0] > IP.m_IP[0]) || (m_IPex[0] < IP.m_IPex[0])) return false;
	if((m_IP[1] > IP.m_IP[1]) || (m_IPex[1] < IP.m_IPex[1])) return false;
	if((m_IP[2] > IP.m_IP[2]) || (m_IPex[2] < IP.m_IPex[2])) return false;
	if((m_IP[3] > IP.m_IP[3]) || (m_IPex[3] < IP.m_IPex[3])) return false;

	return true;
}

void ioIP::GetIP(string& IP) const
{ 
	char temp[32];
	sprintf_s(temp, "%d.%d.%d.%d", m_IP[0], m_IP[1], m_IP[2], m_IP[3]);

	IP = temp;
}

void ioIP::SetIP(const string& IP)
{
	static TOKENS tokens;
	tokens.clear();
	Tokenize(IP, "/", tokens);

	if(tokens.size() >= 2)
	{
		m_subnet = tokens[1];
	}

	Tokenize(IP, ".", tokens);
	if(tokens.size() == 4)
	{
		this->m_IP[0] = tokens[0];
		this->m_IP[1] = tokens[1];
		this->m_IP[2] = tokens[2];
		this->m_IP[3] = tokens[3];
	}

	if(0 != m_subnet)
	{
		int div = m_subnet / 8;
		int quo = m_subnet - (div * 8);

		for(int i = 0 ; i < div ; i++)
		{
			m_IPex[i] = m_IP[i];
		}

		if(quo > 0)
		{
			int range = pow(2.0,  (8 - quo)) - 1;
			m_IPex[div] = m_IP[div] + range;
			++div; 
		}

		for(int i = div ; i < 4 ; i++)
		{
			m_IPex[i] = m_IP[i] + 254;
		}
	}
	else
	{
		CopyMemory(m_IPex, m_IP, sizeof(m_IPex));
	}

	Debug(_T("%d.%d.%d.%d ~ %d.%d.%d.%d\r\n"), m_IP[0], m_IP[1], m_IP[2], m_IP[3], m_IPex[0], m_IPex[1], m_IPex[2], m_IPex[3]);
}

void ioIP::Tokenize(const string& str, const string& delimiters, TOKENS& tokens)
{
	tokens.clear();

	// Skip delimiters at beginning.
	string::size_type lastPos = str.find_first_not_of(delimiters, 0);
	// Find first "non-delimiter".
	string::size_type pos     = str.find_first_of(delimiters, lastPos);

	while (std::string::npos != pos || std::string::npos != lastPos)
	{
		// Found a token, add it to the vector.
		tokens.push_back( atoi(str.substr(lastPos, pos - lastPos).c_str()) );
		// Skip delimiters.  Note the "not_of"
		lastPos = str.find_first_not_of(delimiters, pos);
		// Find next "non-delimiter"
		pos = str.find_first_of(delimiters, lastPos);
	}
}