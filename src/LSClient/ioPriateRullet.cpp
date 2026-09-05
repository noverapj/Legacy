#include "stdafx.h"

#include "ioPriateRullet.h"
#include "GUI/PriateRulletWnd.h"

DWORD OakSwordTypeToOakCode( OakSwordType eType )
{
	switch( eType )
	{
	case OST_WOOD:
		return ioEtcItem::EIT_ETC_OAK_WOOD_SWORD;
	case OST_SILVER:
		return ioEtcItem::EIT_ETC_OAK_SILVER_SWORD;
	case OST_GOLD:
		return ioEtcItem::EIT_ETC_OAK_GOLD_SWORD;
	}

	return 0;
}

OakSwordType OakCodeToOakSwordType( DWORD dwCode )
{
	switch( dwCode )
	{
	case ioEtcItem::EIT_ETC_OAK_WOOD_SWORD:
		return OST_WOOD;
	case ioEtcItem::EIT_ETC_OAK_SILVER_SWORD:
		return OST_SILVER;
	case ioEtcItem::EIT_ETC_OAK_GOLD_SWORD:
		return OST_GOLD;
	}

	return OST_NONE;
}

ioPriateRullet::ioPriateRullet() : m_nMaxHP( 0), m_nCurrHP( 0 )
{
	ClearRullet();
}

ioPriateRullet::~ioPriateRullet()
{

}

void ioPriateRullet::ClearRullet()
{
	for( int i = 0; i < MAX_RULLET_COUNT; ++i )
		m_RoulletTable[i] = 0;
}

void ioPriateRullet::Initialize()
{
	char szBuf[MAX_PATH];
	ioINILoader_e kLoader( "config/sp2_oak_of_fear.ini" );
	kLoader.SetTitle_e( "common" );

	m_nMaxHP = kLoader.LoadInt_e( "max_hp", 0 );

	//보상 테이블
	int nMax = kLoader.LoadInt( "max_reward", 0 );

	m_RewardTable.clear();
	for( int i = 0; i < nMax; ++i )
	{
		wsprintf_e( szBuf, "reward%d", i + 1 );
		kLoader.SetTitle( szBuf );
		LoadReward( kLoader );
	}

	kLoader.SetTitle_e( "common" );
	//보너스 보상 테이블
	nMax = kLoader.LoadInt( "max_bonus_reward", 0 );

	m_BonusRewardTable.clear();
	for( int i = 0; i < nMax; ++i )
	{
		wsprintf_e( szBuf, "bonus_reward%d", i + 1 );
		kLoader.SetTitle( szBuf );
		LoadBonusReward( kLoader );
	}

	kLoader.SetTitle_e( "common" );

	//검 종류 테이블
	m_SwordInfoTable.clear();
	nMax = kLoader.LoadInt( "sword_max", 0 );
	for( int i = 0; i < nMax; ++i )
	{
		wsprintf_e( szBuf, "sword%d", i + 1 );
		kLoader.SetTitle( szBuf );
		LoadSword( kLoader );
	}

	m_DevelopKFaceTable.clear();
	LoadDevelopKFace( kLoader );
}

void ioPriateRullet::LoadReward( ioINILoader& rkLoader )
{
	PriateRulletReward Reward;
	Reward.m_eRewardType	= PriateRulletReward::RT_REWARD;
	Reward.m_bHidden		= rkLoader.LoadBool_e( "reward_hidden", false );
	Reward.m_eType			= rkLoader.LoadInt_e( "reward_type", 0 );
	Reward.m_nVaule1		= rkLoader.LoadInt_e( "reward_value1", 0 );
	Reward.m_nVaule2		= rkLoader.LoadInt_e( "reward_value2", 0 );
	Reward.m_nMin			= rkLoader.LoadInt_e( "reward_min_hp", 0 );
	Reward.m_nMax			= rkLoader.LoadInt_e( "reward_max_hp", 0 );
		
	Reward.m_Name			= g_PresentMgr.GetPresentValue1Text( Reward.m_eType, Reward.m_nVaule1, Reward.m_nVaule2 );
	Reward.m_Icon			= g_PresentMgr.GetPresentIconText( Reward.m_eType, Reward.m_nVaule1, Reward.m_nVaule2 );
	Reward.m_SubIcon = g_PresentMgr.GetPresentSubIconText( Reward.m_eType, Reward.m_nVaule1, Reward.m_nVaule2 );

	m_RewardTable.push_back( Reward );
}

