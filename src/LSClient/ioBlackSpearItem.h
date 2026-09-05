#pragma once
#include "ioweaponitem.h"
class ioBlackSpearItem : public ioWeaponItem
{
private:
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_GATHERING,
		CS_NORMAL_ATTACK,
		CS_GATHER_ATTACK,
		CS_GATHER_ATTACK_END,
	};

	enum SyncType
	{
		ST_NONE,
		ST_NORMAL_ATTACK,
		ST_GATHERING,
		ST_GATHERING_MAX,
		ST_GATHERING_ATTACK,
		ST_GATHERING_ATTACK_COMBO,
		ST_GATHERING_ATTACK_END,
		ST_ROTATE,
		ST_CHECK_BRAND_BUFF,
	};

	typedef struct _tagGatherAttack
	{
		DWORD dwMinTime;
		DWORD dwMaxTime;

		AttributeList GatherLinearCombo;

		_tagGatherAttack()
		{
			dwMinTime = 0;
			dwMaxTime = 0;
		}
	}GatherAttack;

	typedef std::vector<GatherAttack> GatherAttackList;
private:
	//State
	ChargeState						m_ChargeState;
	ioUserKeyInput::DirKeyInput		m_CurDirKey;
	bool							m_bTargetRot;
	bool							m_bLeftRot;

	//Ani
	DWORD							m_dwMotionStartTime;
	DWORD							m_dwMotionEndTime;;

	//Normal Attack
	DWORD							m_dwAttackStartTime;
	ioHashStringVec					m_AttackReadyAniList;
	int								m_iCurCombo;

	//Gather
	DWORD							m_dwMaxEffectGatherTime;
	AttributeList					m_CurGatherAttackList;
	int								m_iGatherAttackIndex;
	DWORD							m_dwGatheringStartTime;
	ioHashString					m_GatherAnimation;
	float							m_fGatherAniRate;
	ioHashString					m_GatherMove;
	GatherAttackList				m_GatherAttackList;
	float							m_fGatherRunSpeed;
	DWORD							m_dwGatherRotateTime;
	bool							m_bSetGatheringMoveAni;
	ioHashString					m_GatheringEffect;
	ioHashString					m_GatheringMaxEffect;
	UniqueObjID						m_GatheringEffectID;
	UniqueObjID						m_GatheringMaxEffectID;
	ioHashString					m_GatheringMaxSound;
	ioHashString					m_GatherMoveEndAni;
	float							m_fGatherMoveEndAniRate;

	//Gaguge
	CEncrypt<DWORD>					m_dwMaxCoolTime;
	CEncrypt<float>					m_fCurCoolTime;
	CEncrypt<float>					m_fRecoveryTime;
	CEncrypt<float>					m_fReduceTime;

	ioHashStringVec					m_vCheckBuffList;
	ioHashStringVec					m_vCheckAddBuffList;
	DWORD							m_dwCheckBuffGapTime;
	DWORD							m_dwCheckBuffStartTime;

private:
	void ClearData();

	//Charge
	void ProcessCharging( ioBaseChar *pOwner );
	
	//Normal
	void ChangeToNormalAttack( ioBaseChar *pOwner );
	void ProcessNormalAttack( ioBaseChar *pOwner );

	//Gather
	void ChangeToGathering( ioBaseChar *pOwner );
	void ChangeToGatherAttackState( ioBaseChar *pOwner );
	void ChangeToGatherAttackEnd( ioBaseChar *pOwner );

	void ProcessGathering( ioBaseChar *pOwner );
	void ProcessGatherAttack( ioBaseChar *pOwner );
	void ProcessGatherAttackEnd( ioBaseChar *pOwner );
	void CallGatherAttack( ioBaseChar *pOwner, int iIndex );
	void CallGatherAttackCombo( ioBaseChar *pOwner, bool bSend );
	void CheckKeyInput( ioBaseChar *pOwner );
	void ProcessRotate( ioBaseChar *pOwner );
	void CheckGatherMoveState( ioBaseChar *pOwner );

	//Gaguge
	void DecreaseCoolTime( int iNeedBullet );

	//Effect
	void DeleteOwnerAttachEffect( ioBaseChar *pOwner, UniqueObjID& EffectID, bool bGrace = true );

public:
	//Check Buff
	void CheckBrandBuff( ioBaseChar *pOwner );
	bool AddNewBrandAttackBuff( ioBaseChar *pOwner );
public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();
	virtual WeaponSubType GetSubType() const;
public:
	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );
	virtual void OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem=false );
public:
	virtual void UpdateExtraData( ioBaseChar *pOwner );

	virtual int GetCurBullet();
	virtual int GetMaxBullet();
	virtual void SetCurBullet( int iCurBullet );
public:
	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );

public:
	ioBlackSpearItem(void);
	ioBlackSpearItem( const ioBlackSpearItem &rhs );
	virtual ~ioBlackSpearItem();
};

inline ioBlackSpearItem* ToBlackSpearItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_BLACK_SPEAR_ITEM )
		return NULL;

	return dynamic_cast< ioBlackSpearItem* >( pItem );
}