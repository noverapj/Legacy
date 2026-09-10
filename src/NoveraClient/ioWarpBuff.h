#pragma once

#include "ioBuff.h"
#include "ioSpecialStateBase.h"

class ioBaseChar;
class ioINILoader;

class ioWarpBuff : public ioBuff
{
public:
	enum WarpState
	{
		SS_NONE,
		SS_WAIT,
		SS_TELEPORT,
		SS_END,
	};

protected:
	ioHashString m_ReadyCircle;

	ioHashString m_TeleportDelayAni;
	float m_fTeleportDelayAniRate;

	ioHashString m_TeleportStartAni;
	ioHashString m_TeleportStartEffect;
	float m_fTeleportStartAniRate;

	ioHashString m_TeleportEndAni;
	ioHashString m_TeleportEndEffect;
	float m_fTeleportEndAniRate;

	WarpState m_State;
	CEncrypt<float> m_fCircleRange;
	CEncrypt<float> m_fCircleMoveSpeed;
	CEncrypt<float> m_fCircleHeightGap;
	CEncrypt<float> m_fCircleOffSet;
	D3DXVECTOR3 m_vCirclePos;

	ioHashString m_RedEffect;
	ioHashString m_BlueEffect;
	UniqueObjID m_dwCurMapEffect;
	DWORD m_dwMotionEndTime;

	bool m_bFirstJump;

protected:
	bool CheckSpecialState( ioBaseChar *pOwner );
	void SetSpecialState( ioBaseChar *pOwner );

	void SetWaitState( ioBaseChar *pChar );
	void SetTeleportState( ioBaseChar *pChar, bool bSendPacket );
	void SetEndState( ioBaseChar *pChar );

	D3DXVECTOR3 GetMagicCirclePos( ioBaseChar *pChar );

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

	virtual bool IsRemoveEnable();
	virtual void ApplyExtraBuffInfo( SP2Packet &rkPacket );

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();

	void ProcessSpecialState( ioBaseChar *pOwner );
	void ClearSpecialState( ioBaseChar *pOwner );

public:
	ioWarpBuff();
	ioWarpBuff( const ioWarpBuff &rhs );
	virtual ~ioWarpBuff();
};

inline ioWarpBuff* ToWarpBuff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_WARP_BUFF )
		return NULL;

	return dynamic_cast< ioWarpBuff* >( pBuff );
}

class ioWarpBuffSpecialState : public ioSpecialStateBase
{
public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_WARP_BUFF_SPECIAL"; }
	virtual const CharState GetState(){ return CS_WARP_BUFF_SPECIAL; }	
	virtual bool IsSendControlInfoToUser( ioBaseChar* pOwner ){ return false; }

public:
	virtual void CheckSpecialState( ioBaseChar* pOwner );
	virtual void CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight );

public:
	ioWarpBuffSpecialState();
	virtual ~ioWarpBuffSpecialState();
};

inline ioWarpBuffSpecialState* ToWarpBuffSpecialState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_WARP_BUFF_SPECIAL )
		return NULL;

	return dynamic_cast< ioWarpBuffSpecialState* >( pState );
}