void ioPriateRullet::LoadBonusReward( ioINILoader& rkLoader )
{
	PriateRulletReward Reward;
	Reward.m_eRewardType	= PriateRulletReward::RT_BONUS_REWARD;
	Reward.m_bHidden		= rkLoader.LoadBool_e( "reward_hidden", false );
	Reward.m_eType			= rkLoader.LoadInt_e( "reward_type", 0 );
	Reward.m_nVaule1		= rkLoader.LoadInt_e( "reward_value1", 0 );
	Reward.m_nVaule2		= rkLoader.LoadInt_e( "reward_value2", 0 );
	Reward.m_nMin			= rkLoader.LoadInt_e( "reward_use_min", 0 );
	Reward.m_nMax			= rkLoader.LoadInt_e( "reward_use_max", 0 );

	Reward.m_Name			= g_PresentMgr.GetPresentValue1Text( Reward.m_eType, Reward.m_nVaule1, Reward.m_nVaule2 );
	Reward.m_Icon			= g_PresentMgr.GetPresentIconText( Reward.m_eType, Reward.m_nVaule1, Reward.m_nVaule2 );
	Reward.m_SubIcon = g_PresentMgr.GetPresentSubIconText( Reward.m_eType, Reward.m_nVaule1, Reward.m_nVaule2 );

	m_BonusRewardTable.push_back( Reward );
}

void ioPriateRullet::LoadSword( ioINILoader& rkLoader )
{
	SwordInfo Info;

	char szBuf[MAX_PATH];
	int nMax = rkLoader.LoadInt( "sword_damage_max", 0 );
	for( int i = 0; i < nMax; ++i )
	{
		SwordDamage Damage;
		wsprintf_e( szBuf, "sword_damage%d", i + 1 );
		Damage.m_nDamage	= rkLoader.LoadInt( szBuf, 0 );

		wsprintf_e( szBuf, "sword_damage%d_rand", i + 1 );
		Damage.m_nRand		= rkLoader.LoadInt( szBuf, 0 );

		Info.m_SwordDamageTable.push_back( Damage );
	}

	Info.m_nSwordCode = rkLoader.LoadInt_e( "sword_etc_code", 0 );
	m_SwordInfoTable.push_back( Info );
}

void ioPriateRullet::LoadDevelopKFace( ioINILoader& rkLoader )
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

		wsprintf_e( szKey, "face%d_hp_max", i + 1 );
		kFace.m_nMax = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "face%d_hp_min", i + 1 );
		kFace.m_nMin = rkLoader.LoadInt( szKey, 0 );

		m_DevelopKFaceTable.push_back( kFace );
	}
}

PriateRulletReward* ioPriateRullet::GetReward( int Index )
{
	if( COMPARE( Index, 0, (int)m_RewardTable.size() ) )
	{
		return &m_RewardTable[Index];
	}

	return NULL;
}

PriateRulletReward* ioPriateRullet::GetCurrReward()
{
	for( int i = 0; i < (int)m_RewardTable.size(); ++i  )
	{
		if( m_RewardTable[i].m_nMin <= m_nCurrHP && m_nCurrHP <= m_RewardTable[i].m_nMax )
			return &m_BonusRewardTable[i];
	}

	return NULL;
}

int ioPriateRullet::GetRewardTurnNumber( int nHP ) const
{	
	int TurnNumber = 0;
	for( auto iter = m_RewardTable.begin(); iter != m_RewardTable.end(); ++iter )
	{	
		const PriateRulletReward& rkReward = *iter;
		if( rkReward.m_nMin <= nHP && rkReward.m_nMax >= nHP )
		{
			++TurnNumber;
			return TurnNumber;
		}
		else if( rkReward.m_nMax >= nHP  )
		{
			++TurnNumber;
		}
	}

	return TurnNumber;
}

PriateRulletReward* ioPriateRullet::GetBounsReward( int Index )
{
	if( COMPARE( Index, 0, (int)m_BonusRewardTable.size() ) )
	{
		return &m_BonusRewardTable[Index];
	}

	return NULL;
}

