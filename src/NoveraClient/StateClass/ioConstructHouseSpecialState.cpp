
#include "stdafx.h"

#include "ioConstructHouseSpecialState.h"
#include "../ioBaseChar.h"
#include "../ioPlayStage.h"

#include "../Housing/ioBlockWorld.h"
#include "../Housing/ioBlockSetupManager.h"
#include "../Housing/ioBlockEventParameter.h"
#include "../Housing/ioUserBlockStorage.h"

ioConstructHouseSpecialState::ioConstructHouseSpecialState()
{
}

ioConstructHouseSpecialState::~ioConstructHouseSpecialState()
{
}

void ioConstructHouseSpecialState::LoadProperty( ioINILoader &rkLoader )
{
	ioSpecialStateBase::LoadProperty( rkLoader );

	char szBuf[MAX_PATH]="";

	rkLoader.LoadString_e( "setup_circle", "", szBuf, MAX_PATH );
	m_SetUpCircle		 = szBuf;
	rkLoader.LoadString_e( "setup_disable_circle", "", szBuf, MAX_PATH );
	m_SetUpDisableCircle = szBuf;

	rkLoader.LoadString_e( "remove_circle", "", szBuf, MAX_PATH );
	m_RemoveCircle		 = szBuf;
	rkLoader.LoadString_e( "remove_disable_circle", "", szBuf, MAX_PATH );
	m_RemoveDisableCircle = szBuf;
}

void ioConstructHouseSpecialState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );

	m_Pos.Init();

	m_dwCurrSetUpCode		= 0;
	m_dwCurrRemoveCode		= 0;
	m_nRemoveUniqueIndex	= 0;
	m_dwCurrKeyCheckTime	= 0;
	m_eCurrRotate			= ROTATE_0;
	m_ActionState			= AS_NONE;
	m_SystemChatType		= SCT_NONE;
	m_dwSystemChatTime		= 0;

	ChangeToPre( pOwner, pOwner->GetCreator() );

	m_BlockModeType	= BMT_GUILD;

	if( pOwner->GetCreator() )
	{
		ioBlockWorld* pWorld = pOwner->GetCreator()->GetBlockWorld();
		if( pWorld )
		{
			m_BlockModeType = pWorld->GetBlockModeType();
		}
	}
}

void ioConstructHouseSpecialState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );

	g_GUIMgr.HideWnd( HOUSING_BLOCK_SETUP_WND );
	pOwner->DestroyMagicCircle();
	pOwner->SetSpecialHide( false );

	if( pOwner->GetCreator() )
	{
		ioBlockWorld* pWorld = pOwner->GetCreator()->GetBlockWorld();
		if( pWorld )		
			pWorld->DestroySetupBlock();

		ioBlock* pBlock = pWorld->GetMapBase()->GetBlockByTilePos( m_Pos );
		if( pBlock )
		{
			pBlock->ChangeNone();
		}
	}
}

bool ioConstructHouseSpecialState::IsCharCollisionSkipState( const ioBaseChar *const pOwner, DWORD dwStartTime, DWORD dwEndTime, DWORD dwSkipType, bool bTeamOnly, bool bDefense, TeamType eOwnerTeam, TeamType eOtherTeam ) const
{	
	return true;
}

void ioConstructHouseSpecialState::CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight )
{
}

bool ioConstructHouseSpecialState::ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime )
{
	return true;
}

ioUserBlockStorage* ioConstructHouseSpecialState::GetStorage()
{
	ioUserBlockStorage* pStorage = NULL;
	switch( m_BlockModeType )
	{
	case BMT_GUILD:
		pStorage = g_App.GetGuildStorage();
		break;
	case BMT_PERSONAL:
		pStorage = g_App.GetHomeStorage();
		break;
	}

	return pStorage;
}

void ioConstructHouseSpecialState::CheckSpecialState( ioBaseChar* pOwner )
{
	//다른 버프나 스킬에 의해서 은신 상태가 풀릴 수 있음으로 매 프로세스 마다 하이드 처리
	pOwner->SetSpecialHide( true );		

	switch( m_State )
	{
	case S_PRE:
		{
			//패킷 응답 대기
		}
		break;
	case S_SETUP:
	case S_ERASE:
		{
			ProcesEditState( pOwner );
		}
		break;
	case S_WAIT:
		{
			//패킷 응답 대기
		}
		break;
	}
}

