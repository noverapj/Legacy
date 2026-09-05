#pragma once

#include "ioSpecialStateBase.h"
#include "ioChargeCommandItem.h"
#include "ioWeaponItem.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioOdinItem : public ioWeaponItem
{
public:
	enum OdinState
	{
		KMS_NONE,
		KMS_NORMAL_ATTACK,
		KMS_CHARGING,		
		KMS_FULL_CHARGE_SELLECT,
		KMS_CIRCLE_LOOP,
		KMS_CIRCLE_ACTION,
	};
	enum OdinCircleType
	{
		OCT_ATTACK,
		OCT_DEFENSE,
		OCT_JUMP,

		OCT_SIZE,
	};
	struct CircleInfo
	{
		float m_fRange;
		float m_fMoveSpeed;
		float m_fHeightGap;
		DWORD m_dwDummyCreateTime;
		ioHashString m_szMapEffect;
		ioHashString m_szCircleLoopAni;
		ioHashString m_szAreaWeaponName;
		ioHashString m_szDummyName;
		AttackAttribute m_ActionAttack;
	};

protected:
	OdinState m_OdinState;
	DWORD m_dwChargeStartTime;
	int m_iCurCombo;

protected:
	ioHashString m_BranchDelayAnimation;
	ioHashString m_stFullChargeAni;
	float m_fFullChargeAniRate;  
	int	m_iCircleType;
	int	m_iReadyCircleType;

	ioHashString m_ReadyCircle;
	ioHashString m_EnableCircle;
	
	CircleInfo m_cCircleInfo[OCT_SIZE];
	D3DXVECTOR3 m_vCirclePos;
	DWORD m_dwMotionEndTime;
	DWORD m_dwFireStartTime;
	DWORD m_dwCreateDummyTime;
	UniqueObjID	m_UniqueObjID;

	//gauge
	CEncrypt<float> m_fMaxGauge;
	CEncrypt<float> m_fNeedGauge;
	CEncrypt<float> m_fIncreaseGauge;
	CEncrypt<float> m_fCurGauge;

private:
	void ChangeFullChargeState( ioBaseChar* pOwner );
	void OnFullChargeState( ioBaseChar* pOwner );
	void OnCircleLoop( ioBaseChar *pOwner );
	void OnCircleAction( ioBaseChar *pOwner );

	void SetActionState( ioBaseChar *pChar, bool bSend );
	void CreateDummyChar( ioBaseChar *pOwner );
	void CreateWeapon( ioBaseChar *pOwner );

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();

public:
	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );
	virtual void OnEndNormalAttack( ioBaseChar *pOwner );

	virtual void OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual void OnReleased( ioBaseChar *pOwner );

	virtual bool IsChargeAttackState( ioBaseChar *pOwner );
	virtual bool IsEnableAttackRotateCheck( ioBaseChar *pOwner );

public:
	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );
	virtual WeaponSubType GetSubType() const;

public:
	OdinState GetOdinState();

	virtual int GetMaxBullet();
	virtual int GetCurBullet();
	virtual int GetNeedBullet();
	virtual void SetCurBullet( int iCurBullet );	

	bool IsEnableGauge();
	void DecreaseGauge();

protected:
	void OnCharging( ioBaseChar *pOwner );

	void ChangeToNormalAttack( ioBaseChar *pOwner );
	void ChangeToCircleLoop( ioBaseChar *pOwner, int iCircleType );
	void ReadyOdinCircleType( ioBaseChar *pOwner, int iCircleType );

public:
	ioOdinItem();
	ioOdinItem( const ioOdinItem &rhs );
	virtual ~ioOdinItem();
};

inline ioOdinItem* ToOdinItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_ODIN_ITEM )
		return NULL;

	return dynamic_cast< ioOdinItem* >( pWeapon );
}