PriateRulletReward* ioPriateRullet::GetCurrBounsReward()
{
	int nSticked = GetRulletStickedCount();
	for( int i = 0; i < (int)m_BonusRewardTable.size(); ++i  )
	{
		if( m_BonusRewardTable[i].m_nMin <= nSticked && nSticked <= m_BonusRewardTable[i].m_nMax )
			return &m_BonusRewardTable[i];
	}

	return NULL;
}

int ioPriateRullet::GetSwordCount( int Index )
{
	ioUserEtcItem* pInven = g_MyInfo.GetUserEtcItem();
	if( pInven == NULL )
		return 0;

	if( !COMPARE( Index, 0, (int)m_SwordInfoTable.size() ) )
		return 0;

	ioUserEtcItem::ETCITEMSLOT rkEtcItem;
	if( pInven->GetEtcItem( m_SwordInfoTable[Index].m_nSwordCode, rkEtcItem ) )
	{
		return rkEtcItem.m_iValue1;
	}

	return 0;
}

int ioPriateRullet::GetRulletStickedCount()
{
	int nStickedCount = 0;
	for( int i = 0; i < MAX_RULLET_COUNT; ++i )
	{
		if( 0 < m_RoulletTable[i] )
			nStickedCount++;
	}

	return nStickedCount;
}

int ioPriateRullet::GetRulletStickedValue( int Index )
{
	if( COMPARE( Index, 0, MAX_RULLET_COUNT ) )
		return m_RoulletTable[Index];

	return 0;
}

int	ioPriateRullet::GetCurrHP() const
{
	return m_nCurrHP;
}

