#pragma once

#include "ioExtendDash.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioBaseChar;
class ioPlayStage;

class ioGenisysarmyDash : public ioExtendDash
{
protected:
	enum DashState
	{
		DS_ATTACK,
	};

protected:
	AttackAttribute		m_AttackAttribute;		// 대시 구르기 
	int					m_iExtraAttackMaxCnt;	// 대시 +D 카운트
	AttackAttribute		m_ExtraAttackAttribute;	// 대시 +D
	DWORD				m_dwActionEndTime;		// 
	int					m_iCurExtendCnt;		// 대시 구르기 중 공격키 카운트
	bool				m_bSetGenisysarmy;		// 대시 구르기 중 공격키 +D 

protected:
	// For Ani Rotate - 차후에 개발완료 후에 삭제하자!
	vAniRotateInfoList	m_vAniRotateInfoList;
	int					m_iCurAniRotate;
	DWORD				m_dwAniRotateTime;
	float				m_fAniRotateAngle;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual DashType GetType() const;
	virtual ioExtendDash* Clone();

	virtual bool StartDash( ioBaseChar *pOwner );
	virtual void ProcessDash( ioBaseChar *pOwner );
	virtual void ApplyDash( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual void CheckAniRotate( ioBaseChar *pChar );

public:
	void SetGenisysarmyDash( ioBaseChar *pOwner );
	void SetExtraInfoToWeapon( ioBaseChar *pOwner, ioPlayStage *pStage, ioWeapon *pWeapon, const ioHashString &szName );

protected:
	virtual void CheckKeyReserve( ioBaseChar *pOwner );	
	void CheckExtendReserve( ioBaseChar *pOwner );

public:
	ioGenisysarmyDash();
	ioGenisysarmyDash( const ioGenisysarmyDash &rhs );
	virtual ~ioGenisysarmyDash();
};

inline ioGenisysarmyDash* ToGenisysarmyDash( ioExtendDash *pDash )
{
	if( !pDash || pDash->GetType() != DT_GENISYSARMY )
	{
		return NULL;
	}
	return dynamic_cast< ioGenisysarmyDash* >( pDash );
}

