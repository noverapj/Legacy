#pragma once

#include "cCommand.h"
#include "cBuffer.h"
#include "cSession.h"
#include "cSerialize.h"
#include "Utility.h"
#include "PerfMon.h"
#include "../common/internal.h"
#include <vector>

class CPing : public cCommand
{
protected:
	virtual BOOL Verify(IIocpContext* pContext, uint8* buffer, const uint32 length)
	{
		if(length != 0)
			return FALSE;

		m_packet = reinterpret_cast<csPing*>(buffer);
		return TRUE;
	}	
	virtual void Self()
	{
		Debug(_T("C_PING\n"));
	}
	virtual void Run(IIocpContext* context);

protected:
	csPing* m_packet;

};

class CServices : public cCommand, public cSerialize
{
protected:
	virtual BOOL Verify(IIocpContext* pContext, uint8* buffer, const uint32 length)
	{
		m_services.clear();
		m_files.clear();
		m_scripts.clear();
		return TRUE;
	}	
	virtual void Self()
	{
		Debug(_T("C_SERVICES\n"));
	}
	virtual void Run(IIocpContext* context);

protected:
	BOOL Ready(IIocpContext* context);
	void Begin(IIocpContext* context);

	BOOL GetRunning(const TCHAR* service, BOOL& running, BOOL& console);

protected:
	cSession* m_session;

	STRINGS m_services, m_files, m_scripts;
};


class CTransfer : public cCommand
{
protected:
	virtual BOOL Verify(IIocpContext* pContext, uint8* buffer, const uint32 length)
	{
		if(length < sizeof(csTransfer))
			return FALSE;

		m_packet = reinterpret_cast<csTransfer*>(buffer);
		m_buffer = NULL;
		m_length = 0;
		m_temp.Erase();

		switch(m_packet->type)
		{
		case TRANSFER_BEGIN :
			if(length < sizeof(csTransferBegin))
				return FALSE;
			m_begin	= reinterpret_cast<csTransferBegin*>(buffer);
			break;

		case TRANSFER_DO :
			{
				if(length < sizeof(csTransferDo))
					return FALSE;
				m_do		= reinterpret_cast<csTransferDo*>(buffer);
				m_buffer	= (buffer + sizeof(csTransferDo));
				if(length != (m_do->length + sizeof(csTransferDo)))
					return FALSE;

				m_length	= m_do->length;
			}
			break;

		case TRANSFER_END :
			if(length < sizeof(csTransferEnd))
				return FALSE;
			m_end = reinterpret_cast<csTransferEnd*>(buffer);
			break;
		}
		return TRUE;
	}	
	virtual void Self()
	{
		Debug(_T("C_TRANSFER\n"));
	}
	virtual void Run(IIocpContext* context);

protected:
	BOOL Ready(IIocpContext* context);
	void Begin(IIocpContext* context);

protected:
	BOOL TransferBegin();
	BOOL TransferDo();
	BOOL TransferEnd();

	BOOL UpdateFile(const TCHAR* fileName, const uint32 fileLength, const BYTE* source);
	BOOL CreateBuffer(const TCHAR* fileName, const uint32 length);

protected:
	cSession* m_session;

	csTransfer* m_packet;
	csTransferBegin* m_begin;
	csTransferDo* m_do;
	csTransferEnd* m_end;
	uint8* m_buffer;
	uint32 m_length;
	cBuffer m_temp;
};


class CRemove : public cCommand
{
protected:
	virtual BOOL Verify(IIocpContext* pContext, uint8* buffer, const uint32 length)
	{
		if(length < sizeof(csRemove))
			return FALSE;

		m_packet = reinterpret_cast<csRemove*>(buffer);
		return TRUE;
	}	
	virtual void Self()
	{
		Debug(_T("C_REMOVE\n"));
	}
	virtual void Run(IIocpContext* context);

protected:
	BOOL Ready(IIocpContext* context);
	void Begin(IIocpContext* context);

protected:
	BOOL RemoveFolder();

protected:
	cSession* m_session;

	csRemove* m_packet;
};

