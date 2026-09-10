#pragma once

class ioSizeItem : public ioWeaponItem
{
	enum FireState
	{
		FS_NONE,
		FS_CHARGING,
		FS_NORMAL_ATTACK,
		FS_CHARGING_ATTACK,
	};

	enum SyncType
	{
		ST_NORMAL_ATTACK,
		ST_CHARGING_ATTACK,
	};

protected:
	ioHashStringVec	m_AttackReadyAniList;	//  기본 공격 준비
	AttributeList	m_vExtendAttributeList;	//
	FireState		m_FireState;			// 상태 
	DWORD			m_dwAttackStartTime;	// 기본 공격 스타트 시간
	int				m_iCurCombo;			// 현재 콤보 값

	// 특별 공격 Gauge
	DWORD			m_dwMaxCoolTime;		//
	DWORD			m_dwNeedCoolTime;		//
	float			m_fRecoveryTime;		//
	float			m_fCurCoolTime;			//

	// D~ 크기가 작아지면서 전진하며 공격
	bool			m_bScale;
	float			m_fScaleValue;			// +, -
	D3DXVECTOR3		m_vBeforeScale;			//
	AttackAttribute	m_ExtendAttribute;		// 공격 애니 
	DWORD			m_dwFullAnimationTime;

	// 이펙트
	ioHashString	m_szScaleEffectCreateSound;
	ioHashString	m_szExtendScaleEffectName;	// 작아질때
	DWORD			m_dwExtendScaleEffectID;

public:
	virtual void	LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();

	virtual void	SetNoneState( ioBaseChar* pOwner, int eNewState = 0 );
	virtual void	SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void	CheckNormalAttack( ioBaseChar *pOwner );
	virtual void	OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual WeaponSubType GetSubType() const;

	// ioBaseChar::CheckEnableDashCheck() 함수에서 m_CharState == CS_EXTEND_DASH 일때 체크함
	virtual	bool	IsConditionalSpecialDashReserve( ioBaseChar *pOwner );
	virtual bool	IsCanFire( const ioBaseChar* pOwner, int iFireCnt = 0, bool bNormalAttack = false ) const;
	virtual void	CallItemProcess( ioBaseChar* pOwner );	// 이펙트

public:// 특별 공격
	virtual	void	UpdateExtraData( ioBaseChar *pOwner );
	virtual int		GetCurBullet();
	virtual int		GetMaxBullet();
	virtual void	SetCurBullet( int iCurBullet );
	virtual int		GetNeedBullet();
	void			InitCoolTime();
	void			MaxCoolTime();
	void			DecreaseCoolTime();

protected:	// 이펙트
	void			CreateScaleEffectChargeEffect( ioBaseChar *pOwner );

public:	// 특별 공격 D~
	void			ChangeToChargeAttackState( ioBaseChar *pOwner );

protected:
	void			OnCharging( ioBaseChar *pOwner );
	void			ChangeToNormalAttack( ioBaseChar *pOwner );

public:
	ioSizeItem();
	ioSizeItem( const ioSizeItem &rhs );
	virtual ~ioSizeItem();
};

inline ioSizeItem* ToSizeItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_SIZE_ITEM )
	{
		return NULL;
	}

	return dynamic_cast< ioSizeItem* >( pItem );
}
