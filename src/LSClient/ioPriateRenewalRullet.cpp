#include "StdAfx.h"
#include "ioPriateRenewalRullet.h"
#include "GUI/PriateRulletRenewalWnd.h"


ioPriateRenewalRullet::ioPriateRenewalRullet(void)
{
	m_iCurrEnableRewardTurnNumber = 0;
	m_bUsingOakPriate = false;
	ClearRullet();
}

ioPriateRenewalRullet::~ioPriateRenewalRullet(void)
{
}

void ioPriateRenewalRullet::ClearRullet()
{
	for( int i = 0; i < MAX_RENEWAL_RULLET_COUNT; ++i )
		m_RoulletTable[i] = 0;
}

void ioPriateRenewalRullet::Initialize()
{
	ioINILoader_e kLoader( "config/sp2_oak_barrel.ini" );
	kLoader.SetTitle_e( "common" );
	m_iSwordCode = kLoader.LoadInt( "sword_etc_code", 0 );

	//보상 테이블
	m_RewardInfoList.clear();
	LoadReward( kLoader );

	m_DevelopKFaceTable.clear();
	LoadDevelopKFace( kLoader );
	
}

void ioPriateRenewalRullet::LoadReward( ioINILoader& rkLoader )
{
	char szBuf[MAX_PATH];
	rkLoader.SetTitle_e( "reward_rate" );
	int iRewardMaxCount = rkLoader.LoadInt( "reward_max", 0 );
	m_iMaxEnableRewardTurnNumber = iRewardMaxCount;
	for( int i = 0; i < iRewardMaxCount; ++i )
	{
		PriateRenewalRewardInfo Reward;
		wsprintf_e( szBuf, "reward%d_element_reward", i + 1 );
		Reward.m_iMax = rkLoader.LoadInt( szBuf, 0 );

		wsprintf_e( szBuf, "reward%d_element_cnt", i + 1 );
		int iRewardElementMaxCount = rkLoader.LoadInt( szBuf, 0 );
		for ( int j=0 ; j < iRewardElementMaxCount ; ++j )
		{
			PriateRenewalPresentData Info;
			wsprintf_e( szBuf, "reward%d_element%d_type", i + 1, j + 1 );
			Info.m_eType = rkLoader.LoadInt( szBuf, 0 );
			wsprintf_e( szBuf, "reward%d_element%d_value1", i + 1, j + 1 );
			Info.m_iVaule1 = rkLoader.LoadInt( szBuf, 0 );
			wsprintf_e( szBuf, "reward%d_element%d_value2", i + 1, j + 1 );
			Info.m_iVaule2 = rkLoader.LoadInt( szBuf, 0 );

			SafeSprintf( szBuf, sizeof( szBuf ), "%s %s", g_PresentMgr.GetPresentValue1Text( Info.m_eType, Info.m_iVaule1, Info.m_iVaule2 ).c_str(),
				g_PresentMgr.GetPresentValue2Text( Info.m_eType, Info.m_iVaule1, Info.m_iVaule2 ).c_str() );
			Info.m_Name			= szBuf;
			Info.m_Icon			= g_PresentMgr.GetPresentIconText( Info.m_eType, Info.m_iVaule1, Info.m_iVaule2 );
			Info.m_SubIcon = g_PresentMgr.GetPresentSubIconText( Info.m_eType, Info.m_iVaule1, Info.m_iVaule2 );
			Reward.m_PresentDataList.push_back( Info );
		}
		m_RewardInfoList.push_back( Reward );
	}
}

void ioPriateRenewalRullet::LoadDevelopKFace( ioINILoader& rkLoader )
{
	char szBuf[MAX_PATH];
	char szKey[MAX_PATH];

	rkLoader.SetTitle_e( "Face" );
	int nMax = rkLoader.LoadInt( "face_max", 0 );
	for( int i = 0; i < nMax; ++i )
	{
		DevelopKFace kFace;

		wsprintf_e( szKey, "face%d_sprite_max", i + 1 );
		int nFaceMax = rkLoader.LoadInt( szKey, 0 );
		for( int j = 0; j < nFaceMax; ++j )
		{
			wsprintf_e( szKey, "face%d_sprite%d", i + 1, j + 1 );
			rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
			kFace.m_FaceName.push_back( szBuf );
		}

		wsprintf_e( szKey, "face%d_bubble_duraiton", i + 1 );
		kFace.m_nBubbleTime = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "face%d_bubble_max", i + 1 );
		int nBubbleMax = rkLoader.LoadInt( szKey, 0 );
		for( int nBubble = 0; nBubble < nBubbleMax; ++nBubble )
		{
			wsprintf_e( szKey, "face%d_bubble_ment%d", i + 1, nBubble + 1 );
			rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
			kFace.m_BubbleMentList.push_back( szBuf );
		}

		wsprintf_e( szKey, "face%d_animation_time", i + 1 );
		kFace.m_nAnimationTime = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "face%d_hole", i + 1 );
		kFace.m_nMax = rkLoader.LoadInt( szKey, 0 );

		m_DevelopKFaceTable.push_back( kFace );
	}
}

