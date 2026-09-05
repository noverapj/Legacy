#pragma once

#include "ioDummyChar.h"
#include "ioOutBoxerItem.h"

class ioShadowBoxerDummy :	public ioDummyChar
{
protected:
	//State
	CharState								m_OwnerState;
	JumpState								m_OwnerJumpState;
	ioOutBoxerItem::OutBoxerChargeState		m_OwnerOutBoxerChargeState;

	//Position, Direction
	D3DXVECTOR3								m_vDummyOffset;
	D3DXVECTOR3								m_vOwnerPosition;
	D3DXVECTOR3								m_vDummyPosition;
	D3DXQUATERNION							m_qtOwnerRot;

	//CS_COUNTER_ATTACK
	AttackAttribute							m_CounterAttack;

	//CS_ATTACK
	int										m_iCurNormalAttackComboCount;
	AttributeList							m_NormalAttackList;
	
	//OutBoxer State
	AttackAttribute							m_BoxerAttack;
	AttackAttribute							m_SecondStepAttack;
	int										m_iCurMovingCount;
	int										m_iCurSecondStepDefenseCount;

	//JumpAttack
	AttackAttribute							m_JumpAttack;

	//Dash Attack( Only CS_DASH_ATTACK )
	AttributeList						m_DashAttakList;
	int										m_iCurExtraDashAttack;	//check dash

	//collision
	ioAniCollisionBoxGrp					*m_pAniCollisionBoxGrp;

	//check hide
	bool									m_bOwnerInvisibleState;

	//ani
	ioHashString							m_szActionAni;
	ioHashString							m_szLoopAni;

protected:
	void ClearStateData();

	void SetActionAni( const ioHashString& szAniName, float fRate );
	void SetLoopAni( const ioHashString& szAniName, float fRate );
	void SetAttackAni( const AttackAttribute& CurAttackAttribute );

	void ProcessStartState();
	void ProcessDelayState( float fTimePerSec, DWORD dwPreTime );
	
	void CheckOutBoxerAttackState( ioBaseChar *pOwner, DWORD dwPreTime );
	void CheckJumpState( ioBaseChar *pOwner, DWORD dwPreTime );
	void CheckDashAttackState( ioBaseChar *pOwner, DWORD dwPreTime );
	void CheckAttackTime( ioBaseChar *pOwner, DWORD dwPreTime );
	void CheckLive();
	void CheckShadowPosition();
	void CheckHide();

	void ChangeNoneAttackAni( ioBaseChar *pOwner, CharState OwnerState );
	void ChangeAttackAni( ioBaseChar *pOwner, CharState OwnerState );

	void UpdateWorldAxisBox() const;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioOrientBox GetAttackCollisionBox( AniCollisionType eType = ACT_NONE, bool bNoScale=false );
	virtual void UpdateBound() const;
	
protected:
	virtual void ProcessState( float fTimePerSec, DWORD dwPreTime );
	
public:
	ioShadowBoxerDummy( ioEntityGroup *pGrp, ioPlayMode *pMode );
	virtual ~ioShadowBoxerDummy(void);
};

inline ioShadowBoxerDummy* ToShadowBoxerDummy( ioDummyChar *pDummy )
{
	if( !pDummy || pDummy->GetType() != DCT_SHADOW_BOXER )
		return NULL;

	return dynamic_cast< ioShadowBoxerDummy* >( pDummy );
}