void ioConstructHouseSpecialState::ChangeToPre( ioBaseChar* pOwner, ioPlayStage* pStage )
{
	m_State = S_PRE;
}

void ioConstructHouseSpecialState::ProcesEditState( ioBaseChar* pOwner )
{
	if(!pOwner->IsNeedProcess() )
		return;

	TilePos NextPos( 0, 0, 0 );
	int nRotate = m_eCurrRotate;
	KeyEvent eEvent = CheckPosKey( pOwner, pOwner->GetCreator(), NextPos, nRotate );
	switch( eEvent )
	{
	case KE_CHANGE_MODE:
		{
			if( m_State == S_SETUP )
			{
				ChangeToRemove( pOwner, pOwner->GetCreator() );
			}
			else
			{
				ioUserBlockStorage* pStorage = GetStorage();
				if( pStorage )
					pStorage->SendBlockInvenSync( true );

				ChangeToSetUp( pOwner, pOwner->GetCreator() );
			}
		}
		break;
	case KE_CHANGE_PREV_ITEM:
		{
			ioUserBlockStorage* pStorage = GetStorage();
			if( pStorage )
				pStorage->PrevItem();

			g_BlockSetupManager.ChangeToSetUp();
		}
		break;
	case KE_CHANGE_NEXT_ITEM:
		{
			ioUserBlockStorage* pStorage = GetStorage();
			if( pStorage )
				pStorage->NextItem();

			g_BlockSetupManager.ChangeToSetUp();
		}
		break;
	case KE_FRONT:
	case KE_BACK:
	case KE_RIGHT:
	case KE_LEFT:
	case KE_UP:
	case KE_DOWN:
	case KE_LEFT_ROTATE:
	case KE_RIGHT_ROTATE:
	case KE_EXCECUTE:
		{
			switch( m_State )
			{
			case S_SETUP:
				ProcessSetUpBlock( pOwner, pOwner->GetCreator(), eEvent, NextPos, (RotateSet)nRotate );
				break;
			case S_ERASE:
				ProcessRemoveBlock( pOwner, pOwner->GetCreator(), eEvent, NextPos, (RotateSet)nRotate );
				break;
			}
		}
		break;
	}

	if( m_dwSystemChatTime + 250 < FRAMEGETTIME() )
	{
		ClearSystemChatReserve();
	}
}

void ioConstructHouseSpecialState::ChangeToSetUp( ioBaseChar *pOwner, ioPlayStage* pStage )
{
	m_State					= S_SETUP;
	m_dwCurrSetUpCode		= 0;
	m_dwCurrRemoveCode		= 0;
	m_nRemoveUniqueIndex	= 0;
	m_ActionState			= AS_NONE;

	if( !pOwner->IsNeedProcess() )
		return;

	ioBlockWorld* pWorld = pStage->GetBlockWorld();
	if( !pWorld )
		return;

	ioUserBlockStorage* pStorage = GetStorage();
	if( pStorage == NULL )
		return;

	ioUserBlockStorage::InvenBlockItem* pItem = pStorage->GetCurrInvenBlockItem();
	if( pItem )
		m_dwCurrSetUpCode = pItem->m_nBlockCode;

	//이전 위치의 선택을 해제
	ioBlock* pBlock = pWorld->GetMapBase()->GetBlockByTilePos( m_Pos );
	if( pBlock )
		pBlock->ChangeNone();

	if( m_Pos.IsNotTilePos() )
	{
		D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
		float fHeight = pWorld->GetHeightByOBB( vPos.x, vPos.z );
		if( vPos.y < fHeight )
			vPos.y = fHeight + FLOAT10;

		if( pWorld->GetMapBase() )
			pWorld->GetMapBase()->WorldCoordToTileCoord( vPos, m_Pos );
	}

	int eBlockType = m_dwCurrSetUpCode / ioBlock::BT_DIVIDE;
	if( eBlockType == ioBlock::BT_TILE )
		m_Pos.y = 1;

	D3DXVECTOR3 vPos;
	TileCoordToWorldCoord( m_Pos.x, m_Pos.y, m_Pos.z, vPos.x, vPos.y, vPos.z );

	pWorld->DestroySetupBlock();
	pWorld->CreateSetupBlock( m_dwCurrSetUpCode, m_Pos );

	ioBlock* pSetUpBlock = pWorld->GetSetUpBlock();
	if( pSetUpBlock )
	{
		float fRotate = m_eCurrRotate * ROTATE_VALUE;
		pSetUpBlock->Rotate( fRotate );
		if( pWorld->CheckBlockVSBlockCollision( pSetUpBlock, ZERO_TILE_POS, fRotate ) )
		{
			pSetUpBlock->ChangeDisable();
			D3DXVECTOR3 vPos( pSetUpBlock->GetWorldPosition().x, pSetUpBlock->GetWorldPosition().y + 3.0f, pSetUpBlock->GetWorldPosition().z );
			pSetUpBlock->SetWorldPosition( vPos );
		}
		else
		{
			if( pSetUpBlock->IsSetUp() )
			{
				pSetUpBlock->ChangeNone();
				m_ActionState = AS_ENABLE_SETUP;
			}
			else
			{
				pSetUpBlock->ChangeDisable();
				D3DXVECTOR3 vPos( pSetUpBlock->GetWorldPosition().x, pSetUpBlock->GetWorldPosition().y + 3.0f, pSetUpBlock->GetWorldPosition().z );
				pSetUpBlock->SetWorldPosition( vPos );
			}
		}
	}

	if( pSetUpBlock && pSetUpBlock->IsColorState() == ioBlock::CSS_NONE )
	{
		ioHashString szDummyCircle;
		pOwner->CreateMagicCircle( m_SetUpCircle, m_SetUpCircle, szDummyCircle, vPos, 0.0f );
		pOwner->SetMagicCirclePos( false, vPos );
	}
	else
	{
		ioHashString szDummyCircle;
		pOwner->CreateMagicCircle( m_SetUpDisableCircle, m_SetUpDisableCircle, szDummyCircle, vPos, 0.0f );
		pOwner->SetMagicCirclePos( false, vPos );
	}
}

