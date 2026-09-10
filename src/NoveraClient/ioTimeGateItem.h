#pragma once
#include "ioWeaponItem.h"

class ioTimeGateItem : public ioWeaponItem
{
protected:
	enum TimeGateState
	{
		TGS_NONE,
		TGS_NORMAL_ATTACK,
		TGS_CHARGING,
		TGS_FIRST_CIRCLE_LOOP,
		TGS_SECOND_CIRCLE_LOOP,
		TGS_PORTAL_ACTION
	};

protected:
	TimeGateState	m_TimeGateState;
	DWORD	m_dwChargeStartTime;
	int		m_iCurCombo;
	float	m_fRange;
	float	m_fRadius;
	float	m_fMoveSpeed;
	float	m_fHeightGap;

	D3DXVECTOR3		m_vFirstCirclePos;
	D3DXVECTOR3		m_vSecondCirclePos;

	ioHashString	m_BranchDelayAnimation;
	ioHashString	m_FullChargeAni;

	ioHashString	m_ReadyCircle;
	ioHashString	m_EnableCircle;

	ioHashString	m_PortalStartEffect;
	DWORD			m_dwPortalStartEffectIndex;

	ioHashString	m_szStartPointDummyName;
	DWORD			m_dwStartPointDummyIndex;
	ioHashString	m_szEndPointDummyName;
	DWORD			m_dwEndPointDummyIndex;

	ioHashString	m_szCreateDummySound;

	float			m_fMaxBullet;
	float			m_fCurBullet;
	float			m_fIncreseGaugeTicDelay;
	float			m_fIncreseGaugeTicRun;
	float			m_fIncreseGaugeTicDash;
	float			m_fIncreseGaugeTicAttack;
	float			m_fIncreseGaugeTicWound;
	int				m_iChargeAttackNeedBullet;
	int				m_iJumpChargeAttackNeedBullet;

public:
	virtual	void	LoadProperty( ioINILoader& rkLoader );
	virtual	ioItem* Clone();
	virtual	WeaponSubType GetSubType() const;

public:
	virtual	void	SetNormalAttack( ioBaseChar* pOwner, int iCurCombo );
	virtual	void	CheckNormalAttack( ioBaseChar* pOwner );

	virtual	void	OnItemGaugeOK( ioBaseChar* pOwner, SP2Packet& rkPacket );
	virtual	void	OnItemGaugeCancel( ioBaseChar* pOwner, SP2Packet& rkPacket );

	virtual	int		GetCurBullet();
	virtual	int		GetMaxBullet();
	virtual	void	SetCurBullet( int iCurBullet );
	virtual	int		GetNeedBullet();
	virtual void	UpdateExtraData( ioBaseChar *pOwner );
			int		GetJumpChargeNeedBullet();

	virtual bool	IsWoundAfterUseExtendState(){ return true; }
	virtual bool	CheckWoundAfterUseExtendState( ioBaseChar* pOwner, BlowTargetType eType );

public:
	bool	IsEnoughJumpChargeAttackBullet();

	void	DecreaseBullet( int iBullet );

protected:
	void	OnCharging( ioBaseChar* pOwner );
	void	OnFirstCircleLoop( ioBaseChar* pOwner );
	void	OnSecondCircleLoop( ioBaseChar* pOwner );
	void	ChangeToNormalAttack( ioBaseChar* pOwner );
	void	ChangeToFirstCircleLoop( ioBaseChar* pOwner );
	void	ChangeToSecondCircleLoop( ioBaseChar* pOwner );
	void	ChangeToPortalAction( ioBaseChar* pOwner );
	void	ChangeToDelay( ioBaseChar* pOwner );
	void	SetPortalAction( ioBaseChar* pOwner, D3DXVECTOR3 vStart, D3DXVECTOR3 vEnd );

	DWORD	AddEffect( const ioHashString& szEffectName, D3DXVECTOR3 vPos, ioBaseChar *pOwner );
	void	RemoveEffect( DWORD& rhID, ioBaseChar *pOwner );

	ioDummyChar*	CreateDummy( ioBaseChar* pOwner, const ioHashString& szName, const D3DXVECTOR3& vPos, const D3DXVECTOR3& vDir, const int iIndex );
	void	DestroyDummy( ioBaseChar* pOwner, DWORD& iDummyIndex );

protected:
	void	ClearData();

public:
	ioTimeGateItem(void);
	ioTimeGateItem( const ioTimeGateItem &rhs );
	virtual	~ioTimeGateItem(void);
};

inline	ioTimeGateItem*	ToTimeGateItem( ioItem *pItem )
{
	ioWeaponItem*	pWeapon	= ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_TIMEGATE_ITEM )
		return	NULL;

	return	dynamic_cast<ioTimeGateItem*>( pItem );
}