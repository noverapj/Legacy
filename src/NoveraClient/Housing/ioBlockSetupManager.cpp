#include "stdafx.h"

#include "BlockDefine.h"
#include "ioBlockSetupManager.h"
#include "ioBlockWorld.h"
#include "ioBlockEventParameter.h"
#include "ioUserBlockStorage.h"

#include "../ioPlayStage.h"
#include "../ioBaseChar.h"
#include "../StateClass/ioConstructHouseSpecialState.h"
#include "../ioEtcItem.h"
#include "../ioUserEtcItem.h"

#include "../GUI/ioHousingBlockSetupWnd.h"

template<> ioBlockSetupManager* Singleton< ioBlockSetupManager >::ms_Singleton = 0;

ioBlockSetupManager::ioBlockSetupManager()
{
}

ioBlockSetupManager::~ioBlockSetupManager()
{
}

ioBlockSetupManager& ioBlockSetupManager::GetSingleton()
{
	return Singleton< ioBlockSetupManager >::GetSingleton();
}

void ioBlockSetupManager::Initialize( ioPlayStage* pStage, BlockModeType eType )
{
	m_pStage				= pStage;
	m_BlockModeType			= eType;
	m_nSetUpItemListIndex	= 0;

	m_SetUpAdminUser.Clear();

	ioINILoader_e kLoader( "config/housing/sp2_block.ini" );
	kLoader.SetTitle_e( "common" );

	m_nBlockLimitCount = kLoader.LoadInt( "block_limit", 3000 );
}

bool ioBlockSetupManager::IsSetUpState()
{
	ioBaseChar* pOwner = m_pStage->GetBaseChar( g_MyInfo.GetPublicID() );
	if( pOwner && pOwner->GetState() == CS_CONSTRUCT_HOUSE )
	{
		return true;
	}

	return false;
}

ioConstructHouseSpecialState* ioBlockSetupManager::GetAdminState()
{
	ioBaseChar* pOwner = m_pStage->GetBaseChar( m_SetUpAdminUser );
	if( pOwner )
	{
		ioConstructHouseSpecialState* pState = dynamic_cast<ioConstructHouseSpecialState*>( pOwner->GetSpecialState() );
		if( pState )
		{
			return pState;
		}
	}

	return NULL;
}

void ioBlockSetupManager::HousingSetUpProcess()
{
	if( IsSetUpState() )
		EndSetUpState();
	else
		BeginSetUpState();
}

void ioBlockSetupManager::BeginSetUpState()
{
	if( g_GUIMgr.IsShow( BRIEFING_WND ) ) 
		return;

	if( !m_pStage )
		return;

	ioBaseChar* pOwner = m_pStage->GetOwnerChar();
	if( !pOwner )
	{
		LOG.PrintTimeAndLog( 0, "%s pOwner == NULL.", __FUNCTION__ );
		return;
	}

	if( !pOwner->IsNeedProcess() )
	{
		LOG.PrintTimeAndLog( 0, "%s pOwner Not NeedProess", __FUNCTION__ );
		return;
	}

	if( pOwner->GetState() == CS_FISHING || pOwner->GetState() == CS_EXCAVATING )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "낚시나 탐사 중에는 사용 할 수 없습니다." );
		return;
	}

	// 캐릭터 상태 체크
	if( pOwner->GetState() != CS_DELAY || pOwner->IsCatchMode() || pOwner->IsPrisonerMode() )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "정지해 있는 상태에서만 편집 할 수 있습니다." );
		return;
	}
	
	// Buff 체크
	if( pOwner->HasBuff( BT_LIMIT_MOVE ) || pOwner->HasBuff( BT_LIMIT_ACTION ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "정지해 있는 상태에서만 편집 할 수 있습니다." );
		return;
	}

	// 아이템 체크
	if( pOwner->IsHasCrownByAllMode() )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "왕관을 벗어야만 편집 할 수 있습니다." );
		return;
	}

	// 광장 체크
	if( pOwner->GetCreator() == NULL || pOwner->GetCreator()->GetBlockWorld() == NULL )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "편집 할 수 없는 모드 입니다." );
		return;
	}

	pOwner->SetState( CS_CONSTRUCT_HOUSE );

	TCPNetwork::MouseBusy( true );
	SP2Packet kPacket( CTPK_CONSTRUCT_MODE );
	kPacket << (byte)m_BlockModeType;
	kPacket << (byte)CONSTRUCT_ON;
	TCPNetwork::SendToServer( kPacket );
}

