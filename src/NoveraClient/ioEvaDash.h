#pragma once

#include "ioSizeItem.h"
#include "ioExtendDash.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioBaseChar;
class ioPlayStage;

class ioEvaDash : public ioExtendDash
{
public:
	enum ChargeDashState
	{
		CDS_NONE,

		// 에어리어웨폰 범위 안에서는 대시 시 텔레포트 사용 가능
		CDS_TELEPORT_START,
		CDS_TELEPORT_END_ACTION,
		CDS_TELEPORT_END,
		CDS_OTHER_ACTION,
	};

protected:
	ChargeDashState		m_DashState;
	AttackAttribute		m_AttackAttribute;
	DWORD				m_dwActionEndTime;

	// 텔레포트 사용 여부
	bool				m_bTeleport;
	ioHashString		m_szBuffName;

	
	ioHashString		m_TeleportDashStartAction;
	AttackAttribute		m_TeleportDashEndAction;

	// 텔레포트 후 아래가 지정한 높이보다 아래거나 드랍존이면 점프하기위한 attribute
	AttackAttribute		m_TeleportJumpAttribute;
	float				m_fTeleportJumpPower;	

	float				m_fDashSpeedRate;

	float				m_fTeleportOffset;	// 텔레포트 거리

	D3DXVECTOR3			m_vTargetPos;
	D3DXVECTOR3			m_vTeleportPos;
		
	ioHashString		m_szTeleportStartEffect;	// effect
	ioHashString		m_szTeleportEndEffect;

	DWORD				m_dwReserveAttackTime;

	// Jump
	float				m_fOtherActionJumpPower;

	float				m_fTeleportLimitingHeight;

	// 다른 용병이 이 대시를 사용할때 그 용병 쪽에서 보내오는 대시 사용 가능 표시 플래그
	bool				m_bEnableTeleportDash;

	// 텔레포트 대시 후 딜레이
	DWORD				m_dwTeleportEndDelay;
	DWORD				m_dwTeleportEndDelayTime;

	float				m_fJumpHeightGap;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual DashType GetType() const;
	virtual ioExtendDash* Clone();

	virtual bool StartDash( ioBaseChar *pOwner );
	virtual void ProcessDash( ioBaseChar *pOwner );
	virtual void ApplyDash( ioBaseChar *pOwner, SP2Packet &rkPacket );


	// 에어리어웨폰 범위 안에서는 대시 시 텔레포트 사용 가능
	virtual void ExtendDashEnd( ioBaseChar *pOwner );	
	void TeleportStartDash( ioBaseChar *pOwner );
	void SetOtherAction( ioBaseChar* pOwner );


public:
	bool IsCanEvaTeleportDash( ioBaseChar *pOwner );


protected:
	virtual void CheckKeyReserve( ioBaseChar *pOwner );

public:
	ioEvaDash();
	ioEvaDash( const ioEvaDash &rhs );
	virtual ~ioEvaDash();
};

inline ioEvaDash* ToEvaDash( ioExtendDash *pDash )
{
	if( !pDash || pDash->GetType() != DT_EVA )
		return NULL;

	return dynamic_cast< ioEvaDash* >( pDash );
}

