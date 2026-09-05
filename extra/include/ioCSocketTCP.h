#pragma once

#include "ioCSocket.h"

class ioCSocketTCP : public ioCSocket 
{
public:
	ioCSocketTCP();
	virtual ~ioCSocketTCP();

public:
	void Init();
	void Destroy();

public:
	BOOL Create();

	BOOL Connect(const char *IP, int port);
	void Disconnect();

	BOOL Send(const char *buffer, int length);
	BOOL Receive(char *buffer, int& length);

	BOOL SetOptions();

public:
	BOOL CreateThread();
	void DestroyThread();
	bool m_bState;

};