void ioBlockSetupManager::ApplySetUpState()
{
	TCPNetwork::MouseBusy( false );

	ioBaseChar* pOwner = m_pStage->GetBaseChar( m_SetUpAdminUser );
	if( pOwner )
	{
		if( pOwner->IsNeedProcess() )
		{
			ioConstructHouseSpecialState* pState = dynamic_cast<ioConstructHouseSpecialState*>( pOwner->GetSpecialState() );
			if( pState )
			{
				pState->ChangeToSetUp( pOwner, pOwner->GetCreator() );
				if( m_SetUpAdminUser == g_MyInfo.GetPublicID() )
				{
					ioHousingBlockSetupWnd* pWnd = dynamic_cast<ioHousingBlockSetupWnd*>( g_GUIMgr.FindWnd( HOUSING_BLOCK_SETUP_WND ) );
					if( pWnd )
					{
						pWnd->SetBlockModeType( m_BlockModeType );
						pWnd->ShowWnd();
					}

					g_GUIMgr.HideWnd( HELP_WND );
					g_GUIMgr.HideWnd( SKILL_LIST_WND );
					g_GUIMgr.HideWnd( ACCESSORY_SKILL_LIST_WND );
					g_GUIMgr.HideWnd( MINI_SOLDIER_SELECT_WND );					
					g_GUIMgr.HideWnd( MESSENGER_WND );
				}
				return;				
			}
		}
		else
		{
			pOwner->SetState( CS_CONSTRUCT_HOUSE );
			return;
		}
	}

	//설치모드로 전환이 안됬다면 권한 반환
	g_BlockSetupManager.EndSetUpState();
}

void ioBlockSetupManager::EndSetUpState()
{
	if( m_pStage->HasCameraFixedStyle( CAMERA_STYLE_HOUSING ) )
		m_pStage->ClearCameraModeFixed( CAMERA_STYLE_HOUSING );

	ioBaseChar* pOwner = m_pStage->GetBaseChar( m_SetUpAdminUser );
	if( pOwner && pOwner->IsNeedProcess() && pOwner->GetState() == CS_CONSTRUCT_HOUSE )
	{
		TCPNetwork::MouseBusy( true );
		SP2Packet kPacket( CTPK_CONSTRUCT_MODE );
		kPacket << (byte)m_BlockModeType;
		kPacket << (byte)CONSTRUCT_OFF;
		TCPNetwork::SendToServer( kPacket );
	}
}

void ioBlockSetupManager::ApplyEndSetUpState()
{
	TCPNetwork::MouseBusy( false );

	if( m_SetUpAdminUser == g_MyInfo.GetPublicID() )
	{
		ioBaseChar* pOwner = m_pStage->GetBaseChar( m_SetUpAdminUser );
		if( pOwner )
		{
			pOwner->SetState( CS_DELAY );
		}

		g_GUIMgr.ShowWnd( HELP_WND );
		g_GUIMgr.ShowWnd( SKILL_LIST_WND );
		g_GUIMgr.ShowWnd( ACCESSORY_SKILL_LIST_WND );
		g_GUIMgr.ShowWnd( MINI_SOLDIER_SELECT_WND );
	}
	else
	{
		ioBaseChar* pOwner = m_pStage->GetBaseChar( g_MyInfo.GetPublicID() );
		if( pOwner && pOwner->GetState() == CS_CONSTRUCT_HOUSE )
		{
			pOwner->SetState( CS_DELAY );
		}
	}
}

void ioBlockSetupManager::SendSetUpBlock( int nItemCode, int x, int y, int z, byte rotate )
{
	TCPNetwork::MouseBusy( true );
	SP2Packet kPacket( CTPK_CONSTRUCT_BLOCK );
	kPacket << (byte)m_BlockModeType;
	kPacket << nItemCode;
	kPacket << x + (z * g_BlockManager.GetTileMax() );
	kPacket << y;
	kPacket << rotate;
	TCPNetwork::SendToServer( kPacket );
}

void ioBlockSetupManager::SendRemoveBlock( __int64 nUniqueIndex )
{
	TCPNetwork::MouseBusy( true );
	SP2Packet kPacket( CTPK_RETRIEVE_BLOCK );
	kPacket << (byte)m_BlockModeType;
	kPacket << nUniqueIndex;
	TCPNetwork::SendToServer( kPacket );
}

