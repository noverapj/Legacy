#pragma once

//----------------------------------------------------------------------------------------------------------------------------------------------------------
class ioBlockEventParameter;
class ioBlockEventListener
{
public:
	enum EventType
	{
		ET_NONE							= 0,
		ET_BLOCK_COLL_NONE_VIEW			= 1,
		ET_BLOCK_COLL_AABB_VIEW			= 2,
		ET_BLOCK_COLL_SPHERE_VIEW		= 3,
		ET_BLOCK_INTERPLAY_COLL_VIEW	= 4,
		ET_BLOCK_CREATE					= 5,
		ET_SETUP_CHANGE_FAIL			= 6,
	};

public:
	virtual void ReciveEvent( EventType eType, ioBlockEventParameter* param ){};

public:
	ioBlockEventListener();
	virtual ~ioBlockEventListener();
};

typedef std::vector<ioBlockEventListener*> ioBlockEventListenerList;

//----------------------------------------------------------------------------------------------------------------------------------------------------------

class ioBlockEventAgency : public Singleton< ioBlockEventAgency >
{
public:
	struct ReseveEvent
	{
		int						eType;
		ioBlockEventParameter*	pParam;

		ReseveEvent()
		{
			eType	= 0;
			pParam	= NULL;
		}
	};
	typedef std::vector<ReseveEvent> ReseveEventList;

protected:
	ioBlockEventListenerList	m_BlockEventListenerList;
	ReseveEventList				m_ReseveEventList;

public:
	static ioBlockEventAgency& GetSingleton();

public:
	void RegisterEventListener( ioBlockEventListener* pListener );
	void UnRegisterEventListener( ioBlockEventListener* pListener );
	
public:
	bool IsEvent( ioBlockEventListener::EventType eType, ioBlockEventParameter* param );
	void SendEvent( ioBlockEventListener::EventType eType, ioBlockEventParameter* param );

public:
	void ProcessEvent();
	void BroadCastEvent( ioBlockEventListener::EventType eType, ioBlockEventParameter* param );

public:
	ioBlockEventAgency();
	virtual ~ioBlockEventAgency();

};

#define g_BlockEventAgency ioBlockEventAgency::GetSingleton()