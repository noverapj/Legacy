#pragma once

#include <vector>

typedef vector<int> TOKENS;

class ioIP
{
public:
	ioIP(void);
	ioIP(const string IP);
	ioIP(const UINT begin, const UINT end);
	~ioIP(void);

	void Init();
	void Destroy();

public:
	ioIP& operator=(const ioIP& IP);
	ioIP& operator=(const string& IP);
	bool operator==(const string& IP);

public:
	void GetIP(string& IP);

protected:
	void SetIP(const string& IP);
	void SetIP(const UINT begin, const UINT end);
	void Tokenize(const string& str, const string& delimiters, TOKENS& tokens);

protected:
	BOOL m_range;
	UINT m_IPs[2];

	int m_IP[4], m_IPex[4];
	int m_subnet;
};