void ioBlockSetupManager::ChangeToSetUp()
{
	ioBaseChar* pOwner = m_pStage->GetBaseChar( m_SetUpAdminUser );
	if( pOwner )
	{
		ioConstructHouseSpecialState* pState = dynamic_cast<ioConstructHouseSpecialState*>( pOwner->GetSpecialState() );
		if( pState )
			pState->ChangeToSetUp( pOwner, m_pStage );
	}
}

void ioBlockSetupManager::ChangeToRemove()
{
	ioBaseChar* pOwner = m_pStage->GetBaseChar( m_SetUpAdminUser );
	if( pOwner )
	{
		ioConstructHouseSpecialState* pState = dynamic_cast<ioConstructHouseSpecialState*>( pOwner->GetSpecialState() );
		if( pState )
			pState->ChangeToRemove( pOwner, m_pStage );
	}
}

bool ioBlockSetupManager::ProcessTCPPacket( SP2Packet &rkPacket )
{
	switch( rkPacket.GetPacketID() )
	{
	case STPK_CONSTRUCT_MODE:
		OnSetUpState( rkPacket );
		return true;
	case STPK_CONSTRUCT_BLOCK:
		OnSetUpBlock( rkPacket );
		return true;
	case STPK_RETRIEVE_BLOCK:
		OnRemoveBlock( rkPacket );
		return true;
	}

	return false;
}

bool ioBlockSetupManager::ProcessUDPPacket( SP2Packet &rkPacket )
{
	return false;
}

void ioBlockSetupManager::Process()
{
}

void ioBlockSetupManager::OnSetUpState( SP2Packet &rkPacket )
{
	TCPNetwork::MouseBusy( false );

	byte eModeType;
	rkPacket >> eModeType;

	byte eResult;
	rkPacket >> eResult;

	switch( eResult )
	{
	case CONSTRUCT_MODE_CHANGE_SUCCESS:
		{
			byte eType;
			rkPacket >> eType;

			ioHashString szName;
			rkPacket >> szName;

			ioBaseChar* pChar = NULL;
			if( m_pStage )
				pChar = m_pStage->GetBaseChar( szName );

			m_SetUpAdminUser = szName;

			if( pChar )
			{
				switch( eType )
				{
				case CONSTRUCT_ON:
					{
						ApplySetUpState();
					}
					break;
				case CONSTRUCT_OFF:
					{
						ApplyEndSetUpState();
						m_SetUpAdminUser.Clear();
					}
					break;
				}
			}
		}
		break;
	case CONSTRUCT_PROGRESSING:
		{
			switch( eModeType )
			{
			case BMT_GUILD:
				g_GUIMgr.SetMsgBox( MB_OK, NULL, "지금 다른 길드원이 길드를 꾸미고 있는 중이니,#잠시 기다렸다가 시도해 주세요." );
				break;
			default:
				g_GUIMgr.SetMsgBox( MB_OK, NULL, "꾸미기 진행 에러" );
				break;
			}
			ApplyEndSetUpState();
		}
		break;
	case CONSTRUCT_AUTH_NONE:
		{
			switch( eModeType )
			{
			case BMT_GUILD:
				g_GUIMgr.SetMsgBox( MB_OK, NULL, "길드장이 지정한 유저만#꾸미기가 가능합니다." );
				break;
			case BMT_PERSONAL:
				g_GUIMgr.SetMsgBox( MB_OK, NULL, "개인본부 소유주만#꾸미기가 가능합니다." );
				break;
			default:
				g_GUIMgr.SetMsgBox( MB_OK, NULL, "꾸미기 권한이 없습니다." );
				break;
			}
			ApplyEndSetUpState();
		}
		break;
	case CONSTRUCT_MODE_EXCEPTION:
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, "꾸미기 상태로 전환 할 수 없는 모드입니다." );
			ApplyEndSetUpState();
		}
		break;
	}
}

