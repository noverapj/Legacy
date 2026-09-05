#pragma once

#include <vector>
#include "ioIP.h"

class ioIPBlocker
{
public:
	ioIPBlocker(void);
	~ioIPBlocker(void);

	void Init();
	void Destroy();

public:
	BOOL Load(const char* fileName, const BOOL range = FALSE);
	BOOL Find(const char* IP);
	BOOL IsActive();

protected:
	BOOL Tokenize(const std::string& str, const std::string& delimiters);

protected:
	typedef std::vector<ioIP> IP_LIST;

	BOOL m_range;
	IP_LIST m_IPs;
};