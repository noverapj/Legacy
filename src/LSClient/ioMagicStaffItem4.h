#pragma once

#include "ioWeaponItem.h"

class ioEntityGroup;

class ioMagicStaffItem4 : public ioWeaponItem
{
protected:
	DWORD m_dwTimeGap;
	DWORD m_dwChargeStartTime;

	float m_fRange;
	float m_fMagicCircleRadius;
	float m_fMoveSpeed;
	float m_fHeightGap;
	CEncrypt<float> m_fSkillGaugeRate;

protected:
	DWORD m_dwDefenseTicTime;
	float m_fGaugePerAllDefense;

protected:
	ioHashString m_PreGatherAnimation;
	ioHashString m_GatherAnimation;
	ioHashString m_EndGatherAnimation;
	ioHashString m_GatherEffect;

	ioHashString m_ReadyCircle;
	ioHashString m_EnableCircle;

//////////////////////////////////////////////////////////////////////////
	//어스 메이지 추가
protected:
	float		m_fMoveSpeedRateExtend;

	ioHashString m_ExtendMoveFront;

	bool m_bFirstExtendMoveMotion;
	bool m_bExtendMotionSetted;

	float m_fExtendMoveAniRate;
	ioUserKeyInput::DirKeyInput m_ExtendStartDirKey;

	float m_fMagicCircleOffset;

	bool m_bChangeDir;

	AttackAttribute m_FinishAttribute;
	DWORD m_dwFinishAttackEndTime;

	ioHashStringVec m_vExtendBuffList;

	D3DXVECTOR3 m_vExtendAttackPos;

	bool m_bChargeAttacked;

//////////////////////////////////////////////////////////////////////////

protected:
	enum FireState
	{
		FS_NONE,
		FS_CHARGING,
		FS_GATHERING,
		FS_NORMAL_ATTACK,
		FS_EXTEND_ATTACK,
	};

	FireState m_FireState;
	CEncrypt<int>  m_iCurCombo;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();

public:
	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );
	virtual void OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );
	virtual void OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem=false );

	virtual bool IsChargeAttackState( ioBaseChar *pOwner );
	virtual void UpdateSkillGauge( ioBaseChar *pOwner, float fRate, bool bUpdate );
	virtual bool IsEnableExtraGauge( ioBaseChar *pChar );
	virtual float GetExtraGauge() { return m_fSkillGaugeRate; }

	virtual WeaponSubType GetSubType() const;

	virtual bool SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType );
	virtual void ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap );


	//////////////////////////////////////////////////////////////////////////
	virtual float GetAttackSpeedRate() const;	
	void CheckExtendMoveAniState( ioBaseChar *pOwner );
	void ChangeExtendAttack( ioBaseChar *pOwner );

public:
	float GetCircleRadius();

protected:
	void OnCharging( ioBaseChar *pOwner );
	void OnGathering( ioBaseChar *pOwner );
	void OnExtendAttack( ioBaseChar *pOwner );

protected:
	void ChangeToNormalAttack( ioBaseChar *pOwner );
	void ChangeToGathering( ioBaseChar *pOwner );

public:
	ioMagicStaffItem4();
	ioMagicStaffItem4( const ioMagicStaffItem4 &rhs );
	virtual ~ioMagicStaffItem4();
};

inline ioMagicStaffItem4* ToMagicStaffItem4( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_MAGIC_STAFF4 )
		return NULL;

	return dynamic_cast< ioMagicStaffItem4* >( pWeapon );
}
