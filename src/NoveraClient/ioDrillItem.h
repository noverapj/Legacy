#pragma once
#include "ioSpecialStateBase.h"

class ioDrillItem : public ioWeaponItem
{
protected:
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_NORMAL_ATTACK,
		CS_JUMP_ATTACK,			//점프공격
		CS_DIG,
		CS_JUMP_DIG,			//점프 상태 Dig
		CS_DIGGING,
		CS_DIGGING_JUMP_ATTACK,	//특별중 A키
		CS_DIGGING_ATTACK,		//특별중 d키
		CS_DIGGING_JUMP_LAND,	//특별 A키 공격끝난 후 Landing
		CS_DIGGING_END,
		CS_DIGGING_JUMP_END,
	};

	enum SyncType
	{
		SS_NORMAL_ATTACK,
		SS_JUMP_ATTACK,
		SS_DIG,
		SS_JUMP_DIG,
		SS_DIGGING,
		SS_ROTATE,
		SS_DIGGING_ATTACK,
		SS_DIGGING_ATTACK_END,
		SS_DIGGING_JUMP_ATTACK,
		SS_DIGGING_JUMP_LANDING,
		SS_DIGGING_END,
	};
protected:
	ChargeState						m_ChargeState;

	//Ani
	DWORD							m_dwMotionStartTime;
	DWORD							m_dwMotionEndTime;;
	//Normal Attack
	ioHashStringVec					m_AttackReadyAniList;
	int								m_iCurCombo;

	//Jump Attack
	AttackAttribute					m_ChargeJumpAttack;
	//-------------------------------------------
	//Dig
	AttackAttribute					m_DigAttack;
	AttackAttribute					m_DigAirAttack;

	//-------------------------------------------
	//Digging
	ioHashString					m_szDiggingLoopAni;
	float							m_fDiggingLoopAniRate;
	//Buff
	ioHashStringVec					m_szOwnerBuffList;
	//Effect
	ioHashString					m_szRedDigStartPointEffect;
	ioHashString					m_szBlueDigStartPointEffect;
	ioHashString					m_szRedDiggingEffect;
	ioHashString					m_szBlueDiggingEffect;
	ioHashString					m_szDisableAttackMark;
	DWORD							m_dwDigStartPointEffectID;
	DWORD							m_dwDigingEffectID;
	DWORD							m_dwDisableAttackMarkID;
	
	D3DXVECTOR3						m_vStartDigPosition;
	//Speed
	float							m_fDiggingSpeed;
	//Rotate
	CEncrypt<DWORD>					m_dwRotateTime;
	bool							m_bTargetRot;
	bool							m_bLeftRot;
	ioUserKeyInput::DirKeyInput		m_CurDirKey;
	//Attack
	float							m_fDiggingAttackDisableRange;
	AttackAttribute					m_DiggingAttack;
	float							m_fDiggingAttackEndJumpPower;
	//Jump Attack
	AttackAttribute					m_DiggingJumpAttack;
	float							m_fDiggingJumpPower;
	float							m_fDiggingJumpLookForce;
	//JumpLand
	ioHashString					m_szDiggingJumpLandAni;
	float							m_fDiggingJumpLandAniRate;
	//End
	ioHashString					m_szDiggingEndAni;
	float							m_fDiggingEndAniRate;
	float							m_fDiggingEndJumpPower;
	float							m_fDiggingJumpEndHeightGap;

	//Gauge
	//float							m_fChargeAttackNeedGauge;
	//float							m_fChargeAttackUseGauge;
	float							m_fDiggingJumpAttackUseGauge;
	//ioHashString					m_szGaugeIncreaseImpossibleBuff;
	//ioHashString					m_szGaugeOverHeatBuff;

	float							m_fRecoverGaugeDelay;
	float							m_fRecoverGaugeRun;

	CEncrypt<int>					m_iMaxSpecialAttCnt;
	CEncrypt<int>					m_iNeedSpecialAttCnt;
	CEncrypt<int>					m_iCurSpecialAttCnt;

	//Dummy
	ioHashString					m_szStartPointDummyName;
	DWORD							m_dwStartPointDummyIndex;
	ioHashString					m_szEndPointDummyName;
	DWORD							m_dwEndPointDummyIndex;

	ioHashString					m_szDiggingStateBuffName;
	ioHashString					m_szDigStateBuffName;


