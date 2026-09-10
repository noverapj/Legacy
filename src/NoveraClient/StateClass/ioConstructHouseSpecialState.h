#pragma once

#include "../ioSpecialStateBase.h"
#include "../Housing/BlockDefine.h"
#include "../Housing/ioBlockEventAgency.h"

class ioUserBlockStorage;

class ioConstructHouseSpecialState : public ioSpecialStateBase, public ioBlockEventListener
{
public:
	enum State
	{
		S_PRE,
		S_SETUP,		
		S_ERASE,
		S_WAIT,
	};

protected:
	enum KeyEvent
	{
		KE_NONE,
		KE_FRONT,
		KE_BACK,
		KE_RIGHT,
		KE_LEFT,
		KE_UP,
		KE_DOWN,
		KE_LEFT_ROTATE,
		KE_RIGHT_ROTATE,
		KE_EXCECUTE,
		KE_CHANGE_MODE,
		KE_CHANGE_PREV_ITEM,
		KE_CHANGE_NEXT_ITEM,
	};

	enum ActionState
	{
		AS_NONE,
		AS_ENABLE_SETUP,
		AS_ENABLE_REMOVE,
	};

	enum SystemChatType
	{
		SCT_NONE,
		SCT_CANT_SETUP,
		SCT_NOT_FIND_SETUP_OBJ,
		SCT_NOT_FIND_RMOVE_OBJ,
		SCT_BLOCK_MAX,
	};

protected:
	State				m_State;
	ActionState			m_ActionState;
	SystemChatType		m_SystemChatType;
	DWORD				m_dwSystemChatTime;	
	TilePos				m_Pos;

protected:
	BlockModeType		m_BlockModeType;

protected:	
	DWORD				m_dwCurrSetUpCode;
	DWORD				m_dwCurrRemoveCode;
	__int64				m_nRemoveUniqueIndex;

	DWORD				m_dwCurrKeyCheckTime;
	
	RotateSet			m_eCurrRotate;

	ioHashString		m_CameraBuff;

protected:
	ioHashString		m_SetUpCircle;
	ioHashString		m_SetUpDisableCircle;

	ioHashString		m_RemoveCircle;
	ioHashString		m_RemoveDisableCircle;

public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );
	virtual void LoadProperty( ioINILoader &rkLoader );

	virtual const char* GetStateName(){ return "CS_CONSTRUCT_HOUSE"; }
	virtual const CharState GetState(){ return CS_CONSTRUCT_HOUSE; }
	virtual const State GetCurrState(){ return m_State; }

	virtual bool IsSendControlInfoToUser( ioBaseChar* pOwner ){ return true; }
	virtual bool IsFallStateCheck(){ return false; }

public:
	ioUserBlockStorage* GetStorage();

public:
	int GetCurrRemoveCode(){ return m_dwCurrRemoveCode; }

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

	// 중간 입장 발생시 동기화 선택
	virtual bool IsJoinSync(){ return true; }

public:
	void ProcesEditState( ioBaseChar* pOwner );

protected:
	void CheckStart( ioBaseChar *pOwner, ioPlayStage* pStage );
	void CheckLoop( ioBaseChar *pOwner, ioPlayStage* pStage );

public:
	void ProcessSetUpBlock( ioBaseChar *pOwner, ioPlayStage* pStage,  KeyEvent eEvent, const TilePos& NextPos, RotateSet eRotate );
	void ProcessRemoveBlock( ioBaseChar *pOwner, ioPlayStage* pStage,  KeyEvent eEvent, const TilePos& NextPos, RotateSet eRotate );

protected:
	ioConstructHouseSpecialState::KeyEvent CheckPosKey( ioBaseChar *pOwner, ioPlayStage* pStage, TilePos& Pos, int& nRotate );

public:
	void ChangeToPre( ioBaseChar* pOwner, ioPlayStage* pStage );
	void ChangeToSetUp( ioBaseChar *pOwner, ioPlayStage* pStage );
	void ChangeToRemove( ioBaseChar *pOwner, ioPlayStage* pStage );

	bool SendSetUp();
	bool SendRemove();

public:
	void ClearSystemChatReserve();
	void SystemChat( SystemChatType eType );

public:
	ioConstructHouseSpecialState();
	virtual ~ioConstructHouseSpecialState();
};

inline ioConstructHouseSpecialState* ToConstructHouseSpecialState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_CONSTRUCT_HOUSE )
		return NULL;

	return dynamic_cast< ioConstructHouseSpecialState* >( pState );
}