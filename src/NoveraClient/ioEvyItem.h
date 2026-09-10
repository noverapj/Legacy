#pragma once

#include "ioWeaponItem.h"
#include "ioUserKeyInput.h"
#include "ioAttackAttribute.h"
#include "ioSpecialStateBase.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioEvyItem : public ioWeaponItem
{
public:
	enum ChargeState
	{
		ES_NONE,
		ES_CHARGING,
		ES_GATHERING,
		ES_SPECIAL_ATTACK,
		ES_SEECIAL_ATTCK_JUMP,
		ES_NORMAL_ATTACK,
		ES_END,
	};

	enum BlankState
	{
		BS_NONE,
		BS_BOMB_MOTION,
		BS_DASH,
		BS_DASH_ENDING,
		BS_DASH_END_JUMP,
		BS_STARTMOTION,
		BS_ENDMOTION,
		BS_END_JUMP,
		BS_END,
	};

	enum SyncSubType
	{
		SST_BLINK_STATE,
		SST_ROTATE,
		SST_EFFECT,
		SST_MARKER,
	};

protected:
	ioHashStringVec m_AttackReadyAniList;

	ChargeState m_ChargeState;

	DWORD m_dwAttackStartTime;
	DWORD m_dwGatheringStartTime;
	int m_iCurCombo;
	int m_nAttackCnt;

	//ioHashString m_DashAnimation;
	//float		 m_fDashAnimationRate;

	//float			m_fDashSpeed;
	float			m_fDashMovedLenth;
	//float			m_fDashRange;
	D3DXVECTOR3		m_vDashMoveDir;

	//ioHashString	m_DashEndAnimation;
	//float			m_fDashEndAniRate;

	bool			m_bKeyReserved;
	bool			m_bAttackKeyReserved;
	bool			m_bDefenseKeyReserved;
	bool			m_bJumpKeyReserved;

	void			ClearReserveKey();

	AttributeList	m_ChargeAttackList;

	bool			m_bThirdChargeAttack;//3Å¸ÈÄ d²Ú¶¼±â½Ã ÆøÆÄ¿ë ÇÃ·¹±×

	bool			m_bSDSkillUsed;

	//////////////////////////////////////////////////////
	ioHashString	m_stSD_Blow_Motion;
	float			m_fSD_BlowMotionRate;

	bool			m_bEndAttackJump;

protected:
	BlankState		m_BlankState;

	ioUserKeyInput::DirKeyInput m_CurDirKey;

	EvyBlinkInfo m_NormalBlinkInfo;
	EvyBlinkInfo m_CurBlinkInfo;

	D3DXVECTOR3 m_vFlyMoveDir;
	CEncrypt<float> m_fFlyMoveSpeed;

	std::vector<float>			m_vfGatherAniRate;
	std::vector<ioHashString>	m_vGatherAnimation;

	DWORD			m_dwMotionEndTime;

protected:

	CEncrypt<float> m_fMaxGauge;
	CEncrypt<float> m_fNeedGauge;

	CEncrypt<float> m_fIncreaseGauge;
	CEncrypt<float> m_fDecreaseGauge;

	CEncrypt<float> m_fCurGauge;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();

public:
	virtual void OnEquiped( ioBaseChar *pOwner );
	virtual void OnReleased( ioBaseChar *pOwner );

	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );
	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );

	virtual void OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem=false );
	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual void OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual bool IsChargeAttackState( ioBaseChar *pOwner );

	/*virtual void GetAutoTargetValue( ioBaseChar *pOwner,
		float &fRange, float &fMinAngle, float &fMaxAngle,
		AutoTargetType eType );*/

public:
	virtual WeaponSubType GetSubType() const;

protected:
	void LoadEvyInfo( ioINILoader &rkLoader );

