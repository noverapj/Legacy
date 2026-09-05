// cProcessor.h: interface for the cProcessor class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CPROCESSOR_H__99EAE6C9_7E44_4B41_AD66_73D633AE4EA2__INCLUDED_)
#define AFX_CPROCESSOR_H__99EAE6C9_7E44_4B41_AD66_73D633AE4EA2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "../include/IProcessor.h"


class cCommandHandler;


class cProcessor : public IProcessor
{
public:
	cProcessor();
	virtual ~cProcessor();

	void Init();
	void Destroy();

public:
	virtual void Timeout();
	virtual void Process(IIocpContext* context, const uint32 commandId, const uint8* data, const uint32 length);
	virtual void Process(const uint32 serverId, const uint32 commandId, const uint8* data, const uint32 length);

private:
	void Flush(IIocpContext* context);

private:
	cCommandHandler* m_commandHandler;
};





#endif // !defined(AFX_CPROCESSOR_H__99EAE6C9_7E44_4B41_AD66_73D633AE4EA2__INCLUDED_)
