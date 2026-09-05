#pragma once

#include <vector>

typedef vector<int> TOKENS;

class ioIP
{
public:
	ioIP(void);
	ioIP(const string IP);
	~ioIP(void);

	void Init();
	void Destroy();

public:
	ioIP& operator=(const ioIP& IP);
	ioIP& operator=(const string& IP);
	bool operator==(const string& IP);
	bool operator==(const ioIP &IP) const;

public:
	void GetIP(string& IP) const;

protected:
	void SetIP(const string& IP);
	void Tokenize(const string& str, const string& delimiters, TOKENS& tokens);

protected:
	int m_IP[4], m_IPex[4];
	int m_subnet;
};