protected:
	void ClearData();

	void ChangeToNormalAttack( ioBaseChar *pOwner );
	void ChangeToDigState( ioBaseChar *pOwner );
	void ChangeToJumpDigState( ioBaseChar *pOwner );
	void ChangeToDiggingState( ioBaseChar *pOwner );
	void ChangeToDiggingAttack( ioBaseChar *pOwner );
	void ChangeToDiggingJumpAttack( ioBaseChar *pOwner );
	void ChangeToDiggingJumpLand( ioBaseChar *pOwner );
	void ChangeToDiggingEnd( ioBaseChar *pOwner );
	void ChangeToDiggingJumpEnd( ioBaseChar *pOwner );

	void ProcessCharging( ioBaseChar *pOwner );
	void ProcessNormalAttack( ioBaseChar *pOwner );
	void ProcessJumpAtackState( ioBaseChar *pOwner );
	void ProcessDigState( ioBaseChar *pOwner );
	void ProcessJumpDigState( ioBaseChar *pOwner );
	void ProcessDiggingState( ioBaseChar *pOwner );
	void ProcessDiggingAttackState( ioBaseChar *pOwner );
	void ProcessDiggingJumpAttackState( ioBaseChar *pOwner );
	void ProcessDiggingJumpLandState( ioBaseChar *pOwner );
	void ProcessDiggingEndState( ioBaseChar *pOwner );
	void ProcessRotate( ioBaseChar *pOwner );

	void AddOwnerBuffList( ioBaseChar *pOwner );
	DWORD AddEffect( const ioHashString& szEffectName, D3DXVECTOR3 vPos, ioBaseChar *pOwner );
	
	void RemoveOwnerBuffList( ioBaseChar *pOwner );
	void RemoveEffect( DWORD& rhID, ioBaseChar *pOwner );

	void CheckKeyInput( ioBaseChar *pOwner );
	bool CheckOwnerLanding( ioBaseChar *pOwner );
	bool CheckEnableDiggingAttackRange( ioBaseChar *pOwner );

	void DecreaseGauge( float fNeedGauge );

	ioDummyChar* CreateDummy( ioBaseChar* pOwner, const ioHashString& szName, const D3DXVECTOR3& vPos, const D3DXVECTOR3& vDir, const int iIndex );
	void DestroyDummy( ioBaseChar* pOwner, DWORD& iDummyIndex );

public:
	void ChangeToJumpAttackState( ioBaseChar *pOwner );

	bool IsNoDropState( ioBaseChar *pOwner );

	virtual int GetNeedBullet();
	bool CheckEnableChargeAttackGauge();
	

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();
	virtual WeaponSubType GetSubType() const;
public:
	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void SetNoneState( ioBaseChar* pOwner, int eNewState = 0 );

	virtual void CheckNormalAttack( ioBaseChar *pOwner );

	virtual bool IsCharSkipState( const ioBaseChar* pOwner );

	virtual void UpdateExtraData( ioBaseChar *pOwner );

	virtual int GetCurBullet();
	virtual int GetMaxBullet();
	virtual void SetCurBullet( int iCurBullet );

	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );

	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual void OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket );
public:
	ioDrillItem(void);
	ioDrillItem( const ioDrillItem &rhs );
	virtual ~ioDrillItem(void);
};

inline ioDrillItem * ToDrillItem ( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_DRILL_ITEM )
		return NULL;

	return dynamic_cast< ioDrillItem * >( pItem );
}