class CInstall : public cCommand
{
protected:
	virtual BOOL Verify(IIocpContext* pContext, uint8* buffer, const uint32 length)
	{
		if(length < sizeof(Install))
			return FALSE;

		m_packet = reinterpret_cast<Install*>(buffer);
		if(m_packet->install)
		{
			if(length < sizeof(csInstall))
				return FALSE;

			m_install = reinterpret_cast<csInstall*>(buffer);
		}
		else
		{
			if(length < sizeof(csUninstall))
				return FALSE;

			m_uninstall = reinterpret_cast<csUninstall*>(buffer);
		}
		return TRUE;
	}	
	virtual void Self()
	{
		Debug(_T("C_INSTALL\n"));
	}
	virtual void Run(IIocpContext* context);

protected:
	BOOL Ready(IIocpContext* context);
	void Begin(IIocpContext* context);

protected:
	BOOL DoInstall(const TCHAR* service, const TCHAR* file, const TCHAR* script);
	BOOL DoUninstall(const TCHAR* service);
	void Remove(const BOOL b);

	BOOL CreateRegistry(const TCHAR* service, const TCHAR* file, const TCHAR* script);
	BOOL DeleteRegistry(const TCHAR* service);

protected:
	cSession* m_session;

	Install* m_packet;
	csInstall* m_install;
	csUninstall* m_uninstall;
};


class CPlay : public cCommand
{
protected:
	virtual BOOL Verify(IIocpContext* pContext, uint8* buffer, const uint32 length)
	{
		if(length < sizeof(Play))
			return FALSE;

		m_packet = reinterpret_cast<Play*>(buffer);
		if(m_packet->play)
		{
			if(length < sizeof(csPlay))
				return FALSE;

			m_play = reinterpret_cast<csPlay*>(buffer);
		}
		else
		{
			if(length < sizeof(csStop))
				return FALSE;

			m_stop = reinterpret_cast<csStop*>(buffer);
		}
		return TRUE;
	}	
	virtual void Self()
	{
		Debug(_T("C_PLAY\n"));
	}
	virtual void Run(IIocpContext* context);

protected:
	BOOL Ready(IIocpContext* context);
	void Begin(IIocpContext* context);

protected:
	BOOL DoPlay(const TCHAR* service, const BOOL isDemon, DWORD &PID);
	BOOL DoStop(const TCHAR* service, const BOOL isDemon);

protected:
	cSession* m_session;

	Play*	m_packet;
	csPlay* m_play;
	csStop* m_stop;

};


class CBackup : public cCommand
{
protected:
	virtual BOOL Verify(IIocpContext* pContext, uint8* buffer, const uint32 length)
	{
		if(length < sizeof(csBackup))
			return FALSE;

		m_packet = reinterpret_cast<csBackup*>(buffer);
		return TRUE;
	}	
	virtual void Self()
	{
		Debug(_T("C_BACKUP\n"));
	}
	virtual void Run(IIocpContext* context);

protected:
	BOOL Ready(IIocpContext* context);
	void Begin(IIocpContext* context);

protected:
	BOOL DoBackup(const TCHAR* source, const TCHAR* target);

protected:
	cSession* m_session;

	csBackup* m_packet;
};

class CReboot : public cCommand
{
protected:
	virtual BOOL Verify(IIocpContext* pContext, uint8* buffer, const uint32 length)
	{
		if(length < sizeof(csReboot))
			return FALSE;

		m_packet = reinterpret_cast<csReboot*>(buffer);
		return TRUE;
	}	
	virtual void Self()
	{
		Debug(_T("C_REBOOT\n"));
	}
	virtual void Run(IIocpContext* context);

protected:
	BOOL Ready(IIocpContext* context);
	void Begin(IIocpContext* context);

protected:
	BOOL DoReboot(const uint32 option);

protected:
	cSession* m_session;

	csReboot* m_packet;
};

class CResource : public cCommand, public cSerialize
{
protected:
	virtual BOOL Verify(IIocpContext* pContext, uint8* buffer, const uint32 length)
	{
		return TRUE;
	}	
	virtual void Self()
	{
		Debug(_T("C_RESOURCE\n"));
	}
	virtual void Run(IIocpContext* context);

protected:
	BOOL Ready(IIocpContext* context);
	void Begin(IIocpContext* context);

protected:
	cSession* m_session;

	PerfMon m_perfomance;
};
