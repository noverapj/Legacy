#pragma once

#include "ioExtendDash.h"

class ioBaseChar;
class ioINILoader;

class ioTeleportDash : public ioExtendDash
{
protected:
	enum DASH_STATE
	{
		DS_NONE,
		DS_TELEPORT_START,
		DS_TELEPORT_END_ACTION,
		DS_TELEPORT_END,
		DS_OTHER_ACTION,
	};
protected:
	ioHashString m_TeleportDashStartAction;

	AttackAttribute m_TeleportDashEndAction;
	
	// 텔레포트 후 아래가 지정한 높이보다 아래거나 드랍존이면 점프하기위한 attribute
	AttackAttribute m_TeleportJumpAttribute;
	float m_fTeleportJumpPower;
	
	DWORD m_dwActionEndTime;

	float m_fDashSpeedRate;

	// 텔레포트 거리
	float m_fTeleportOffset;

	DASH_STATE m_DashState;

	D3DXVECTOR3 m_vTargetPos;
	D3DXVECTOR3 m_vTeleportPos;

	// effect
	ioHashString m_szTeleportStartEffect;
	ioHashString m_szTeleportEndEffect;

	DWORD m_dwReserveAttackTime;

	// Jump
	float m_fOtherActionJumpPower;

	float m_fTeleportLimitingHeight;

	// 다른 용병이 이 대시를 사용할때 그 용병 쪽에서 보내오는 대시 사용 가능 표시 플래그
	bool m_bEnableTeleportDash;

	// 텔레포트 대시 후 딜레이
	DWORD m_dwTeleportEndDelay;
	DWORD m_dwTeleportEndDelayTime;

	float m_fJumpHeightGap;
	
public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual DashType GetType() const;
	virtual ioExtendDash* Clone();

public:
	virtual void ApplyDash( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual bool StartDash( ioBaseChar *pOwner );
	virtual void ProcessDash( ioBaseChar *pOwner );

	void SetEndAction( ioBaseChar *pOwner );
	void CheckRefreshDashAction( ioBaseChar *pOwner );

	virtual void ExtendDashEnd( ioBaseChar *pOwner );

	bool IsEndAction();

	void SetOtherAction( ioBaseChar* pOwner );

	bool IsCanTeleportDash() { return m_bEnableTeleportDash; }

protected:
	virtual void CheckKeyReserve( ioBaseChar *pOwner );

public:
	ioTeleportDash();
	ioTeleportDash( const ioTeleportDash &rhs );
	virtual ~ioTeleportDash();
};

inline ioTeleportDash* ToTeleportDash( ioExtendDash *pDash )
{
	if( !pDash || pDash->GetType() != DT_TELEPORT )
		return NULL;

	return dynamic_cast< ioTeleportDash* >( pDash );
}

