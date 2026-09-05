#pragma once

#include <string>

class ioHTTP
{
public:
	ioHTTP(void);
	~ioHTTP(void);

	void Init();
	void Destroy();

public:
	BOOL GetResultData(const CHAR* URL, const char* POST, char* result, const int length);
	BOOL GetResultData(const CHAR* URL, const char* POST, char* HEADER, char* result, const int length);

};