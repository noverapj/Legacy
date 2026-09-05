#pragma once

#include "ioWeaponItem.h"
#include "ioAttackAttribute.h"
#include "ioSpecialStateBase.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioThunderBirdItem : public ioWeaponItem
{
public:
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_GATHERING,
		CS_ATTACK_FIRE,
		CS_EQUIP_OBJECT,
		CS_BOMB_MOTION,
		CS_END,
	};

	enum SyncType
	{
		ST_ATTACK,
		ST_GATHER,
		ST_RESET_GATHER,
		ST_BOMB_MOTION,
		ST_RESET_ATTACK_MOVE,
	};

protected:
	ChargeState		m_ChargeState;

	DWORD			m_dwAttackStartTime;
	int				m_iCurCombo;		
	float			m_fNeedExtraGauge;
		
	DWORD			m_dwObjectItem;
	ioHashString	m_ObjectEquipAnimation;
	float			m_fObjectEquipAnimationRate;

	//개더링 관련
	ioHashStringVec	m_PreGatherAnimationList;
	ioHashStringVec	m_PreGatherAnimationListW;
	ioHashString	m_GatherAnimation;
	ioHashString	m_GatherAnimationW;
	ioHashString	m_GatherEffect;
	UniqueObjID		m_dwGatherEffectID;

	//D꾹 이동관련
	bool			m_bChangeDir;
	bool			m_bExtendMotionSetted;
	bool			m_bFirstExtendMoveMotion;
	float			m_fExtendMoveAniRate;
	float			m_fMoveSpeedRateExtend;
	ioHashString	m_ExtendMoveFront;
	ioHashString	m_ExtendMoveFrontW;

	bool m_bEquipObjectItem;

	ioHashString m_szSDMotion;
	float m_fSDMotionRate;
	DWORD m_dwSDMotionEndTime;

public:
	void Init();

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();
	virtual WeaponSubType GetSubType() const;

public:
	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );
	
	virtual bool SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType );
	virtual void ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap );
		
	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );

	virtual void SetUseExtraGauge( float fAmt );
	virtual void UpdateExtraData( ioBaseChar *pOwner );		

	virtual float GetAttackSpeedRate() const;

	virtual void OnReleased( ioBaseChar *pOwner );
	virtual void ReleaseObjectState( ioBaseChar *pOwner, ioObjectItem* pReleasedObject );

	virtual bool IsChargeAttackState( ioBaseChar *pOwner );
	virtual bool IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck );
	virtual bool ProcessCancel( ioBaseChar *pOwner );

	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );

public:	
	virtual int GetMaxBullet();	
	virtual int GetCurBullet();
	virtual void SetCurBullet( int iCurBullet );
	virtual int GetNeedBullet();

protected:
	void ResetGathering( ioBaseChar* pOwner );

	void OnCharging( ioBaseChar *pOwner );
	void OnGathering( ioBaseChar *pOwner );

	void ChangeToGatheringState( ioBaseChar *pOwner );
	void ChangeToAttackFire( ioBaseChar *pOwner );
	
	void CheckExtendMoveAniState( ioBaseChar *pOwner );	
		
public:
	bool IsEquipObject( ioBaseChar *pOwner );
	void EquipObjectItem( ioBaseChar *pOwner );
	void ReleaseObject( ioBaseChar *pOwner );
	virtual void SetNoneState( ioBaseChar* pOwner, int eNewState = 0 );
		
public:
	void FillThunderBirdInfo( ioBaseChar *pOwner, SP2Packet &rkPacket );
	void SetThunderBirdInfo( ioBaseChar *pOwner, SP2Packet &rkPacket  );
	bool CheckReleaseObjectItem( ioBaseChar *pOwner );

public:
	void ChangeToBombState( ioBaseChar *pOwner, bool bSendPacket );
	void ProcessBombState( ioBaseChar *pOwner );
	
public:
	ioThunderBirdItem();
	ioThunderBirdItem( const ioThunderBirdItem &rhs );
	virtual ~ioThunderBirdItem();
};

inline ioThunderBirdItem* ToThunderBirdItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_THUNDER_BIRD_ITEM )
		return NULL;

	return dynamic_cast< ioThunderBirdItem* >( pItem );
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
class ioThunderBirdBombSpecialState : public ioSpecialStateBase
{
public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_THUNDER_BIRD_BOMB_STATE"; }
	virtual const CharState GetState(){ return CS_THUNDER_BIRD_BOMB_STATE; }
	virtual bool IsEnableBlowWounded( ioBaseChar *pOwner ){ return false; }	

public:
	virtual void CheckSpecialState( ioBaseChar* pOwner );

public:
	ioThunderBirdBombSpecialState();
	virtual ~ioThunderBirdBombSpecialState();
};

inline ioThunderBirdBombSpecialState* ToThunderBirdBombSpecialState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_THUNDER_BIRD_BOMB_STATE )
		return NULL;

	return dynamic_cast< ioThunderBirdBombSpecialState* >( pState );
}