void ioConstructHouseSpecialState::ChangeToRemove( ioBaseChar *pOwner, ioPlayStage* pStage )
{
	m_State					= S_ERASE; 
	m_dwCurrSetUpCode		= 0;
	m_dwCurrRemoveCode		= 0;
	m_nRemoveUniqueIndex	= 0;
	m_ActionState			= AS_NONE;

	if( !pOwner->IsNeedProcess() )
		return;

	ioBlockWorld* pWorld = pStage->GetBlockWorld();
	if( !pWorld )
		return;

	if( m_Pos.IsNotTilePos() )
	{
		D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
		float fHeight = pWorld->GetHeightByOBB( vPos.x, vPos.z );
		if( vPos.y < fHeight )
			vPos.y = fHeight + FLOAT10;

		if( pWorld->GetMapBase() )
			pWorld->GetMapBase()->WorldCoordToTileCoord( vPos, m_Pos );
	}
	pWorld->DestroySetupBlock();

	D3DXVECTOR3 vPos;
	TileCoordToWorldCoord( m_Pos.x, m_Pos.y, m_Pos.z, vPos.x, vPos.y, vPos.z );

	ioBlock* pBlock = pWorld->GetMapBase()->GetBlockByTilePos( m_Pos );
	if( pBlock )
	{
		pBlock->ChangeSelect();
		m_ActionState = AS_ENABLE_REMOVE;

		if( pBlock->GetProperty() )
		{
			m_dwCurrRemoveCode = pBlock->GetProperty()->GetCode();
		}

		ioHashString szDummyCircle;
		pOwner->CreateMagicCircle( m_RemoveCircle, m_RemoveCircle, szDummyCircle, vPos, 0.0f );
		pOwner->SetMagicCirclePos( false, vPos );
	}
	else
	{
		ioHashString szDummyCircle;
		pOwner->CreateMagicCircle( m_RemoveDisableCircle, m_RemoveDisableCircle, szDummyCircle, vPos, 0.0f );
		pOwner->SetMagicCirclePos( false, vPos );
	}
}

