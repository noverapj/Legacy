#pragma once

#include <vector>

typedef std::vector<int> TOKENS;

class ioIP
{
public:
	ioIP(void);
	ioIP(const std::string IP);
	ioIP(const UINT begin, const UINT end);
	~ioIP(void);

	void Init();
	void Destroy();

public:
	ioIP& operator=(const ioIP& IP);
	ioIP& operator=(const std::string& IP);
	bool operator==(const std::string& IP);

public:
	void GetIP(std::string& IP);

protected:
	void SetIP(const std::string& IP);
	void SetIP(const UINT begin, const UINT end);
	void Tokenize(const std::string& str, const std::string& delimiters, TOKENS& tokens);

protected:
	BOOL m_range;
	UINT m_IPs[2];

	int m_IP[4], m_IPex[4];
	int m_subnet;
};