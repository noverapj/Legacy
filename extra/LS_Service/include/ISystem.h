#pragma once


class IProcessor;


class ISystem
{
public:
	ISystem(void);
	~ISystem(void);

	void Init();
	void Destroy();

public:
	BOOL Startup(
		const uint32 maxSession, 
		const uint32 maxPacket, 
		const uint32 maxQueue, 
		const uint32 encoding=0) const;
	void Cleanup() const;

	BOOL Start(const int32 workerCount = 4, const int32 commandCount = 1, const int32 connectorCount = 0) const;
	void Stop() const;

	void SetProcessor(IProcessor* processor) const;
	void SetEvent(const uint32 connectId, const uint32 disconnectId) const;

	BOOL Listen(const uint16 port, const uint32 backlog) const;
	BOOL Connect(const uint32 serverId, const TCHAR* IP, const uint16 port) const;
	
	BOOL Call(const uint32 command, const uint8* buffer, const uint32 length) const;
};
