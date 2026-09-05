#pragma once

#include <vector>
#include "ioIP.h"

class ioIPBlocker
{
public:
	ioIPBlocker(void);
	virtual ~ioIPBlocker(void);

	void Init();
	void Destroy();
 
public:
	BOOL Load(const char* fileName);
	BOOL Find(char* IP);
	BOOL IsActive();

protected:
	BOOL Tokenize(const std::string& str, const std::string& delimiters);

protected:
	typedef std::vector<ioIP> IP_LIST;
	IP_LIST m_IPs;
};