void ioConstructHouseSpecialState::ProcessSetUpBlock( ioBaseChar *pOwner, ioPlayStage* pStage, KeyEvent eEvent, const TilePos& NextPos, RotateSet eRotate )
{
	ioBlockWorld* pWorld = pStage->GetBlockWorld();
	if( !pWorld )
		return;

	m_ActionState = AS_NONE;

	TilePos Pos = m_Pos + NextPos;

	//이동 할 타일 위치가 0이고
	if( Pos.y == 0 )
	{
		//맵의 높이가 0이 아닌경우에는 블럭 설치 불가(월드맵과 충돌함)
		D3DXVECTOR3 vPos;
		TileCoordToWorldCoord( Pos.x, Pos.y, Pos.z, vPos.x, vPos.y, vPos.z );
		float fHeight = pStage->GetMapHeight( vPos.x, vPos.z, NULL, true, 0.0f, true );
		if( fHeight > 0.0f )
			return;
	}

	if( COMPARE( Pos.x, 0, g_BlockManager.GetTileMax() ) && COMPARE( Pos.y, 0, g_BlockManager.GetHeightMax() ) && COMPARE( Pos.z, 0, g_BlockManager.GetTileMax() ) )
	{
		int eBlockType = m_dwCurrSetUpCode / ioBlock::BT_DIVIDE;
		if( eBlockType == ioBlock::BT_TILE )
			Pos.y = 1;

		m_Pos = Pos;
		float fRotate	= eRotate * ROTATE_VALUE;
		m_eCurrRotate	= eRotate;

		ioBlock* pSetUpBlock = pWorld->GetSetUpBlock();
		if( pSetUpBlock )
		{
			bool bBlockCollision = false;
			if( pWorld->CheckBlockVSBlockCollision( pSetUpBlock, NextPos, fRotate ) )
			{
				bBlockCollision = true;
				pSetUpBlock->ChangeDisable();
			}

			pSetUpBlock->SetTitleCoord( Pos.x, Pos.y, Pos.z );
			pSetUpBlock->Rotate( fRotate );

			if( !bBlockCollision )
			{
				if( pSetUpBlock->IsSetUp() )
				{
					pSetUpBlock->ChangeNone();
					m_ActionState = AS_ENABLE_SETUP;
				}
				else
				{
					pSetUpBlock->ChangeDisable();
				}
			}

			if( pSetUpBlock->IsColorState() == ioBlock::CSS_DISABLE )
			{
				D3DXVECTOR3 vPos( pSetUpBlock->GetWorldPosition().x, pSetUpBlock->GetWorldPosition().y + FLOAT1, pSetUpBlock->GetWorldPosition().z );
				pSetUpBlock->SetWorldPosition( vPos );
			}
		}

		D3DXVECTOR3 vPos;
		TileCoordToWorldCoord( Pos.x, Pos.y, Pos.z, vPos.x, vPos.y, vPos.z );
		pOwner->SetWorldPosition( vPos );

		if( pSetUpBlock && pSetUpBlock->IsColorState() == ioBlock::CSS_NONE )
		{
			ioHashString szDummyCircle;
			pOwner->CreateMagicCircle( m_SetUpCircle, m_SetUpCircle, szDummyCircle, vPos, 0.0f );
			pOwner->SetMagicCirclePos( false, vPos );
		}
		else
		{
			ioHashString szDummyCircle;
			pOwner->CreateMagicCircle( m_SetUpDisableCircle, m_SetUpDisableCircle, szDummyCircle, vPos, 0.0f );
			pOwner->SetMagicCirclePos( false, vPos );
		}

		if( eEvent == KE_EXCECUTE )
		{
			if( pSetUpBlock )
			{
				if( pSetUpBlock->IsColorState() == ioBlock::CSS_NONE )
				{
					if( g_BlockManager.GetBlockTotalCount() < g_BlockSetupManager.GetBlockLimitCount() )
					{
						m_State = S_WAIT;
						g_BlockSetupManager.SendSetUpBlock( m_dwCurrSetUpCode, m_Pos.x, m_Pos.y, m_Pos.z, (byte)m_eCurrRotate );
					}
					else
					{
						SystemChat( SCT_BLOCK_MAX );
					}
				}
				else
				{
					SystemChat( SCT_CANT_SETUP );
				}
			}
			else
			{
				SystemChat(SCT_NOT_FIND_SETUP_OBJ );
			}
		}
	}
}