protected:
	void OnCharging( ioBaseChar *pOwner );
	void OnGathering( ioBaseChar *pOwner );
	void OnDash( ioBaseChar *pOwner );
	void OnDashEnding( ioBaseChar *pOwner );
	void OnBlinkEnding( ioBaseChar *pOwner );

	void ChangeToNormalAttack( ioBaseChar *pOwner );
	void ChangeToChargeAttack( ioBaseChar *pOwner, int iCurCombo );

	void ChangeToGathering( ioBaseChar *pOwner );
	void ChangeToSpecialBlow( ioBaseChar *pOwner );
	void ChangeToDashState( ioBaseChar *pOwner );
	void ChangeToDashEnding( ioBaseChar *pOwner );
	void ChangeToEnd( ioBaseChar *pOwner );

	void ChangeToBlinkStartMotion( ioBaseChar *pOwner );
	void ChangeToBlinkEndMotion( ioBaseChar *pOwner );

	void ChangeDefenseState( ioBaseChar *pOwner );
	void ChangeDashEndJump( ioBaseChar *pOwner );

	bool IsChargeAutoTarget( ioBaseChar *pOwner );

	void KeyReserve( ioBaseChar *pOwner );

	void CheckDashMoveDir( ioBaseChar *pOwner );

	void CalcBlankPos( ioBaseChar *pOwner );
public:
	void SetEvyBlinkState( ioBaseChar *pOwenr, const EvyBlinkInfo &rkInfo, bool bSendNet );

	void ProcessBlinkState( ioBaseChar *pOwner );

	inline const D3DXVECTOR3& GetGoomihoFlyMoveDir() const { return m_vFlyMoveDir; }
	inline const float GetGoomihoFlyMoveSpeed() const { return m_fFlyMoveSpeed; }

	void SetSDSet( bool bUsed );

protected:
	bool IsEnableTargetState( ioBaseChar *pTarget );

protected:
	DWORD m_dwFireStartTime;
	DWORD m_dwFireEndTime;
	DWORD m_dwFireDelayTime;

	//Gauge
public:
	virtual int GetMaxBullet();
	virtual int GetNeedBullet();
	virtual int GetCurBullet();
	virtual void SetCurBullet( int iCurBullet );

	virtual void InitExtraGauge();
	virtual void MaxExtraGauge();

	virtual float GetMaxExtraGauge();
	virtual float GetCurExtraGauge();
	virtual void SetCurExtraGauge( float fGauge );

	void CheckDecreaseGauge();

	bool IsEnableGauge();
	void ClearSpecialState( ioBaseChar *pOwner );

	virtual void UpdateExtraData( ioBaseChar *pOwner );

public:
	bool IsBallTarget();

public:
	ioEvyItem();
	ioEvyItem( const ioEvyItem &rhs );
	virtual ~ioEvyItem();
};

inline ioEvyItem* ToEvyItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_EVY_ITEM )
		return NULL;

	return dynamic_cast< ioEvyItem* >( pItem );
}

//-----------------------------------------------------------------------------------------------------------

class ioEvyBlinkSpecialState : public ioSpecialStateBase
{
public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_EVY_BLINK"; }
	virtual const CharState GetState(){ return CS_EVY_BLINK; }	
	virtual bool IsEnableBlowWounded( ioBaseChar *pOwner ){ return false; }

public:
	virtual void CheckSpecialState( ioBaseChar* pOwner );
	virtual void CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight );
	
public:
	ioEvyBlinkSpecialState();
	virtual ~ioEvyBlinkSpecialState();
};

inline ioEvyBlinkSpecialState* ToEvyBlinkSpecialState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_EVY_BLINK )
		return NULL;

	return dynamic_cast< ioEvyBlinkSpecialState* >( pState );
}

//-----------------------------------------------------------------------------------------------------------

class ioEvyBombSpecialState : public ioSpecialStateBase
{
public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_EVY_BOMB_STATE"; }
	virtual const CharState GetState(){ return CS_EVY_BOMB_STATE; }	
	virtual bool IsEnableBlowWounded( ioBaseChar *pOwner ){ return false; }

public:
	virtual void CheckSpecialState( ioBaseChar* pOwner );

public:
	ioEvyBombSpecialState();
	virtual ~ioEvyBombSpecialState();
};

inline ioEvyBombSpecialState* ToEvyBombSpecialState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_EVY_BOMB_STATE )
		return NULL;

	return dynamic_cast< ioEvyBombSpecialState* >( pState );
}