int ioPriateRullet::GetFaceIndex() const
{	
	int Index = 0;
	for( auto iter = m_DevelopKFaceTable.begin(); iter != m_DevelopKFaceTable.end(); ++iter )
	{
		const DevelopKFace& kFace = *iter;
		if( kFace.m_nMin <= m_nCurrHP && kFace.m_nMax >= m_nCurrHP )
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

int ioPriateRullet::GetFaceIndex( int nHP ) const
{	
	int Index = 0;
	for( auto iter = m_DevelopKFaceTable.begin(); iter != m_DevelopKFaceTable.end(); ++iter )
	{
		const DevelopKFace& kFace = *iter;
		if( kFace.m_nMin <= nHP && kFace.m_nMax >= nHP )
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

DWORD ioPriateRullet::GetCurrFaceTime() const
{
	for( auto iter = m_DevelopKFaceTable.begin(); iter != m_DevelopKFaceTable.end(); ++iter )
	{
		const DevelopKFace& kFace = *iter;
		if( kFace.m_nMin <= m_nCurrHP && kFace.m_nMax >= m_nCurrHP )
		{
			return kFace.m_nAnimationTime;
		}
	}

	return 0;
}

void ioPriateRullet::GetCurrFaceName( ioHashStringVec& szFaceList ) const
{
	for( auto iter = m_DevelopKFaceTable.begin(); iter != m_DevelopKFaceTable.end(); ++iter )
	{
		const DevelopKFace& kFace = *iter;
		if( kFace.m_nMin <= m_nCurrHP && kFace.m_nMax >= m_nCurrHP )
		{
			szFaceList = kFace.m_FaceName;
		}
	}
}

void ioPriateRullet::GetCurrBubble( ioHashStringVec& BubbleList, int& nBubbleTime )
{
	for( auto iter = m_DevelopKFaceTable.begin(); iter != m_DevelopKFaceTable.end(); ++iter )
	{
		const DevelopKFace& kFace = *iter;
		if( kFace.m_nMin <= m_nCurrHP && kFace.m_nMax >= m_nCurrHP )
		{
			BubbleList		= kFace.m_BubbleMentList;
			nBubbleTime		= kFace.m_nBubbleTime;
		}
	}
}

void ioPriateRullet::SendRequest()
{
	TCPNetwork::MouseBusy( true );
	SP2Packet kPacket( CTPK_OAK_INFO_REQUEST );
	TCPNetwork::SendToServer( kPacket );
}

void ioPriateRullet::ApplyRulletRequest( SP2Packet &rkPacket )
{
	TCPNetwork::MouseBusy( false );
	int nCount = 0;
	rkPacket >> m_nCurrHP;
	rkPacket >> nCount;
	m_nCurrRewardTurnNumber = GetRewardTurnNumber( m_nCurrHP );

	for( int i = 0; i < nCount; ++i )
	{
		rkPacket >> m_RoulletTable[i];
	}

	PriateRulletWnd* pWnd = dynamic_cast<PriateRulletWnd*>( g_GUIMgr.FindWnd( PRIATE_RULLET_WND ) );
	if( pWnd )
		pWnd->ShowWnd();
}

bool ioPriateRullet::SendRulletUse( int Index, DWORD dwCode )
{
	if( COMPARE( Index, 0, MAX_RULLET_COUNT ) && m_RoulletTable[Index] == 0 )
	{
		TCPNetwork::MouseBusy( true );
		SP2Packet kPacket( CTPK_OAK_USE_SWORD );
		kPacket << Index << (byte)OakCodeToOakSwordType( dwCode );
		TCPNetwork::SendToServer( kPacket );
		return true;
	}

	return false;
}

void ioPriateRullet::ApplyRulletUse( SP2Packet &rkPacket )
{
	TCPNetwork::MouseBusy( false );
	int CurrHP = 0;
	int eCritical = 0;
	int eType = 0;
	int Index = 0;
	int nHP = 0;

	rkPacket >> nHP >> eCritical >> eType >> Index;
	if( COMPARE( Index, 0, MAX_RULLET_COUNT ) )
	{
		m_RoulletTable[Index] = eType;

		//HP구간이 같다면 도발 애니메이션을 재생하도록 함
		bool bProvoke = false;
		if( GetFaceIndex( m_nCurrHP ) == GetFaceIndex( nHP ) )
			bProvoke = true;

		if( g_App.IsMeDeveloper() )
		{
			LOG.PrintTimeAndLog(0, "공포의 오크통 데미지 : %d, 현재 HP : %d, 아이템 : %d, 크리티컬 : %d",
				m_nCurrHP - nHP, nHP, OakSwordTypeToOakCode( static_cast<OakSwordType>(eType ) ), (int)eCritical );
		}

		//보상 갯수를 체크하여 애니메이션 종류를 결정(130%스케일애니메이션/200%스케일 애니메이션)
		int nPreTurnNumber		= m_nCurrRewardTurnNumber;
		m_nCurrRewardTurnNumber	= GetRewardTurnNumber( nHP );
		m_nCurrHP				= nHP;

		ioUserEtcItem* pETC = g_MyInfo.GetUserEtcItem();
		if( pETC )
		{
			ioUserEtcItem::ETCITEMSLOT kSlot;
			if( pETC->GetEtcItem( OakSwordTypeToOakCode( static_cast<OakSwordType>(eType ) ), kSlot ) )
			{
				kSlot.m_iValue1--;
				if( kSlot.m_iValue1 <= 0 )
					pETC->DeleteEtcItem( kSlot.m_iType );
				else
					pETC->SetEtcItem( kSlot );
			}
		}

		MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
		if( pInvenWnd && pInvenWnd->IsShow() )
		{
			pInvenWnd->UpdateTab( true, true );
		}

		int nSticked = GetRulletStickedCount();

		PriateRulletWnd* pWnd = dynamic_cast<PriateRulletWnd*>( g_GUIMgr.FindWnd( PRIATE_RULLET_WND ) );
		if( pWnd )
			pWnd->OnSwordUse( Index, false, bProvoke, m_nCurrHP, m_nCurrRewardTurnNumber - nPreTurnNumber, eCritical );
	}
}

void ioPriateRullet::SendRulletReset()
{
	TCPNetwork::MouseBusy( true );
	SP2Packet kPacket( CTPK_OAK_RESET );
	TCPNetwork::SendToServer( kPacket );
}

void ioPriateRullet::ApplyRulletReset( SP2Packet &rkPacket )
{
	TCPNetwork::MouseBusy( false );

	int nPreHP				= m_nCurrHP;
	m_nCurrHP				= m_nMaxHP;
	m_nCurrRewardTurnNumber	= 0;
	ClearRullet();

	if( g_App.IsMeDeveloper() )
	{
		LOG.PrintTimeAndLog(0, "공포의 오크통 초기화 : HP(%d)", m_nCurrHP );
	}

	PriateRulletWnd* pWnd = dynamic_cast<PriateRulletWnd*>( g_GUIMgr.FindWnd( PRIATE_RULLET_WND ) );
	if( pWnd )
	{	
		pWnd->SetState( PriateRulletWnd::RS_END );
	}
}