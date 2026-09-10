#include "StdAfx.h"
#include "ioRandomPassiveSkill.h"

ioRandomPassiveSkill::ioRandomPassiveSkill(void)
{
	m_fRandomPercent = 0.0f;
}

ioRandomPassiveSkill::ioRandomPassiveSkill( const ioRandomPassiveSkill &rhs ):
ioPassiveSkill( rhs ),
m_fMaxRandomPercent( rhs.m_fMaxRandomPercent ),
m_WounderBuffList( rhs.m_WounderBuffList ),
m_WounderDeBuffList( rhs.m_WounderDeBuffList ),
m_AttackerBuffList( rhs.m_AttackerBuffList ),
m_DefenseBreakBuffList( rhs.m_DefenseBreakBuffList ),
m_EnableModeList( rhs.m_EnableModeList )
{
	m_fRandomPercent = 0.0f;
}

ioRandomPassiveSkill::~ioRandomPassiveSkill(void)
{
}

ioSkill* ioRandomPassiveSkill::Clone()
{
	return new ioRandomPassiveSkill( *this );
}

void ioRandomPassiveSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioPassiveSkill::LoadProperty( rkLoader );
	m_fMaxRandomPercent = rkLoader.LoadFloat_e( "max_random_percent", 0.0f );

	char szBuf[MAX_PATH] = "", szKey[MAX_PATH];
	int iCnt = rkLoader.LoadInt_e( "max_wounder_buff_count", 0 );
	m_WounderBuffList.clear();
	for( int i=0; i<iCnt; ++i )
	{
		wsprintf_e( szKey, "wounder_buff%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_WounderBuffList.push_back( ioHashString(szBuf) );
	}

	iCnt = rkLoader.LoadInt_e( "max_wounder_debuff_count", 0 );
	m_WounderDeBuffList.clear();
	for( int i=0; i<iCnt; ++i )
	{
		wsprintf_e( szKey, "wounder_debuff%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_WounderDeBuffList.push_back( ioHashString(szBuf) );
	}

	iCnt = rkLoader.LoadInt_e( "max_attacker_buff_count", 0 );
	m_AttackerBuffList.clear();
	for( int i=0; i<iCnt; ++i )
	{
		wsprintf_e( szKey, "attacker_buff%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_AttackerBuffList.push_back( ioHashString(szBuf) );
	}


	iCnt = rkLoader.LoadInt_e( "max_defense_break_buff_count", 0 );
	m_DefenseBreakBuffList.clear();
	for( int i=0; i<iCnt; ++i )
	{
		wsprintf_e( szKey, "defense_break_buff%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_DefenseBreakBuffList.push_back( ioHashString(szBuf) );
	}

	int iModeCnt = rkLoader.LoadInt_e( "enable_mode_cnt", 0 );
	for( i=0; i < iModeCnt; ++i )
	{
		wsprintf_e( szKey, "enable_mode_index%d", i+1 );

		int iModeIndex = rkLoader.LoadInt( szKey, 0 );
		m_EnableModeList.push_back( iModeIndex );
	}
}

bool ioRandomPassiveSkill::OnSkillStart( ioBaseChar *pChar )
{
	if( !ioPassiveSkill::OnSkillStart( pChar ) )
		return false;
	ioItem* pItem = GetOwnerItem();
	if ( !pItem )
		return false;

	if ( !COMPARE( pItem->GetType(), ES_RING, MAX_EQUIP_SLOT ) )
		return false;

	float fNum = GetAccessoryRandomNum() / FLOAT1000;
	if ( !COMPARE( fNum, 0, m_fMaxRandomPercent ) )
		return false;

	m_fRandomPercent = fNum;
	return true;
}

void ioRandomPassiveSkill::AddWounderRandomDebuffs( ioBaseChar* pAttacker, ioBaseChar* pWounder, int iModeType )
{
	if ( !pAttacker || !pWounder )
		return;

	if ( !IsEnableMode( iModeType ) )
		return;

	if ( m_WounderDeBuffList.empty() )
		return;

	if ( pAttacker->GetTeam() == pWounder->GetTeam() )
		return;

	if ( pAttacker->GetCharName() == pWounder->GetCharName() )
		return;

	IORandom random;
	random.SetRandomSeed( timeGetTime() );
	int iRand = random.Random( m_fMaxRandomPercent );

	if ( iRand > m_fRandomPercent )
		return;

	int iSize = m_WounderDeBuffList.size();
	for ( int i=0 ; i<iSize ; ++i )
	{
		ioBuff *pBuff = pWounder->AddNewBuff( m_WounderDeBuffList[i],
			pAttacker->GetCharName(), GetOwnerItem()->GetName(), this );
	}

	ioItem *pItem = GetOwnerItem();
	if ( !pItem || pItem->GetOwnerName().IsEmpty())
		return;

	if ( pWounder->IsNeedSendNetwork() )
	{
		int iSlot = m_pOwnerItem->GetType() - 1;
		SP2Packet kPacket( CUPK_PASSIVE_SKILL_EXTRA_INFO );
		kPacket << pAttacker->GetCharName();
		kPacket << GetName();
		kPacket << iSlot;
		kPacket << (int)ST_WOUNDER_DEBUFF;
		kPacket << pWounder->GetCharName();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioRandomPassiveSkill::AddAttackerRandombuffs( ioBaseChar* pAttacker, ioBaseChar* pWounder, int iModeType )
{
	if ( !pAttacker || !pWounder )
		return;

	if ( !IsEnableMode( iModeType ) )
		return;

	if ( m_AttackerBuffList.empty() )
		return;

	if ( pAttacker->GetTeam() == pWounder->GetTeam() )
		return;

	if ( pAttacker->GetCharName() == pWounder->GetCharName() )
		return;

	IORandom random;
	random.SetRandomSeed( timeGetTime() );
	int iRand = random.Random( m_fMaxRandomPercent );

	if ( iRand > m_fRandomPercent )
		return;

	int iSize = m_AttackerBuffList.size();
	for ( int i=0 ; i<iSize ; ++i )
	{
		ioBuff *pBuff = pAttacker->AddNewBuff( m_AttackerBuffList[i],
			pAttacker->GetCharName(), GetOwnerItem()->GetName(), this );
	}

	if ( pWounder->IsNeedSendNetwork() )
	{
		int iSlot = m_pOwnerItem->GetType() - 1;
		SP2Packet kPacket( CUPK_PASSIVE_SKILL_EXTRA_INFO );
		kPacket << pAttacker->GetCharName();
		kPacket << GetName();
		kPacket << iSlot;
		kPacket << (int)ST_ATTACKER_BUFF;
		kPacket << pAttacker->GetCharName();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioRandomPassiveSkill::AddWounderRandombuffs( ioBaseChar* pAttacker, ioBaseChar* pWounder, int iModeType )
{
	if ( !pAttacker || !pWounder )
		return;

	if ( !IsEnableMode( iModeType ) )
		return;

	if ( m_WounderBuffList.empty() )
		return;

	if ( pAttacker->GetTeam() == pWounder->GetTeam() )
		return;

	if ( pAttacker->GetCharName() == pWounder->GetCharName() )
		return;

	IORandom random;
	random.SetRandomSeed( timeGetTime() );
	int iRand = random.Random( m_fMaxRandomPercent );

	if ( iRand > m_fRandomPercent )
		return;

	int iSize = m_WounderBuffList.size();
	for ( int i=0 ; i<iSize ; ++i )
	{
		ioBuff *pBuff = pWounder->AddNewBuff( m_WounderBuffList[i],
			pWounder->GetCharName(), GetOwnerItem()->GetName(), this );
	}

	if ( pWounder->IsNeedSendNetwork() )
	{
		int iSlot = m_pOwnerItem->GetType() - 1;
		SP2Packet kPacket( CUPK_PASSIVE_SKILL_EXTRA_INFO );
		kPacket << pWounder->GetCharName();
		kPacket << GetName();
		kPacket << iSlot;
		kPacket << (int)ST_WOUNDER_BUFF;
		kPacket << pWounder->GetCharName();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioRandomPassiveSkill::AddDefenseBreakBuffs( ioBaseChar* pAttacker, ioBaseChar* pWounder, int iModeType )
{
	if ( !pAttacker || !pWounder )
		return;

	if ( !IsEnableMode( iModeType ) )
		return;

	if ( !IsEnableMode( iModeType ) )
		return;

	if ( m_DefenseBreakBuffList.empty() )
		return;

	if ( pAttacker->GetTeam() == pWounder->GetTeam() )
		return;

	if ( pAttacker->GetCharName() == pWounder->GetCharName() )
		return;

	IORandom random;
	random.SetRandomSeed( timeGetTime() );
	int iRand = random.Random( m_fMaxRandomPercent );

	if ( iRand > m_fRandomPercent )
		return;

	int iSize = m_DefenseBreakBuffList.size();
	for ( int i=0 ; i<iSize ; ++i )
	{
		ioBuff *pBuff = pAttacker->AddNewBuff( m_DefenseBreakBuffList[i],
			pAttacker->GetCharName(), GetOwnerItem()->GetName(), this );
	}

	if ( pWounder->IsNeedSendNetwork() )
	{
		int iSlot = m_pOwnerItem->GetType() - 1;
		SP2Packet kPacket( CUPK_PASSIVE_SKILL_EXTRA_INFO );
		kPacket << pAttacker->GetCharName();
		kPacket << GetName();
		kPacket << iSlot;
		kPacket << (int)ST_DEFENCE_BREAK_BUFF;
		kPacket << pAttacker->GetCharName();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioRandomPassiveSkill::IsEnableMode( int iCurModeType )
{
	//비어 있을 경우 모든 모드에서 가능하게
	if ( m_EnableModeList.empty() )
		return true;

	int iSize = m_EnableModeList.size();
	for( int i=0; i < iSize; ++i )
	{
		if( m_EnableModeList[i] == iCurModeType )
			return true;
	}
	return false;
}

void ioRandomPassiveSkill::ApplyExtraPassiveSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	ioPassiveSkill::ApplyExtraPassiveSkillInfo( pChar, pStage, rkPacket );

	if ( !pChar || !pStage )
		return;
	int iType;
	rkPacket >> iType;

	switch( iType )
	{
	case ST_WOUNDER_DEBUFF:
		{
			ioHashString szWounder;
			rkPacket >> szWounder;
			ioBaseChar* pWounder = pStage->GetBaseChar( szWounder );
			if ( !pWounder )
				return;
			int iSize = m_WounderDeBuffList.size();
			for ( int i=0 ; i<iSize ; ++i )
			{

				ioBuff *pBuff = pWounder->AddNewBuff( m_WounderDeBuffList[i],
					pChar->GetCharName(), GetOwnerItem()->GetName(), this );
			}
		}
		break;
	case ST_ATTACKER_BUFF:
		{
			int iSize = m_AttackerBuffList.size();
			for ( int i=0 ; i<iSize ; ++i )
			{
				ioBuff *pBuff = pChar->AddNewBuff( m_AttackerBuffList[i],
					pChar->GetCharName(), GetOwnerItem()->GetName(), this );
			}
		}
		break;
	case ST_WOUNDER_BUFF:
		{
			ioHashString szWounder;
			rkPacket >> szWounder;
			ioBaseChar* pWounder = pStage->GetBaseChar( szWounder );
			if ( !pWounder )
				return;

			int iSize = m_WounderBuffList.size();
			for ( int i=0 ; i<iSize ; ++i )
			{
				ioBuff *pBuff = pWounder->AddNewBuff( m_WounderBuffList[i],
					pWounder->GetCharName(), GetOwnerItem()->GetName(), this );
			}
		}
		break;
	case ST_DEFENCE_BREAK_BUFF:
		{
			int iSize = m_DefenseBreakBuffList.size();
			for ( int i=0 ; i<iSize ; ++i )
			{
				ioBuff *pBuff = pChar->AddNewBuff( m_DefenseBreakBuffList[i],
					pChar->GetCharName(), GetOwnerItem()->GetName(), this );
			}
		}
		break;
	}
}
