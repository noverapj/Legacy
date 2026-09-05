#pragma once

#include "ioSpecialStateBase.h"
#include "ioChargeCommandItem.h"
#include "ioWeaponItem.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioDimensionistItem : public ioWeaponItem
{
public:
	enum DimensionistState
	{
		KMS_NONE,
		KMS_NORMAL_ATTACK,
		KMS_CHARGING,		
		KMS_FULL_CHARGE_SELLECT,
		KMS_CIRCLE_LOOP,
		KMS_CIRCLE_ACTION,
		KMS_CANCEL_DASH,
		KMS_CREATE_DUMMY,
	};
	//enum DimensionistCircleType
	//{
	//	OCT_ATTACK,
	//	OCT_DEFENSE,
	//	OCT_JUMP,

	//	OCT_SIZE,
	//};
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
	DimensionistState m_DimensionistState;
	DWORD m_dwChargeStartTime;
	int m_iCurCombo;

protected:
	ioHashString m_BranchDelayAnimation;
	ioHashString m_stFullChargeAni;
	float m_fFullChargeAniRate;  

	ioHashString m_ReadyCircle;
	ioHashString m_EnableCircle;
	
	CircleInfo m_cCircleInfo;
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

	float m_fBulletIncreaseRate;

	int		m_iCurBullet;
	int		m_iMaxBullet;
	int		m_iDashNeedBullet;
	int		m_iSpecialAttackNeedBullet;
	float	m_fTempBullet;

	DWORD	m_dwSpecialStateChargeTime;
	DWORD	m_dwSpecialStateChargeStartTime;

	DWORD	m_dwSpecialStateEndStartTime;

	// CircleLoop 확인 카운트
	int	m_iSpecialAttackCount;

	// 블랙홀 더미 생성한 위치 저장
	D3DXVECTOR3 m_vCreateDummyPos;

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
	virtual bool IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck ) { return true; }

public:
	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );
	virtual WeaponSubType GetSubType() const;

public:
	DimensionistState GetDimensionistState();

	virtual int GetMaxBullet();
	virtual int GetCurBullet();
	virtual int GetNeedBullet();
	virtual void SetCurBullet( int iCurBullet );	
	int GetSpecialAttackNeedBullet();

	bool CheckEnoughBulletsToDoDash();
	void DecreaseBulletsToDoDash();
	bool CheckEnoughBulletsToDoSpecialAttack();
	void DecreaseBulletsToDoSpecialAttack();

	bool IsEnableGauge();
	void DecreaseGauge();

protected:
	void OnCharging( ioBaseChar *pOwner );

	void ChangeToNormalAttack( ioBaseChar *pOwner );
	void ChangeToCircleLoop( ioBaseChar *pOwner );
	void ReadyDimensionistCircleType( ioBaseChar *pOwner );

	virtual void UpdateExtraData( ioBaseChar *pOwner );

	virtual bool ProcessCancelBySpeicalDash( ioBaseChar *pOwner );
	virtual bool IsConditionalSpecialDashReserve( ioBaseChar *pOwner );
	virtual void SetChangeConditionalSpecialDash( ioBaseChar *pOwner, ioUserKeyInput::DirKeyInput eCurrDir, int eCallerType );

public:
	ioDimensionistItem();
	ioDimensionistItem( const ioDimensionistItem &rhs );
	virtual ~ioDimensionistItem();
};

inline ioDimensionistItem* ToDimensionistItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_DIMENSIONIST_ITEM )
		return NULL;

	return dynamic_cast< ioDimensionistItem* >( pWeapon );
}