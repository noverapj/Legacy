#pragma once

#include "ioObjectItem.h"

class ioWeapon;

typedef std::vector<float> AngleList;

class ioSmartBoomObjectItem : public ioObjectItem
{
public:
	enum
	{
		GENERATE_ALL_DROP_WEAPON,
		GENERATE_WEAPON_DIRECT,
	};

protected:

	bool m_bDropBomb;

	DWORD m_dwMutipleUseCnt;
	DWORD m_dwMutipleCurUseCnt;

	ioHashString m_EquipedEffect;
	D3DXVECTOR3 m_vDropMoveDir;

	EquipSlot m_OwnerItemEquipSlot;	
	AngleList m_fAngleHList;

	DWORDVec m_WeaponIndexList;

	CEncrypt<bool> m_bDisableDefense;
	CEncrypt<bool> m_bCheckWeaponOnRelease;
	CEncrypt<float> m_fJumpAttackEndNextJumpAmt;
	CEncrypt<bool> m_bDirectAttackOnKey;

	ioHashString    m_GenerateAnimation;
	CEncrypt<float> m_fGenerateAniRate;
	DWORD           m_dwMotionEndTime;
	DWORD           m_dwMotionFireTime;
	bool            m_bGenerateWeaponDirect;

	bool            m_bWounded;

	CEncrypt<DWORD> m_dwProtectTime;
	DWORD           m_dwEquipTime;

	CEncrypt<int>   m_iReduceGaugeSlot;

	bool			m_bUseTimeOver;
	DWORD			m_dwMaxUseTime;

	bool			m_bDecSkillGaugeAllTime;

	bool			m_bAllDeadCreatedWeapon;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();

	void LoadFireDirectionAppointList( ioINILoader &rkLoader );

public:
	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );
	virtual ObjectSubType GetObjectSubType() const;
	virtual bool IsOneTimeWeaponItem() const;
	virtual bool IsEnableDefense() const;
	virtual bool IsProtectState();

	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual bool SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType );
	
public:
	virtual void OnEquiped( ioBaseChar *pOwner );
	virtual void OnReleased( ioBaseChar *pOwner );

public:
	bool isMutipleUseWaeponItem() const;
	bool IsDirectAttackOnKey() const { return m_bDirectAttackOnKey; }
	DWORD GetCurUseWeaponCount() { return m_dwMutipleCurUseCnt; }
	DWORD GetMaxUseWeaponCount() { return m_dwMutipleUseCnt; }
	void ChangeDropState( const D3DXVECTOR3 &vDropDir );
	void IncreaseUseObjectCnt();

	void SetOwnerItemEquipSlot( EquipSlot iSlot );
 
	float GetAngleH( DWORD dwIndex );

public:
	virtual void OnProcessState( ioPlayStage *pStage, ioBaseChar *pOwner );

	float GetJumpAttackEndNextJumpAmt() { return m_fJumpAttackEndNextJumpAmt; }

	bool CheckWeaponOnRelease() { return m_bCheckWeaponOnRelease; }
	bool EnableReleaseItem( ioBaseChar *pOwner );

	void GenerateAllDropWeapon( ioBaseChar *pOwner );

	void ApplySmartBoomObjectItem( ioPlayStage *pStage, SP2Packet &rkPacket );

	void SetWounded( bool bWounded ) { m_bWounded = bWounded; }
	bool IsWounded() { return m_bWounded; }
	bool IsTimeOver();

protected:
	void SetGenerateWeaponDirect( ioBaseChar *pOwner );
	void ReduceSkillGauge( ioBaseChar *pOwner );

public:
	void AddWeaponIndex( DWORD dwIndex );

public:
	virtual bool CheckReleaseProcess( ioPlayStage *pStage, ioBaseChar *pOwner );
	virtual bool IsNormalAttackStop();

public:
	ioSmartBoomObjectItem();
	ioSmartBoomObjectItem( const ioSmartBoomObjectItem &rhs );
	virtual ~ioSmartBoomObjectItem();
};

inline ioSmartBoomObjectItem* ToSmartBoomObjectItem( ioItem *pItem )
{
	ioObjectItem *pObject = ToObjectItem( pItem );
	if( !pObject || pObject->GetObjectSubType() != ioObjectItem::OST_SMARTBOOM )
		return NULL;

	return dynamic_cast< ioSmartBoomObjectItem* >( pObject );
}

