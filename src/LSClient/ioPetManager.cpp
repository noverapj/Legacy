
#include "stdafx.h"

#include "ioPetManager.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "PetDefine.h"

#include "GUI/PetWnd.h"

template<> ioPetManager* Singleton< ioPetManager >::ms_Singleton = 0;

ioPetManager::ioPetManager( ioPlayStage *pStage )
{
	m_pPlayStage = pStage;
	m_CurPetInfoList.clear();
}

ioPetManager::~ioPetManager()
{
	m_CurPetInfoList.clear();
	ClearPetBaseInfo();
	DestroyPetList();
}

ioPetManager& ioPetManager::GetSingleton()
{
	return Singleton< ioPetManager >::GetSingleton();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//기본 데이터 읽기
void ioPetManager::LoadPetBaseInfo()
{
	ioINILoader_e kLoader( "config/sp2_pet.ini" );

	char szBuf[MAX_PATH] = "";
	char szTitle[MAX_PATH] = "";
	char szGroupName[MAX_PATH] = "";

	kLoader.SetTitle_e( "common_info" );
	int nGroupCnt = kLoader.LoadInt_e( "pet_group_cnt", 0 );

	for( int i=0; i < nGroupCnt; ++i )
	{
		PetBaseInfo kBaseInfo;

		wsprintf_e( szTitle, "pet_group%d", i );
		kLoader.LoadString( szTitle, "", szBuf, MAX_PATH );
		wsprintf_e( szGroupName, "config/%s", szBuf );

		kBaseInfo.m_PetPath = szGroupName;

		ioINILoader kSetPetLoader( szGroupName );
		kSetPetLoader.SetTitle_e( "common_info" );

		int iCnt = kSetPetLoader.LoadInt_e( "pet_cnt", 0 );
		for( int j=0; j < iCnt; ++j )
		{
			wsprintf_e( szTitle, "pet%d", j+1 );
			kBaseInfo.m_PetTitle = szTitle;

			kSetPetLoader.SetTitle( szTitle );
			
			int nPetCode = kSetPetLoader.LoadInt_e( "pet_code", 0 );
			kBaseInfo.m_PetTypeInfo.m_PetMainType = (PetMainType)kSetPetLoader.LoadInt_e( "pet_main_type", 0 );
			kBaseInfo.m_PetTypeInfo.m_nPetSubType = kSetPetLoader.LoadInt_e( "pet_sub_type", 0 );
			LoadModelInfo( kSetPetLoader, kBaseInfo.m_vPetModelInfoList );

			m_vPetBaseInfoList.insert( std::make_pair( nPetCode, kBaseInfo ) );
		}
	}
}

//모델 정보 읽기 (랭크별)
void ioPetManager::LoadModelInfo( ioINILoader &rkLoader, PetModelInfoList &vPetModelInfoList )
{
	char szBuf[MAX_PATH], szKey[MAX_PATH];
	vPetModelInfoList.clear();

	int nMaxModelInfo = rkLoader.LoadInt_e( "max_model_info", 0 );
	for ( int i=0; i<nMaxModelInfo; ++i )
	{
		PetModelInfo sPetModelInfo;
		
		wsprintf_e( szKey, "model_info%d_rank", i+1 );
		sPetModelInfo.m_PetRankType = (PetRankType)rkLoader.LoadInt( szKey, PRT_NONE );

		wsprintf_e( szKey, "model_info%d_model", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		sPetModelInfo.m_PetModel = szBuf;

		D3DXVECTOR3 vScale;
		wsprintf_e( szKey, "model_info%d_scale_x", i+1 );
		sPetModelInfo.m_vScale.x = rkLoader.LoadFloat( szKey, FLOAT1 );
		wsprintf_e( szKey, "model_info%d_scale_y", i+1 );
		sPetModelInfo.m_vScale.y = rkLoader.LoadFloat( szKey, FLOAT1 );
		wsprintf_e( szKey, "model_info%d_scale_z", i+1 );
		sPetModelInfo.m_vScale.z = rkLoader.LoadFloat( szKey, FLOAT1 );

		vPetModelInfoList.push_back( sPetModelInfo );
	}
}

//데이터 삭제
void ioPetManager::ClearPetBaseInfo()
{
	PetBaseInfoList::iterator Iter = m_vPetBaseInfoList.begin();
	for ( ; Iter!=m_vPetBaseInfoList.end(); ++Iter )
	{
		PetBaseInfo& sPetBaseInfo = Iter->second;
		sPetBaseInfo.m_vPetModelInfoList.clear();
	}
	m_vPetBaseInfoList.clear();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//펫 생성
ioPet* ioPetManager::CreatePetEntity( ioHashString szModelName, ioPlayMode *pMode, PetTypeInfo sPetType )
{
	ioEntityGroup *pGrp = m_pPlayStage->CreateEntityGrpAndSceneNode( szModelName, true );
	if( !pGrp )
	{
		LOG.PrintTimeAndLog( 0, "ioPetManager::CreatePet - %s Fail!", szModelName.c_str() );
		return NULL;
	}

	ioPet *pPet = NULL;
	switch ( (PetMainType)sPetType.m_PetMainType )
	{
	case PMT_DEFAULT:
		pPet = CreateDefaultPet( pGrp, pMode, sPetType.m_nPetSubType );
		break;
	}

	if ( pPet != NULL)
		m_PetList.push_back( pPet );

	return pPet;
}

//메인 타입에 따른 서브 타입 펫 생성
ioPet* ioPetManager::CreateDefaultPet( ioEntityGroup *pGrp, ioPlayMode *pMode, int nSubType )
{
	switch ( (DefaultPetSubType)nSubType )
	{
	case DPST_DEFAULT:
		return new ioDefaultPet( pGrp, pMode );
	}
	return NULL;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//현재 모든 펫 비우기
void ioPetManager::DestroyPetList()
{
	m_PetList.clear();
	m_pPlayStage->DestroyAllPet();
}

//엔드 상태 확인 후 제거
void ioPetManager::ClearDeadPet()
{
	PetList::iterator iter = m_PetList.begin();
	while( iter != m_PetList.end() )
	{
		ioPet *pPet = ToPet( *iter );
		if( pPet && !pPet->IsLive() )
			iter = m_PetList.erase( iter );
		else
			++iter;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//기본 펫 정보 얻기
bool ioPetManager::GetPetBaseInfo( int nPetCode, PetBaseInfo &rkInfo )
{
	if( m_vPetBaseInfoList.empty() || nPetCode == 0 )
		return false;

	PetBaseInfoList::iterator FindIter = m_vPetBaseInfoList.find( nPetCode );
	if ( FindIter != m_vPetBaseInfoList.end() )
	{
		rkInfo = FindIter->second;
		return true;
	}

	return false;
}

//해당 캐릭터의 펫 찾기
ioPet* ioPetManager::FindPet( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return NULL;

	int nPetCnt = m_PetList.size();
	for( int i=0; i < nPetCnt; i++ )
	{
		if( m_PetList[i]->GetOwnerName() == pOwner->GetCharName() )
			return m_PetList[i];
	}

	return NULL;
}

//해당 캐릭터의 펫 엔드 상태로 전환
void ioPetManager::SetPetDieState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	int nPetCnt = m_PetList.size();
	for( int i=0; i < nPetCnt; i++ )
	{
		if( m_PetList[i]->GetOwnerName() == pOwner->GetCharName() )
			m_PetList[i]->OnDeadState();
	}
}

//펫 모델 정보 얻기 (코드와 랭크에 따라)
bool ioPetManager::GetPetModelInfo( int nPetCode, PetRankType eCurPetRankType, PetModelInfo &sPetModelInfo )
{
	if( m_vPetBaseInfoList.empty() || nPetCode == 0 )
		return false;

	PetBaseInfoList::iterator FindIter = m_vPetBaseInfoList.find( nPetCode );
	if ( FindIter != m_vPetBaseInfoList.end() )
	{
		PetBaseInfo sPetBaseInfo = FindIter->second;
		PetModelInfoList vPetModelInfoList = sPetBaseInfo.m_vPetModelInfoList;

		int nInfoCnt = (int)vPetModelInfoList.size();
		for ( int j=0; j<nInfoCnt; ++j )
		{
			if (vPetModelInfoList[j].m_PetRankType == eCurPetRankType )
			{
				sPetModelInfo = vPetModelInfoList[j];
				return true;
			}
		}
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//펫 생성
ioPet* ioPetManager::CreatePet( const ioHashString &szOwnerName, const D3DXVECTOR3 &vTargetPos )
{
	if ( !m_pPlayStage )
		return NULL;

	ioPlayMode* pPlayMode = m_pPlayStage->GetPlayMode();
	if ( !pPlayMode )
		return NULL;

	char szTitle[MAX_PATH] = "";

	//주인 찾기
	ioBaseChar *pOwner = m_pPlayStage->GetBaseChar( szOwnerName );
	if ( !pOwner )
	{
		LOG.PrintTimeAndLog( 0, "ioPet* ioPetManager::CreatePet : GetBaseChar Fail" );
		return NULL;
	}

	//펫 엔드 상태로 만듬
	SetPetDieState( pOwner );

	//데이터 얻기
	CurPetInfo sCurPetInfo;
	if ( !GetCurPetInfo( szOwnerName, sCurPetInfo ) )
	{
		LOG.PrintTimeAndLog( 0, "ioPet* ioPetManager::CreatePet : GetCurPetInfo Fail" );
		return NULL;
	}

	//기본 정보 찾기
	PetBaseInfo kBaseInfo;
	if( !GetPetBaseInfo( sCurPetInfo.m_nPetCode, kBaseInfo ) )
	{
		LOG.PrintTimeAndLog( 0, "ioPet* ioPetManager::CreatePet : GetPetBaseInfo Fail" );
		return NULL;
	}

	ioINILoader kLoader;
	kLoader.SetFileName( kBaseInfo.m_PetPath.c_str() );
	kLoader.SetTitle( kBaseInfo.m_PetTitle.c_str() );

	//모델 정보 얻기
	PetModelInfo sPetModelInfo;	
	if ( !GetPetModelInfo( sCurPetInfo.m_nPetCode, sCurPetInfo.m_CurRankType, sPetModelInfo ) )
	{
		LOG.PrintTimeAndLog( 0, "ioPet* ioPetManager::CreatePet : GetPetModelInfo Fail" );
		return NULL;
	}

	char szModelName[MAX_PATH] = "";
	wsprintf_e( szModelName, "%s", sPetModelInfo.m_PetModel.c_str() );

	//엔티티 생성
	ioPet *pPet = CreatePetEntity( szModelName, pPlayMode, kBaseInfo.m_PetTypeInfo );

	//엔티티 생성 실패
	if( !pPet )
	{
		LOG.PrintTimeAndLog( 0, "ioPet* ioPetManager::CreatePet : Create Fail" );
		return NULL;
	}

	//데이터 읽기
	pPet->LoadProperty( kLoader );
	//기타 기본 설정
	pPet->SetShadowCastEnable( true );
	pPet->SetOwner( szOwnerName );

	ioEntityGroup *pGrp = pPet->GetGroup();
	if( pGrp )
	{
		ioPetAniEventHandler *pHandler = new ioPetAniEventHandler;
		pHandler->SetCreatePet( pPet );
		pGrp->SetAniEventHandler( pHandler );
	}

	//크기 및 생성 위치 설정
	D3DXVECTOR3 vScale = sPetModelInfo.m_vScale;
	D3DXVECTOR3 vPos = vTargetPos;
	if( pOwner )
	{
		if( vPos.y <= 0.0f )
		{
			const ioAxisAlignBox &rkWorldBox = pOwner->GetWorldAxisBox();
			vPos.y = rkWorldBox.GetCenter().y + rkWorldBox.GetExtents( 1 );
		}

		pPet->InitPet( vPos, vScale, sCurPetInfo.m_CurRankType );

		if ( !pPet->OnStartState() )
		{
			LOG.PrintTimeAndLog( 0, "ioPet* ioPetManager::CreatePet : OnStartState Fail" );
			pPet->SetLive( false );
		}
	}

	return pPet;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//펫 현재 정보 추가
void ioPetManager::AddCurPetInfo( ioHashString szCharName, CurPetInfo sCurPetInfo, bool bOnlyChange )
{
	//데이터 설정
	CurPetInfoList::iterator FindIter = m_CurPetInfoList.find( szCharName );
	//정보가 이미 있다면 변경
	if ( FindIter != m_CurPetInfoList.end() )
		FindIter->second = sCurPetInfo;
	//없다면 새로 추가
	else
	{
		if ( !bOnlyChange )
			m_CurPetInfoList.insert( std::make_pair( szCharName, sCurPetInfo ) );
	}
}

//펫 현재 정보 얻기
bool ioPetManager::GetCurPetInfo( ioHashString szCharName, CurPetInfo &sCurPetInfo )
{
	if ( szCharName.IsEmpty() || (int)m_CurPetInfoList.size() <= 0 )
		return false;

	CurPetInfoList::iterator FindIter = m_CurPetInfoList.find( szCharName );
	//정보가 있다면 반환
	if ( FindIter != m_CurPetInfoList.end() )
	{
		CurPetInfo sPetData = FindIter->second;
		sCurPetInfo = sPetData;
		return true;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//펫 동기화
void ioPetManager::OnPetSync( SP2Packet &rkPacket )
{
	ioHashString szCreator;
	rkPacket >> szCreator;

	if ( !m_pPlayStage )
		return;

	ioBaseChar *pCreator = m_pPlayStage->GetBaseChar( szCreator );
	if( !pCreator )
	{
		LOG.PrintTimeAndLog( 0, "void ioPetManager::OnPetSync : pCreator Fail" );
		return;
	}

	ioPet *pPet = FindPet( pCreator );
	if( pPet )
		pPet->ApplyPetSync( m_pPlayStage, rkPacket );
}

//펫 변경(슬롯만 변경)
void ioPetManager::OnPetChangeData( SP2Packet &rkPacket )
{
	TCPNetwork::MouseBusy( false );

	int nState;
	rkPacket >> nState;

	switch( nState )
	{
	case PET_CHANGE_OK:
		{
			int nSlotIndex = 0;
			bool bEquip = false;
			rkPacket >> nSlotIndex;
			rkPacket >> bEquip;

			ioUserPet *pPet = g_MyInfo.GetUserPet();
			if ( pPet )
			{
				pPet->SetPetEquip( nSlotIndex, bEquip );

				PetWnd *pPetWnd = dynamic_cast<PetWnd*>(g_GUIMgr.FindWnd( PET_WND ));
				if ( pPetWnd && pPetWnd->IsShow() )
				{
					if ( bEquip )
						pPetWnd->SetActivePet();
					else
						pPetWnd->SetInActivePet();
				}
			}
		}
		break;
	case PET_CHANGE_FAIL:
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		}
		break;
	}
}

//펫 장착(전투방)
void ioPetManager::OnPetEquipData( SP2Packet &rkPacket )
{
	ioHashString szCharName;
	int nPetCode, nPetRankType, nPetCurLevel;
	bool bEquip;

	rkPacket >> szCharName;
	rkPacket >> nPetCode >> nPetRankType >> nPetCurLevel;
	rkPacket >> bEquip;

	if ( !m_pPlayStage )
		return;

	ioBaseChar *pOwner = m_pPlayStage->GetBaseChar( szCharName );
	if ( !pOwner )
	{
		LOG.PrintTimeAndLog( 0, "void ioPetManager::OnPetEquipData : pOwner Fail" );
		return;
	}

	//활성화
	CurPetInfo sCurPetInfo;
	if ( bEquip )
	{
		sCurPetInfo.m_nPetCode = nPetCode;
		sCurPetInfo.m_CurRankType = (PetRankType)nPetRankType;
		sCurPetInfo.m_nCurLevel = nPetCurLevel;

		AddCurPetInfo( szCharName, sCurPetInfo, false );
		//펫 생성
		if ( !m_pPlayStage->CreatePet( szCharName, pOwner->GetWorldPosition(), false ) )
		{
			sCurPetInfo.m_nPetCode = 0;
			sCurPetInfo.m_CurRankType = PRT_NONE;
			sCurPetInfo.m_nCurLevel = 0;
			AddCurPetInfo( szCharName, sCurPetInfo, false );
			SetPetDieState( pOwner );
		}	
	}
	//비활성화
	else
	{
		sCurPetInfo.m_nPetCode = 0;
		sCurPetInfo.m_CurRankType = PRT_NONE;
		sCurPetInfo.m_nCurLevel = 0;
		AddCurPetInfo( szCharName, sCurPetInfo, false );
		SetPetDieState( pOwner );
	}	

	//유저정보노드의 펫 정보를 수정
	UserInfoNode *pUserInfoNode = g_UserInfoMgr.GetUserInfoNode( szCharName, false );
	if( pUserInfoNode )
	{
		pUserInfoNode->SetPetCode( nPetCode );
		pUserInfoNode->SetPetLevel( nPetCurLevel );
	}
	else
		LOG.PrintTimeAndLog( 0, "void ioPetManager::OnPetEquipData : pUserInfoNode Fail" );

}

//펫 변경(슬롯 제거)
void ioPetManager::OnPetRemoveData( SP2Packet &rkPacket )
{
	TCPNetwork::MouseBusy( false );

	int nState;
	rkPacket >> nState;

	switch( nState )
	{
	case PET_SELL_OK:
		{
			int nSlotIndex;
			rkPacket >> nSlotIndex;

			ioUserPet *pUserPet = g_MyInfo.GetUserPet();
			if ( pUserPet )
			{
				PetSlot sPetSlot;
				if ( pUserPet->GetPetSlot( nSlotIndex, sPetSlot ) )
				{
					int nPetCode, nPetRank, nPetLevel;
					nPetCode = sPetSlot.m_nPetCode;
					nPetRank = sPetSlot.m_PetRankType;
					nPetLevel = sPetSlot.m_nCurLevel;

					pUserPet->DeletePetSlotData( nSlotIndex );

					__int64 nMoney;
					rkPacket >> nMoney;
					g_MyInfo.SetMoney( nMoney );

					PetWnd *pPetWnd = dynamic_cast<PetWnd*>(g_GUIMgr.FindWnd( PET_WND ));
					if ( pPetWnd && pPetWnd->IsShow() )
						pPetWnd->SetRemovePet( nPetCode, nPetRank, nPetLevel );
				}
			}
		}
		break;
	case PET_SELL_FAIL:
		{
			int nErrorNum;
			rkPacket >> nErrorNum;

			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1), nErrorNum );
		}
		break;
	}
}

//펫 부화
void ioPetManager::OnPetAdd( SP2Packet &rkPacket )
{
	ioUserPet *pUserPet = g_MyInfo.GetUserPet();
	if( !pUserPet )
	{
		LOG.PrintTimeAndLog( 0, "void ioPetManager::OnPetAdd : pUserPet Fail" );
		return;
	}

	int nState;
	rkPacket >> nState;

	switch ( nState )
	{
	case PET_HATCH_OK:
		{
			int nPetCode, nPetRank, nSlotIndex, nPetLevel;
			rkPacket >> nSlotIndex >> nPetCode >> nPetRank >> nPetLevel;

			PetSlot sPetSlot;
			sPetSlot.m_nPetCode = nPetCode;
			sPetSlot.m_PetRankType = nPetRank;
			sPetSlot.m_nCurLevel = nPetLevel;
			sPetSlot.m_nIndex = nSlotIndex;

			pUserPet->AddPetSlotData( sPetSlot );

			MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
			if( pInvenWnd )
			{
				if ( pInvenWnd->IsShow() )
					pInvenWnd->OnPetEggResultInfo( nPetCode, nPetRank, nSlotIndex );

				//UI 갱신
				PetWnd *pPetWnd = dynamic_cast<PetWnd*>( g_GUIMgr.FindWnd(PET_WND) );
				if ( pPetWnd )
				{
					if ( pPetWnd->IsShow() )
					{
						pPetWnd->OnPetAddResultApply( sPetSlot.m_nIndex );
						pInvenWnd->GoToTop();
					}
					else
						pPetWnd->SetSelectPetSlot( sPetSlot.m_nIndex );
				}
			}				
		}
		break;
	case PET_HATCH_FAIL:
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		}
		break;
	case PET_PRESENT_OK:
		{
			int nPetCode, nPetRank, nSlotIndex, nPetLevel;
			rkPacket >> nSlotIndex >> nPetCode >> nPetRank >> nPetLevel;

			PetSlot sPetSlot;
			sPetSlot.m_nPetCode = nPetCode;
			sPetSlot.m_PetRankType = nPetRank;
			sPetSlot.m_nCurLevel = nPetLevel;
			sPetSlot.m_nIndex = nSlotIndex;

			pUserPet->AddPetSlotData( sPetSlot );

			//UI 갱신
			PetWnd *pPetWnd = dynamic_cast<PetWnd*>( g_GUIMgr.FindWnd(PET_WND) );
			if ( pPetWnd )
			{
				if ( pPetWnd->IsShow() )
					pPetWnd->OnPetAddResultApply( sPetSlot.m_nIndex );
				else
					pPetWnd->SetSelectPetSlot( sPetSlot.m_nIndex );
			}

			MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
			if( pInvenWnd && pInvenWnd->IsShow() )
				pInvenWnd->GoToTop();
		}
		break;
	}
}

//펫 경험치 증가
void ioPetManager::OnPetNurture( SP2Packet &rkPacket )
{
	TCPNetwork::MouseBusy( false );

	ioUserPet *pUserPet = g_MyInfo.GetUserPet();
	if( !pUserPet )
	{
		LOG.PrintTimeAndLog( 0, "void ioPetManager::OnPetNurture : pUserPet Fail" );
		return;
	}

	int nState;
	rkPacket >> nState;

	switch ( nState )
	{
	case PET_NURTURE_OK:
		{
			int nSlotIndex, nPetRank, nPetLevel, nCurExp;
			int nMaterialCode;

			rkPacket >> nSlotIndex >> nPetRank >> nPetLevel >> nCurExp;
			rkPacket >> nMaterialCode;

			PetSlot sPetSlot;
			if ( !pUserPet->GetPetSlot( nSlotIndex, sPetSlot ) )
			{
				LOG.PrintTimeAndLog( 0, "void ioPetManager::OnPetNurture : GetPetSlot Fail" );
				return;
			}

			//UI 설정 후
			PetWnd *pPetWnd = dynamic_cast<PetWnd*>( g_GUIMgr.FindWnd(PET_WND) );
			if ( pPetWnd && pPetWnd->IsShow() )
				pPetWnd->OnPetEatResultInfo( sPetSlot.m_nPetCode, nPetRank, sPetSlot.m_nCurLevel, nPetLevel, sPetSlot.m_CurExp, nCurExp );

			//데이터 변경
			sPetSlot.m_PetRankType = nPetRank;
			sPetSlot.m_CurExp = nCurExp;
			sPetSlot.m_nCurLevel = nPetLevel;

			pUserPet->SetPetSlot( sPetSlot );

			if ( pPetWnd && pPetWnd->IsShow() )
				pPetWnd->OnPetEatResultApply( sPetSlot.m_nIndex );
		}
		break;
	case PET_NURTURE_FAIL:
		{
			int nErrorNum;
			rkPacket >> nErrorNum;

			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1), nErrorNum );
		}
		break;
	}
}

//펫 레벨 증가 (전투방)
void ioPetManager::OnPetNurtureLevelUp( SP2Packet &rkPacket )
{
	ioHashString szCharName;
	int nPetCode, nPetRankType, nPetCurLevel;

	rkPacket >> szCharName;
	rkPacket >> nPetCode >> nPetRankType >> nPetCurLevel;

	if ( !m_pPlayStage )
		return;

	ioBaseChar *pOwner = m_pPlayStage->GetBaseChar( szCharName );
	if ( !pOwner )
	{
		LOG.PrintTimeAndLog( 0, "void ioPetManager::OnPetNurtureLevelUp : pOwner Fail" );
		return;
	}

	CurPetInfo sCurPetInfo;
	if ( nPetCode <= 0 || nPetRankType <= 0 )
	{
		LOG.PrintTimeAndLog( 0, "void ioPetManager::OnPetNurtureLevelUp : sCurPetInfo Fail" );
		return;
	}
	else
	{
		sCurPetInfo.m_nPetCode = nPetCode;
		sCurPetInfo.m_CurRankType = (PetRankType)nPetRankType;
		sCurPetInfo.m_nCurLevel = nPetCurLevel;
	}

	//데이터 설정
	AddCurPetInfo( szCharName, sCurPetInfo, true );
}

//펫 합성
void ioPetManager::OnPetCompound( SP2Packet &rkPacket )
{
	TCPNetwork::MouseBusy( false );

	ioUserPet *pUserPet = g_MyInfo.GetUserPet();
	if( !pUserPet )
	{
		LOG.PrintTimeAndLog( 0, "void ioPetManager::OnPetCompound : pUserPet Fail" );
		return;
	}

	int nState;
	rkPacket >> nState;

	switch ( nState )
	{
	case PET_COMPOUND_OK:
		{
			int nNewPetCode, nNewPetRank, nNewPetSlot, nNewPetLevel;
			int nTargetSlot, nMaterialSlot;
			rkPacket >> nNewPetSlot >> nNewPetCode >> nNewPetRank >> nNewPetLevel;
			rkPacket >> nTargetSlot >> nMaterialSlot;

			//랭크업인지 새로운 펫인지 확인
			PetSlot sTargetSlot;
			if ( !pUserPet->GetPetSlot( nTargetSlot, sTargetSlot ) )
			{
				LOG.PrintTimeAndLog( 0, "void ioPetManager::OnPetCompound : GetPetSlot Fail" );
				return;
			}

			bool bResultType = false;
			if ( sTargetSlot.m_nPetCode == nNewPetCode )
			{
				//코드가 같을때만 랭크업
				//랭크가 높아졌다면 랭크업
				if ( sTargetSlot.m_PetRankType < nNewPetRank )
					bResultType = true;
				else
					bResultType = false;
			}
			else
				bResultType = false;

			//펫 슬롯 삭제 후
			if ( !pUserPet->DeletePetSlotData( nTargetSlot ) ||
				!pUserPet->DeletePetSlotData( nMaterialSlot ) )
			{
				LOG.PrintTimeAndLog( 0, "void ioPetManager::OnPetCompound : DeletePetSlotData Fail" );
				return;
			}			

			//새로운 펫 추가
			PetSlot sNewPetSlot;
			sNewPetSlot.m_nPetCode = nNewPetCode;
			sNewPetSlot.m_PetRankType = nNewPetRank;
			sNewPetSlot.m_nIndex = nNewPetSlot;
			sNewPetSlot.m_nCurLevel = nNewPetLevel;

			pUserPet->AddPetSlotData( sNewPetSlot );

			//UI 갱신
			PetWnd *pPetWnd = dynamic_cast<PetWnd*>( g_GUIMgr.FindWnd(PET_WND) );
			if ( pPetWnd && pPetWnd->IsShow() )
			{
				pPetWnd->OnPetCompoundResultInfo( sNewPetSlot.m_nPetCode, sNewPetSlot.m_PetRankType, bResultType );
				pPetWnd->OnPetCompoundResultApply( sNewPetSlot.m_nIndex );
			}
		}
		break;
	case PET_COMPOUND_FAIL:
		{
			int nErrorNum;
			rkPacket >> nErrorNum;

			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1), nErrorNum );
		}
		break;
	}
}