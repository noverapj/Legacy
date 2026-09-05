#pragma once

class ioBaseChar;
class ioINILoader;

class ioTargetControlBuff : public ioBuff
{
public:
	enum
	{
		RESERVE_TIME = 250,
	};

protected:
	int m_iCurUseCount;
	bool m_bDropZoneCheck;
	float m_fControlOutRange;
	float m_fChangeMoveSpeed;
	bool m_bDisableKeyInputCheck;
	bool m_bControlSkill;

	DWORD m_dwDropZoneCheckTime;
	DWORD m_dwDropZoneStartTime;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual void ProcessBuff( float fTimePerSec );	
	virtual void EndBuff();

	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );
	float GetCurChangeMoveSpeed() const		{ return m_fChangeMoveSpeed; }

public:
	void RemoveOldTargetControlBuff( ioBaseChar* pOwner );		
	void CheckDropZone();
	void CheckOutRange();
	inline void ResetCurrUseCount(){ m_iCurUseCount = 0; }

public:
	void ProcessRun( float fTimePerSec );
	void ProcessStartCreatorSync();

public:
	ioTargetControlBuff();
	ioTargetControlBuff( const ioTargetControlBuff &rhs );
	virtual ~ioTargetControlBuff();
};

inline ioTargetControlBuff* ToTargetControlBuff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_TARGET_CONTROL )
		return NULL;

	return dynamic_cast< ioTargetControlBuff* > ( pBuff );
}