void ioConstructHouseSpecialState::ProcessRemoveBlock( ioBaseChar *pOwner, ioPlayStage* pStage,  KeyEvent eEvent, const TilePos& NextPos, RotateSet eRotate )
{
	ioBlockWorld* pWorld = pStage->GetBlockWorld();
	if( !pWorld )
		return;

	m_ActionState = AS_NONE;

	TilePos Pos = m_Pos + NextPos;

	//이동 할 타일 위치가 0이고
	if( Pos.y == 0 )
	{
		//맵의 높이가 0이 아닌경우에는 블럭 설치 불가(월드맵과 충돌함)
		D3DXVECTOR3 vPos;
		TileCoordToWorldCoord( Pos.x, Pos.y, Pos.z, vPos.x, vPos.y, vPos.z );
		float fHeight = pStage->GetMapHeight( vPos.x, vPos.z, NULL, true, 0.0f, true );
		if( fHeight > 0.0f )
			return;
	}

	if( COMPARE( Pos.x, 0, g_BlockManager.GetTileMax() ) && COMPARE( Pos.y, 0, g_BlockManager.GetHeightMax() ) && COMPARE( Pos.z, 0, g_BlockManager.GetTileMax() ) )
	{
		if( m_Pos != Pos )
		{
			//이전 위치의 선택을 해제
			ioBlock* pBlock = pWorld->GetMapBase()->GetBlockByTilePos( m_Pos );
			if( pBlock )
				pBlock->ChangeNone();
		}

		m_Pos = Pos;

		D3DXVECTOR3 vPos;
		TileCoordToWorldCoord( Pos.x, Pos.y, Pos.z, vPos.x, vPos.y, vPos.z );				

		//마법진만 표시되기 때문에 ioBlockWorld::ProcessBaseCharAlphaEvent() 등의 이벤트를 수행하기 위해서 위치를 이동
		pOwner->SetWorldPosition( vPos );		
		ioBlock* pBlock = pWorld->GetMapBase()->GetBlockByTilePos( m_Pos );
		if( pBlock )
		{
			pBlock->ChangeSelect();

			ioHashString szDummyCircle;
			pOwner->CreateMagicCircle( m_RemoveCircle, m_RemoveCircle, szDummyCircle, vPos, 0.0f );
			pOwner->SetMagicCirclePos( false, vPos );

			m_ActionState = AS_ENABLE_REMOVE;

			if( pBlock->GetProperty() )
			{
				m_dwCurrRemoveCode = pBlock->GetProperty()->GetCode();
			}
			m_nRemoveUniqueIndex = pBlock->GetUniqueIndex();

			if( eEvent == KE_EXCECUTE )
			{
				m_State = S_WAIT;
				g_BlockSetupManager.SendRemoveBlock( pBlock->GetUniqueIndex() );
			}
		}
		else
		{
			ioHashString szDummyCircle;
			pOwner->CreateMagicCircle( m_RemoveDisableCircle, m_RemoveDisableCircle, szDummyCircle, vPos, 0.0f );
			pOwner->SetMagicCirclePos( false, vPos );

			m_dwCurrRemoveCode = 0;
			if( eEvent == KE_EXCECUTE )
			{
				SystemChat( SCT_NOT_FIND_RMOVE_OBJ );
			}
		}
	}
}

bool ioConstructHouseSpecialState::SendSetUp()
{
	if( m_ActionState == AS_ENABLE_REMOVE )
	{
		m_State = S_WAIT;
		g_BlockSetupManager.SendSetUpBlock( m_dwCurrSetUpCode, m_Pos.x, m_Pos.y, m_Pos.z, (byte)m_eCurrRotate );
		return true;
	}

	return false;
}

bool ioConstructHouseSpecialState::SendRemove()
{
	if( m_ActionState == AS_ENABLE_REMOVE )
	{
		m_State = S_WAIT;
		g_BlockSetupManager.SendRemoveBlock( m_nRemoveUniqueIndex );
		return true;
	}

	return false;
}

