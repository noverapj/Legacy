#pragma once


class cManager
{
public:
	cManager();
	~cManager();

public:
	BOOL Init(const uint32 maxSession, const uint32 flush);
	BOOL Listen(const uint16 port, const uint32 backlog);
	BOOL Start(const uint32 worker, const uint32 commander);
	BOOL Timer();
};
