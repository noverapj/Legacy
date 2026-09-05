#pragma once

#include "ioModeItem.h"
#include "GUI/CenterHelpWnd.h"

class SP2Packet;
class ioMapEffect;
class ioPlayStage;

class ioShuffleBuffItem : public ioModeItem
{
protected:
	enum ItemState
	{
		IS_WAIT,		
		IS_CREATE,
		IS_DELAY,
	};	

public:
	enum BuffTargetType
	{
		BTT_NONE,
		BTT_OWENR,
		BTT_ENEMY,
		BTT_TEAM,
	};

	struct CallBuffInfo
	{
		ioHashStringVec m_szBuffNameVec;
		BuffTargetType  m_TargetType;
		float           m_fRange;

		ioHashString    m_szCallBuffIcon;
		ShuffleHelpMent m_CallBuffMentInfo;

		CallBuffInfo()
		{
			m_TargetType = BTT_NONE;
			m_fRange = 0.0f;
		}
	};
	typedef std::vector<CallBuffInfo> vCallBuffInfo;

protected:
	ItemState m_State;
	vCallBuffInfo m_vCallBuffInfo;

protected:
	DWORD m_dwWaitTime;

	DWORD m_dwCreateTime;	
	DWORD m_dwCreateStartTime;

	ioHashString m_szCreateEffect;
	UniqueObjID m_CreateEffectID;

protected:
	ioHashString m_szAddBuffBoxIcon;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioModeItem *Clone();

public:
	virtual void SetItemValue( DWORD dwIndex, float fXPos, float fZPos, DWORD dwActivateTime );

public:
	void SetCreateState();

public:
	virtual void Process();

	void ProcessWait();
	void ProcessCreate();
	void ProcessDelay();

public:
	virtual bool IsCollision( ioBaseChar *pChar );

public:
	bool IsAvailableRange( ioBaseChar* pTarget, float fRange );
	void AddNewBuff( const CallBuffInfo& rkInfo, ioBaseChar* pOwner, ioBaseChar* pTarget );

public:
	const ioShuffleBuffItem::CallBuffInfo* OnBuffCall( ioBaseChar* pOwner, const BaseCharList &rkTargetList, DWORD dwCallBuff );

public:
	ioShuffleBuffItem();
	ioShuffleBuffItem( const ioShuffleBuffItem &rhs );
	virtual ~ioShuffleBuffItem();
};


inline ioShuffleBuffItem* ToShuffleBuffItem( ioModeItem *pItem )
{
	if( !pItem || pItem->GetTypeID() != MIT_BUFF )
		return NULL;

	return dynamic_cast< ioShuffleBuffItem* >( pItem );
}
