// cIocpContext.h: interface for the cIocpContext class.
//
//////////////////////////////////////////////////////////////////////

#pragma once
 
#include "cSocket.h"
#include "cStorage.h"
#include "../include/IIocpContext.h"

class cIocpContext : public IIocpContext, public cSocket, public cStorage
{
	friend class cWorkerThread;

public:
	cIocpContext();
	~cIocpContext();

	void Init();
	void Destroy();

public:
	virtual BOOL IsActive()						{ return m_active;		}
	virtual void SetActive( BOOL active )		{ m_active = active;	}

	virtual void Disconnect();
	virtual	uint32	GetEntity()					{ return m_entity;		}
	virtual uint32  GetIP()						{ return m_IP;			}

public:
	void Reset();
	
	void SetEntity( uint32 entity )				{ m_entity = entity;	}
	void SetPortID( uint32 portId )				{ m_portId = portId;		}
	void SetSequence( uint8 sequence )			{ m_sequence = sequence;}

	uint32	GetPortID()							{ return m_portId;		}
	//uint8	GetSequence()						{ return ((m_sequence+1) > 0xff) ? 1 : (m_sequence+1);	}
	uint8	GetSequence()						{ return m_sequence;	}
	void PopSequence()
	{
		if(m_sequence >= 0xff) m_sequence = 0;
		++m_sequence;	
	}

	void Associate()							{ m_associated  = TRUE;	}
	BOOL IsAssociated()							{ return m_associated;	}

private:
	BOOL m_active;
	BOOL m_associated;

	uint32	m_entity;
	uint32	m_portId;
	uint8	m_sequence;
	
	static uint32 ENTITY;
};