void ioBlockSetupManager::OnSetUpBlock( SP2Packet &rkPacket )
{
	TCPNetwork::MouseBusy( false );

	byte eModeType;
	rkPacket >> eModeType;
	
	byte eRes;
	rkPacket >> eRes;

	switch( eRes )
	{
	case CONSTRUCT_BLOCK_SUCCESS:
		{
			ioHashString szName;
			rkPacket >> szName;

			__int64 nUniqueID;
			rkPacket >> nUniqueID;

			DWORD dwCode;
			rkPacket >> dwCode;

			int nPos;
			rkPacket >> nPos;

			int nLevel;
			rkPacket >> nLevel;

			byte Rotate;
			rkPacket >> Rotate;	

			if( nUniqueID != 0 )
			{				
				if( m_pStage )
				{
					//월드에 블럭 설치
					ioBlockWorld* pWorld = m_pStage->GetBlockWorld();
					if( pWorld )
					{
						TilePos Pos( nPos % g_BlockManager.GetTileMax(), nLevel, nPos / g_BlockManager.GetTileMax() );
						float fRotate = Rotate * ROTATE_VALUE;

						pWorld->CreateBlock( dwCode, Pos, fRotate, nUniqueID );
					}
					
					//인벤토리에서 갯수 차감
					ioUserBlockStorage* pStorage = NULL;
					switch( eModeType )
					{
					case BMT_GUILD:
						pStorage = g_App.GetGuildStorage();
						break;
					case BMT_PERSONAL:
						pStorage = g_App.GetHomeStorage();
						break;
					}

					if( pStorage )					
						pStorage->DecreaseItem( dwCode );

					//설치모드 상태 변경
					ioBaseChar* pChar = m_pStage->GetBaseChar( szName );
					if( pChar )
					{
						ioConstructHouseSpecialState* pState = ToConstructHouseSpecialState( pChar->GetSpecialState() );
						if( pState )
							pState->ChangeToSetUp( pChar, m_pStage );
					}
				}
			}
			else
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, "설치 할 수 없습니다." );
				EndSetUpState();
			}
		}
		break;
	case CONSTRUCT_POS_INVALID:
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, "잘못된 위치 입니다." );
			EndSetUpState();
		}
		break;
	case CONSTRUCT_BLOCK_SHORTAGE:
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, "수량이 부족 합니다." );
			EndSetUpState();
		}
		break;
	case CONSTRUCT_BLOCK_EXCEPTION:
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, "꾸미기 아이템 설치 예외 오류" );
			EndSetUpState();
		}
		break;
	case CONSTRUCT_BLOCK_MAX_COUNT:
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, "최대치에 도달하여 더이상 설 치 할 수 없습니다." );
			EndSetUpState();
		}
		break;
	}
}

void ioBlockSetupManager::OnRemoveBlock( SP2Packet &rkPacket )
{
	TCPNetwork::MouseBusy( false );

	byte eModeType;
	rkPacket >> eModeType;

	byte eRes;
	rkPacket >> eRes;

	switch( eRes )
	{
	case RETRIEVE_BLOCK_SUCCESS:
		{
			ioHashString szName;
			rkPacket>> szName;

			__int64 nUniqueIndex;
			rkPacket >> nUniqueIndex;

			ioBaseChar* pChar = NULL;
			if( m_pStage )
			{
				ioBlock* pBlock = g_BlockManager.GetBlock( nUniqueIndex );
				ioBlockWorld* pWorld = m_pStage->GetBlockWorld();
				if( pWorld && pBlock )
				{					
					//인벤토리에서 갯수 증가
					if( pBlock->GetProperty() )
					{
						ioUserBlockStorage* pStorage = NULL;
						switch( eModeType )
						{
						case BMT_GUILD:
							pStorage = g_App.GetGuildStorage();
							break;
						case BMT_PERSONAL:
							pStorage = g_App.GetHomeStorage();
							break;
						}

						if( pStorage )
						{
							pStorage->IncreaseItem( pBlock->GetProperty()->GetCode() );
							pStorage->UpdateInvenBlockItem();
						}
					}

					pWorld->DestroyBlock( pBlock->GetTilePos() );
				}

				pChar = m_pStage->GetBaseChar( szName );
				if( pChar )
				{
					ioConstructHouseSpecialState* pState = ToConstructHouseSpecialState( pChar->GetSpecialState() );
					if( pState )
						pState->ChangeToRemove( pChar, m_pStage );
				}
			}
		}
		break;
	case RETRIEVE_BLOCK_NONE_INFO:
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, "일치하는 꾸미기 아이템을 찾을 수 없습니다." );
			EndSetUpState();
		}
		break;
	case RETRIEVE_BLOCK_EXCEPTION:
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, "꾸미기 아이템 삭제 예외 오류" );
			EndSetUpState();
		}
		break;
	}
}