ioConstructHouseSpecialState::KeyEvent ioConstructHouseSpecialState::CheckPosKey( ioBaseChar *pOwner, ioPlayStage* pStage, TilePos& Pos, int& nRotate )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( g_Input.IsKeyUp( KEY_D ) )
	{			
		return KE_EXCECUTE;
	}
	else
	{
		if( m_dwCurrKeyCheckTime + 100 < dwCurTime )
		{
			//좌/우		
			if( g_Input.IsKeyDown( KEY_RIGHTARROW ) && !g_Input.IsKeyDown( KEY_LEFTARROW ) )
			{
				m_dwCurrKeyCheckTime = dwCurTime;
				Pos.x += 1;
				return KE_RIGHT;
			}
			else if( !g_Input.IsKeyDown( KEY_RIGHTARROW ) && g_Input.IsKeyDown( KEY_LEFTARROW ) )
			{
				m_dwCurrKeyCheckTime = dwCurTime;
				Pos.x -= 1;
				return KE_LEFT;
			}

			//앞/뒤
			if( g_Input.IsKeyDown( KEY_DOWNARROW ) && !g_Input.IsKeyDown( KEY_UPARROW ) )
			{
				m_dwCurrKeyCheckTime = dwCurTime;
				Pos.z += 1;
				return KE_BACK;
			}
			else if( !g_Input.IsKeyDown( KEY_DOWNARROW ) && g_Input.IsKeyDown( KEY_UPARROW ) )
			{
				m_dwCurrKeyCheckTime = dwCurTime;
				Pos.z -= 1;
				return KE_FRONT;
			}

			//위/아래
			if( g_Input.IsKeyDown( KEY_A ) && !g_Input.IsKeyDown( KEY_S ) )
			{
				m_dwCurrKeyCheckTime = dwCurTime;
				Pos.y += 1;	
				return KE_UP;
			}
			else if( !g_Input.IsKeyDown( KEY_A ) && g_Input.IsKeyDown( KEY_S ) )
			{
				m_dwCurrKeyCheckTime = dwCurTime;
				Pos.y -= 1;
				return KE_DOWN;
			}

			//회전
			if( g_Input.IsKeyDown( KEY_W ) && !g_Input.IsKeyDown( KEY_E ) && !g_Input.IsKeyUp( KEY_F ) && !g_Input.IsKeyUp( KEY_Q ) && !g_Input.IsKeyUp( KEY_R ) )
			{
				m_dwCurrKeyCheckTime = dwCurTime;
				nRotate--;
				if( nRotate < 0 )
					nRotate = ROTATE_270;

				return KE_LEFT_ROTATE;
			}	
			else if( !g_Input.IsKeyDown( KEY_W ) && g_Input.IsKeyDown( KEY_E ) && !g_Input.IsKeyUp( KEY_F ) && !g_Input.IsKeyUp( KEY_Q ) && !g_Input.IsKeyUp( KEY_R ) )
			{
				m_dwCurrKeyCheckTime = dwCurTime;
				nRotate ++;
				if( nRotate == ROTATE_360 )
					nRotate = ROTATE_0;
				else if( nRotate > ROTATE_360 )
					nRotate = ROTATE_90;

				return KE_RIGHT_ROTATE;
			}
			else if( !g_Input.IsKeyDown( KEY_W ) && !g_Input.IsKeyDown( KEY_E ) && g_Input.IsKeyUp( KEY_F ) && !g_Input.IsKeyUp( KEY_Q ) && !g_Input.IsKeyUp( KEY_R ) )
			{
				m_dwCurrKeyCheckTime = dwCurTime;
				return KE_CHANGE_MODE;
			}
			else if( !g_Input.IsKeyDown( KEY_W ) && !g_Input.IsKeyDown( KEY_E ) && !g_Input.IsKeyUp( KEY_F ) && g_Input.IsKeyUp( KEY_Q ) && !g_Input.IsKeyUp( KEY_R ) )
			{
				m_dwCurrKeyCheckTime = dwCurTime;
				return KE_CHANGE_PREV_ITEM;
			}
			else if( !g_Input.IsKeyDown( KEY_W ) && !g_Input.IsKeyDown( KEY_E ) && !g_Input.IsKeyUp( KEY_F ) && !g_Input.IsKeyUp( KEY_Q ) && g_Input.IsKeyUp( KEY_R ) )
			{
				m_dwCurrKeyCheckTime = dwCurTime;
				return KE_CHANGE_NEXT_ITEM;
			}
		}
	}

	return KE_NONE;
}

void ioConstructHouseSpecialState::ClearSystemChatReserve()
{
	m_SystemChatType	= SCT_NONE;
	m_dwSystemChatTime	= 0;
}

void ioConstructHouseSpecialState::SystemChat( SystemChatType eType )
{
	if( m_SystemChatType != eType )
	{
		switch( eType )
		{
		case SCT_CANT_SETUP:
			g_ChatMgr.SetSystemMsg( "설치 가능한 영역이 아닙니다." );
			break;
		case SCT_NOT_FIND_SETUP_OBJ:
			g_ChatMgr.SetSystemMsg( "설치 가능한 오브젝트가 없습니다." );
			break;
		case SCT_NOT_FIND_RMOVE_OBJ:
			g_ChatMgr.SetSystemMsg( "제거 가능한 오브젝트가 없습니다." );
			break;
		case SCT_BLOCK_MAX:
			g_ChatMgr.SetSystemMsg( "최대치에 도달하여 더이상 설 치 할 수 없습니다." );
			break;
		}

		m_SystemChatType = eType;
		m_dwSystemChatTime = FRAMEGETTIME();
	}
}