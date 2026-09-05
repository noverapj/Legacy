#pragma once

class IIocpContext;


class INetwork
{
public:
	INetwork(void);
	~INetwork(void);

public:
	BOOL Flush(IIocpContext *context);

	BOOL Send(	IIocpContext *iocpContext,
				const uint16 command, 
				const uint8* buffer = NULL, 
				const uint16 length = 0,
				const BOOL encoding = TRUE);
					
	void Send(	const uint32 ID,
				const uint16 command, 
				const uint8* buffer = NULL, 
				const uint16 length = 0);
};
