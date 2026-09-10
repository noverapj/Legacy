#pragma once

#include "../ioSpecialStateBase.h"
#include "../Housing/BlockDefine.h"
#include "../Housing/ioBlockEventAgency.h"
#include "../Housing/ioBlockProperty.h"

class ioBlockInterplaySpecialState : public ioSpecialStateBase, public ioBlockEventListener
{
public:
	enum InterPlayState
	{
		IPS_NONE,
		IPS_PRE,
		IPS_LOOP,
		IPS_ACTION,
		IPS_END,
	};
	
	enum SyncState
	{
		SS_PRE,
		SS_END,
	};

protected:
	InterPlayState	m_State;

protected:
	__int64			m_nBlockUniqueIndex;
	int				m_nCollIndex;
	D3DXVECTOR3		m_vPos;
	D3DXQUATERNION	m_qtRot;

protected:
	int				m_eInterPlayType;
	DWORD			m_dwPreEndTime;
	DWORD			m_dwActionEndTime;
	DWORD			m_dwEndStateEndTime;

protected:
	const ioBlockProperty::InterPlayInfo* m_pCurrInterPlay;

public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );
	virtual void LoadProperty( ioINILoader &rkLoader );

	virtual const char* GetStateName(){ return "CS_BLOCK_INTEPLAY"; }
	virtual const CharState GetState(){ return CS_BLOCK_INTEPLAY; }

	virtual bool IsSendControlInfoToUser( ioBaseChar* pOwner ){ return true; }
	virtual bool IsFallStateCheck(){ return false; }
	virtual bool IsStateEnd( ioBaseChar* pOwner );

public:
	virtual bool IsCharCollisionSkipState( const ioBaseChar *const pOwner,
		DWORD dwStartTime,
		DWORD dwEndTime,
		DWORD dwSkipType,
		bool bTeamOnly,
		bool bDefense,
		TeamType eOwnerTeam,
		TeamType eOtherTeam ) const;

	virtual void CheckSpecialState( ioBaseChar* pOwner );
	virtual void CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight );
	virtual bool ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime );

public:
	void Start( ioBaseChar* pOwner, const D3DXVECTOR3& vPos, const D3DXQUATERNION& qtRot, __int64 nUniqueIndex, int nCollIndex );

protected:
	void ChangeToPre( ioBaseChar* pOwner, ioPlayStage* pStage );
	void ChangeToLoop( ioBaseChar* pOwner, ioPlayStage* pStage );
	void ChangeToAction( ioBaseChar* pOwner, ioPlayStage* pStage );
	void ChangeToEnd( ioBaseChar* pOwner, ioPlayStage* pStage );

protected:
	void CheckBlock( ioBaseChar* pOwner, ioPlayStage* pStage );
	void CheckPre( ioBaseChar* pOwner, ioPlayStage* pStage );
	void CheckLoop( ioBaseChar* pOwner, ioPlayStage* pStage );
	void CheckAction( ioBaseChar* pOwner, ioPlayStage* pStage );

public:
	// 베이스 캐릭터에서 SetState()로 상태 클래가 생성 됬을떄 동기화 선택
	virtual bool IsInitializeSync(){ return true; }

	// 중간 입장 발생시 동기화 선택
	virtual bool IsJoinSync(){ return true; }

	//중간 입장 동기화
	virtual void FillJoinSync(  SP2Packet &rkPacket, ioBaseChar* pOwner );
	virtual void ApplyJoinSync( SP2Packet &rkPacket, ioBaseChar* pOwner );

	//스테이트 내부 동기화
	virtual void ApplyStateSync( SP2Packet &rkPacket, ioBaseChar* pOwner );

public:
	ioBlockInterplaySpecialState();
	virtual ~ioBlockInterplaySpecialState();
};

inline ioBlockInterplaySpecialState* ToBlockInterplaySpecialState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_BLOCK_INTEPLAY )
		return NULL;

	return dynamic_cast< ioBlockInterplaySpecialState* >( pState );
}