int ioPriateRenewalRullet::GetSwordCount()
{
	ioUserEtcItem* pInven = g_MyInfo.GetUserEtcItem();
	if( pInven == NULL )
		return 0;

	ioUserEtcItem::ETCITEMSLOT rkEtcItem;
	if( pInven->GetEtcItem( m_iSwordCode, rkEtcItem ) )
		return rkEtcItem.m_iValue1;

	return 0;
}

int ioPriateRenewalRullet::GetFaceIndex() const
{
	int Index = 0;
	for( auto iter = m_DevelopKFaceTable.begin(); iter != m_DevelopKFaceTable.end(); ++iter )
	{
		const DevelopKFace& kFace = *iter;
		if( kFace.m_nMin <= m_iCurrEnableRewardTurnNumber && kFace.m_nMax >= m_iCurrEnableRewardTurnNumber )
		{
			return Index;
		}
		else
		{
			Index++;
		}
	}
	return -1;
}

int ioPriateRenewalRullet::GetFaceIndex( int iEnableRewardTurnNumber ) const
{
	int Index = 0;
	for( auto iter = m_DevelopKFaceTable.begin(); iter != m_DevelopKFaceTable.end(); ++iter )
	{
		const DevelopKFace& kFace = *iter;
		if( kFace.m_nMin <= iEnableRewardTurnNumber && kFace.m_nMax >= iEnableRewardTurnNumber )
		{
			return Index;
		}
		else
		{
			Index++;
		}
	}
	return -1;
}

DWORD ioPriateRenewalRullet::GetCurrFaceTime() const
{
	for( auto iter = m_DevelopKFaceTable.begin(); iter != m_DevelopKFaceTable.end(); ++iter )
	{
		const DevelopKFace& kFace = *iter;
		if( kFace.m_nMin <= m_iCurrEnableRewardTurnNumber && kFace.m_nMax >= m_iCurrEnableRewardTurnNumber )
		{
			return kFace.m_nAnimationTime;
		}
	}

	return 0;
}

void ioPriateRenewalRullet::GetCurrFaceName( ioHashStringVec& szFaceList ) const
{
	for( auto iter = m_DevelopKFaceTable.begin(); iter != m_DevelopKFaceTable.end(); ++iter )
	{
		const DevelopKFace& kFace = *iter;
		if( kFace.m_nMin <= m_iCurrEnableRewardTurnNumber && kFace.m_nMax >= m_iCurrEnableRewardTurnNumber )
		{
			szFaceList = kFace.m_FaceName;
		}
	}
}

void ioPriateRenewalRullet::GetCurrBubble( ioHashStringVec& BubbleList, int& nBubbleTime )
{
	for( auto iter = m_DevelopKFaceTable.begin(); iter != m_DevelopKFaceTable.end(); ++iter )
	{
		const DevelopKFace& kFace = *iter;
		if( kFace.m_nMin <= m_iCurrEnableRewardTurnNumber && kFace.m_nMax >= m_iCurrEnableRewardTurnNumber )
		{
			BubbleList		= kFace.m_BubbleMentList;
			nBubbleTime		= kFace.m_nBubbleTime;
		}
	}
}

int ioPriateRenewalRullet::GetRulletStickedCount()
{
	int nStickedCount = 0;
	for( int i = 0; i < MAX_RENEWAL_RULLET_COUNT; ++i )
	{
		if( 0 < m_RoulletTable[i] )
			nStickedCount++;
	}

	return nStickedCount;
}

