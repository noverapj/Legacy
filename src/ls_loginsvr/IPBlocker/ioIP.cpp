#include "stdafx.h"
#include "cFile.h"
#include "ioIP.h"

using namespace std;

ioIP::ioIP(void) : m_range(FALSE)
{
	Init();
}

ioIP::ioIP(const string IP) : m_range(FALSE)
{
	Init();
	SetIP(IP);
}

ioIP::ioIP(const UINT begin, const UINT end) : m_range(TRUE)
{
	Init();
	SetIP(begin, end);
}

ioIP::~ioIP(void)
{
	Destroy();
}

void ioIP::Init()
{
	m_subnet = 0;
	ZeroMemory(m_IPs, sizeof(m_IPs));
	ZeroMemory(m_IP, sizeof(m_IP));
	ZeroMemory(m_IPex, sizeof(m_IPex));
}

void ioIP::Destroy()
{
}

//operator
ioIP& ioIP::operator=(const ioIP& IP)
{
	if(IP.m_range)
	{
		this->m_range = IP.m_range;
		CopyMemory(m_IPs, IP.m_IPs, sizeof(m_IPs));
	}
	else
	{
		this->m_range = IP.m_range;
		this->m_subnet = IP.m_subnet;
		CopyMemory(m_IP, IP.m_IP, sizeof(m_IP));
		CopyMemory(m_IPex, IP.m_IPex, sizeof(m_IPex));
		return *this;
	}
}

ioIP& ioIP::operator=(const string& IP)
{
	SetIP(IP);
	return *this;
}

bool ioIP::operator==(const std::string& IP)
{
	if(m_range)
	{
		UINT IPnum = htonl(inet_addr(IP.c_str()));
		if(IPnum >= m_IPs[0] && IPnum <= m_IPs[1])
			return true;
		return false;
	}
	else
	{
		TOKENS tokens;
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
}

void ioIP::GetIP(string& IP)
{ 
	char temp[32];
	sprintf_s(temp, "%d.%d.%d.%d", m_IP[0], m_IP[1], m_IP[2], m_IP[3]);

	IP = temp;
}

void ioIP::SetIP(const string& IP)
{
	TOKENS tokens;
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

	char start[64], end[64];
	sprintf_s(start, "%d.%d.%d.%d",  m_IP[0], m_IP[1], m_IP[2], m_IP[3]);
	sprintf_s(end, "%d.%d.%d.%d",  m_IPex[0], m_IPex[1], m_IPex[2], m_IPex[3]);

	m_IPs[0] = htonl(inet_addr(start));
	m_IPs[1] = htonl(inet_addr(end));

	Debug(_T("%d.%d.%d.%d ~ %d.%d.%d.%d\r\n"), m_IP[0], m_IP[1], m_IP[2], m_IP[3], m_IPex[0], m_IPex[1], m_IPex[2], m_IPex[3]);
}

void ioIP::SetIP(const UINT begin, const UINT end)
{
	m_IPs[0] = begin;
	m_IPs[1] = end;
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