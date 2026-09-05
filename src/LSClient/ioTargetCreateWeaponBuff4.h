#pragma once
#include "iobuff.h"
class ioTargetCreateWeaponBuff4 :
	public ioBuff
{
private:
	enum TargetBuffState
	{
		TBS_NONE,
		TBS_DELAY,
		TBS_CREATE_TIC_WEAPON,
		TBS_WEAPON_FIRE,
	};

private:
	TargetBuffState	m_BuffState;
	ioHashStringVec	m_vSkipBuffList;
	float			m_fTargetRange;
	float			m_fTargetAngle;
	TargetWoundType m_TargetWoundType;
	bool			m_CheckWoundedTarget;
	int				m_iMaxTargetCount;
	
	int				m_iMaxAttackCount;
	int				m_iCurAttackCount;

	DWORD			m_dwTargetAttackTicTime;
	DWORD			m_dwTargetAttackStartTime;
	WeaponInfo		m_TargetAttackWeapon;
	D3DXVECTOR3		m_TargetAttackOffset;

	bool			m_bDisableDownState;
	bool			m_bDisableDropState;

	DWORD			m_dwDelayEndTime;
	DWORD			m_dwDelayDuration;

	ioHashStringVec m_vTargetNameList;
public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();
public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();
	virtual void ApplyExtraBuffInfo( SP2Packet &rkPacket );
private:
	void Init();

	bool CheckOwnerState();
	bool CheckCreatorState();

	void SetDelayState();
	void SetCreateTicWeaponState();
	void CreateTargetAttackWeapon();

	void FindTargetList();
	bool CheckTargetState( ioBaseChar *pChar, ioBaseChar *pTarget );
	bool CheckSkipBuff( ioBaseChar *pTarget );
	void CreateWeapon( ioBaseChar *pChar, ioBaseChar *pCreator, ioHashString szTargetName );
public:
	ioTargetCreateWeaponBuff4(void);
	ioTargetCreateWeaponBuff4( const ioTargetCreateWeaponBuff4 &rhs );
	~ioTargetCreateWeaponBuff4(void);
};

inline ioTargetCreateWeaponBuff4* ToTargetCreateWeaponBuff4( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_TARGET_WEAPON4 )
		return NULL;

	return dynamic_cast< ioTargetCreateWeaponBuff4* >( pBuff );
}