PriateRenewalRewardInfo* ioPriateRenewalRullet::GetReward( int Index )
{
	if( COMPARE( Index, 0, (int)m_RewardInfoList.size() ) )
	{
		return &m_RewardInfoList[Index];
	}

	return NULL;
}

PriateRenewalRewardInfo* ioPriateRenewalRullet::GetCurrReward()
{
	for( int i = 0; i < (int)m_RewardInfoList.size(); ++i  )
	{
		if( m_iCurrEnableRewardTurnNumber <= m_RewardInfoList[i].m_iMax )
			return &m_RewardInfoList[i];
	}

	return NULL;
}

void ioPriateRenewalRullet::ApplyRenewalOakPacket( SP2Packet &rkPacket )
{
	switch( rkPacket.GetPacketID() )
	{
	case STPK_OAK_BARREL_GET_INFO:
		RenewalOakInfo( rkPacket );
		break;
	case STPK_OAK_BARREL_USE_SWORD:
		RenewalOakSwordUse( rkPacket );
		break;
	case STPK_OAK_BARREL_GET_REWARD:
		RenewalOakGetReward( rkPacket );
		break;
	}
}

void ioPriateRenewalRullet::RenewalOakInfo( SP2Packet &rkPacket )
{
	TCPNetwork::MouseBusy( false );
	BYTE btCurrEnableRewardTurnNumber;
	//사용 여부
	rkPacket >> m_bUsingOakPriate;
	//사용 안하면 패킷을 더 받을 필요가 없다 ㅎ
	if ( !m_bUsingOakPriate )
	{
		if( g_TCPSocketMgr.IsTestZone() )
			LOG.PrintTimeAndLog( 0, "오크통 비활성" );
		return;
	}

	rkPacket >> btCurrEnableRewardTurnNumber;
	m_iCurrEnableRewardTurnNumber = (int)btCurrEnableRewardTurnNumber;

	for( int i = 0; i < MAX_RENEWAL_RULLET_COUNT; ++i )
	{
		BYTE btValue;
		rkPacket >> btValue;
		m_RoulletTable[i] = (int)btValue;
	}

	if( g_TCPSocketMgr.IsTestZone() )
		LOG.PrintTimeAndLog( 0, "오크통 활성, 보상 가능 개수 : %d", m_iCurrEnableRewardTurnNumber );
}

void ioPriateRenewalRullet::RenewalOakSwordUse( SP2Packet &rkPacket )
{
	TCPNetwork::MouseBusy( false );
	int iType;
	rkPacket >> iType;
	if ( iType == OAK_BARREL_OK )
	{
		bool bSucess;
		BYTE btCurrEnableRewardTurnNumber;
		BYTE btIndex;
		BYTE btFailRewardIndex;
		rkPacket >> bSucess;
		rkPacket >> btIndex;
		rkPacket >> btCurrEnableRewardTurnNumber;
		

		int iIndex = (int)btIndex;
		if ( !COMPARE( iIndex, 0, MAX_RENEWAL_RULLET_COUNT ) )
		{
			LOG.PrintTimeAndLog( 0, "STPK_OAK_BARREL_USE_SWORD Packet over index : %d ", iIndex );
			return;
		}

		if ( bSucess )
		{
			m_RoulletTable[btIndex] = 1;
			if( g_TCPSocketMgr.IsTestZone() )
				LOG.PrintTimeAndLog( 0, "오크통 칼 꽂기 성공, 보상 가능 개수 : %d, 인덱스 : %d ", (int)btCurrEnableRewardTurnNumber, (int)btIndex );
		}
		else
		{
			ClearRullet();
			rkPacket >> btFailRewardIndex;
			if( g_TCPSocketMgr.IsTestZone() )
				LOG.PrintTimeAndLog( 0, "오크통 칼 꽂기 실패 - 보상받는 인덱스 : %d, 랜덤 인덱스 값 : %d", m_iCurrEnableRewardTurnNumber, (int)btFailRewardIndex );
		}

		ioUserEtcItem* pETC = g_MyInfo.GetUserEtcItem();
		if( pETC )
		{
			ioUserEtcItem::ETCITEMSLOT kSlot;
			if( pETC->GetEtcItem( m_iSwordCode, kSlot ) )
			{
				//kSlot.m_iValue1--;
				if( kSlot.m_iValue1 <= 0 )
					pETC->DeleteEtcItem( kSlot.m_iType );
				else
					pETC->SetEtcItem( kSlot );
			}
		}

		PriateRulletRenewalWnd* pWnd = dynamic_cast<PriateRulletRenewalWnd*>( g_GUIMgr.FindWnd( PRIATE_RULLET_RENEWAL_WND ) );
		if( pWnd )
		{
			if ( bSucess )
				pWnd->OnSuccessSwordUse( iIndex, (int)btCurrEnableRewardTurnNumber );
			else
				pWnd->OnFailSwordUse( iIndex, (int)btFailRewardIndex, m_iCurrEnableRewardTurnNumber );
		}
		m_iCurrEnableRewardTurnNumber = (int)btCurrEnableRewardTurnNumber;
			
	}
	else if ( iType == OAK_BARREL_ERR_HOLE_INDEX )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "초과된 인덱스 입니다." );
		return;
	}
	else if ( iType == OAK_BARREL_ERR_HOLE_DUPLICATED )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "이미 장착된 인덱스 입니다." );
		return;
	}
	else if ( iType == OAK_BARREL_ERR_LIMIT_SWORD )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "일일 최대 한도를 초과" );
		return;
	}
	else
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "오크통 에러 타입 : %d", iType );
		return;
	}
	
}

void ioPriateRenewalRullet::RenewalOakGetReward( SP2Packet &rkPacket )
{
	TCPNetwork::MouseBusy( false );
	int iType;
	rkPacket >> iType;
	if ( iType == OAK_BARREL_OK )
	{
		BYTE btRewardCount;
		rkPacket >> btRewardCount;
		IntVec vRewardList;
		BYTE btData;
		for ( BYTE i = 0 ;i < btRewardCount ; ++i )
		{
			rkPacket >> btData;
			vRewardList.push_back( (int)btData );
		}
		m_iCurrEnableRewardTurnNumber = 0;
		ClearRullet();

		//보상이 0이면 안된다!
		if ( !COMPARE( vRewardList.size(), 1, MAX_RENEWAL_RULLET_COUNT ) )
		{
			LOG.PrintTimeAndLog( 0, "STPK_OAK_BARREL_GET_REWARD Packet Reward Data Empty reward count %d ", btRewardCount );
			return;
		}

		if( g_TCPSocketMgr.IsTestZone() )
		{
			LOG.PrintTimeAndLog( 0, "오크통 보상 받기 성공, 보상 개수 : %d", (int)vRewardList.size() );
			for ( int i=0 ; i<(int)vRewardList.size() ; ++i )
				LOG.PrintTimeAndLog( 0, "오크통 %d 보상, 보상 index : %d ", i, vRewardList[i] );
		}
		PriateRulletRenewalWnd* pWnd = dynamic_cast<PriateRulletRenewalWnd*>( g_GUIMgr.FindWnd( PRIATE_RULLET_RENEWAL_WND ) );
		if( pWnd )
			pWnd->StartRollingEndTime( vRewardList );
	}
	else if ( iType == OAK_BARREL_ERR_REWARD_STEP )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "보상 단계가 맞지 않습니다.", iType );
		return;
	}
	else
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "오크통 에러 타입 : %d", iType );
		return;
	}
}

bool ioPriateRenewalRullet::SendRenewalRulletUse( int Index )
{
	if( COMPARE( Index, 0, MAX_RENEWAL_RULLET_COUNT ) && m_RoulletTable[Index] == 0 )
	{
		TCPNetwork::MouseBusy( true );
		SP2Packet kPacket( CTPK_OAK_BARREL_USE_SWORD );
		kPacket << Index;
		TCPNetwork::SendToServer( kPacket );
		return true;
	}
	return false;
}

bool ioPriateRenewalRullet::SendRewardRequest()
{
	if ( m_iCurrEnableRewardTurnNumber <= 0 )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "보상 가능한 아이템이 하나 이상 있어야 합니다." );
		return false;
	}
	TCPNetwork::MouseBusy( true );

	BYTE btNum = (BYTE)m_iCurrEnableRewardTurnNumber;
	SP2Packet kPacket( CTPK_OAK_BARREL_GET_REWARD );
	kPacket << btNum;
	TCPNetwork::SendToServer( kPacket );

	return true;
}

int ioPriateRenewalRullet::GetRulletStickedValue( int Index )
{
	if( COMPARE( Index, 0, MAX_RENEWAL_RULLET_COUNT ) )
		return m_RoulletTable[Index];

	return 0;
}
