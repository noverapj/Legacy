#include "stdafx.h"

#include "ioPlayStage.h"
#include "ioShuffleBuffItem.h"

#include "ioNpcChar.h"

ioShuffleBuffItem::ioShuffleBuffItem()
{
	m_State = IS_WAIT;
	m_dwWaitTime = 0;
}

ioShuffleBuffItem::ioShuffleBuffItem( const ioShuffleBuffItem &rhs )
	: ioModeItem( rhs ),
	m_dwCreateTime( rhs.m_dwCreateTime ),
	m_szCreateEffect( rhs.m_szCreateEffect ),
	m_vCallBuffInfo( rhs.m_vCallBuffInfo )

{
	m_State = IS_WAIT;

	m_dwWaitTime = 0;
	m_dwCreateStartTime = 0;
}

ioShuffleBuffItem::~ioShuffleBuffItem()
{
}

void ioShuffleBuffItem::LoadProperty( ioINILoader &rkLoader )
{
	ioModeItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	m_dwCreateTime = rkLoader.LoadInt_e( "create_time", 1000 );
	rkLoader.LoadString_e( "create_effect", "", szBuf, MAX_PATH );
	m_szCreateEffect = szBuf;

	int iCount = rkLoader.LoadInt_e( "call_count", 0 );
	for( int i = 0; i < iCount; ++i )
	{
		CallBuffInfo Info;
		sprintf_s_e( szKey, "call%d_buff_target_type", i+1 );
		Info.m_TargetType = (BuffTargetType)rkLoader.LoadInt( szKey, 0 );

		sprintf_s_e( szKey, "call%d_buff_target_range", i+1 );
		Info.m_fRange = rkLoader.LoadFloat( szKey, 0.0f );

		sprintf_s_e( szKey, "call%d_buff_icon", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		Info.m_szCallBuffIcon = szBuf;

		sprintf_s_e( szKey, "call%d_buff_ment", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		Info.m_CallBuffMentInfo.m_szGenerateMent = szBuf;

		sprintf_s_e( szKey, "call%d_buff_ment_time", i+1 );
		Info.m_CallBuffMentInfo.m_dwGenerateMentTime = rkLoader.LoadInt( szKey, 0.0f );

		sprintf_s_e( szKey, "call%d_buff_ment_color", i+1 );
		rkLoader.LoadString( szKey, "0xffffffff", szBuf, MAX_PATH );
		char *szColor = NULL;
		Info.m_CallBuffMentInfo.m_dwGenerateColor = strtoul( szBuf, &szColor, 16 );		

		sprintf_s_e( szKey, "call%d_buff_ment_scale", i+1 );
		Info.m_CallBuffMentInfo.m_fGenerateScale = rkLoader.LoadFloat( szKey, 0.0f );

		sprintf_s_e( szKey, "call%d_buff_count", i+1 );
		int iBuffCount = rkLoader.LoadInt( szKey, 0 );

		if( 0 < iBuffCount )
			Info.m_szBuffNameVec.reserve( iBuffCount );

		for( int iBuff = 0; iBuff < iBuffCount; ++iBuff )
		{
			sprintf_s_e( szKey, "call%d_buff%d_name", i+1, iBuff+1 );
			rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
			Info.m_szBuffNameVec.push_back( szBuf );
		}

		m_vCallBuffInfo.push_back( Info );
	}
}

ioModeItem *ioShuffleBuffItem::Clone()
{
	return new ioShuffleBuffItem( *this );
}

void ioShuffleBuffItem::SetItemValue( DWORD dwIndex, float fXPos, float fZPos, DWORD dwActivateTime )
{
	ioModeItem::SetItemValue( dwIndex, fXPos, fZPos );

	m_dwWaitTime = FRAMEGETTIME() + dwActivateTime;
	m_State = IS_WAIT;
}

void ioShuffleBuffItem::SetCreateState()
{
	if( m_pCreator )
	{
		if( m_CreateEffectID != -1 )
		{			
			m_pCreator->DestroyMapEffect( m_CreateEffectID );
			m_CreateEffectID = -1;
		}
	}	

	OnCreate();
	m_State = IS_CREATE;
}

void ioShuffleBuffItem::Process()
{
	ioModeItem::Process();

	switch( m_State )
	{
	case IS_WAIT:
		ProcessWait();
		break;
	case IS_CREATE:
		ProcessCreate();
		break;
	case IS_DELAY:
		ProcessDelay();
		break;
	}
}

void ioShuffleBuffItem::ProcessWait()
{
	if( m_dwWaitTime < FRAMEGETTIME() )
	{
		m_dwCreateStartTime = FRAMEGETTIME();
		ioMapEffect *pEffect = m_pCreator->CreateMapEffect( m_szCreateEffect, GetMidPositionByOffset(), D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 ) );
		if( pEffect )
		{				
			m_CreateEffectID = pEffect->GetUniqueID();
		}

		SetCreateState();
	}
}

void ioShuffleBuffItem::ProcessCreate()
{
	if( m_dwCreateStartTime + m_dwCreateTime < FRAMEGETTIME() )
	{
		m_State = IS_DELAY;
	}
}

void ioShuffleBuffItem::ProcessDelay()
{
}

bool ioShuffleBuffItem::IsCollision( ioBaseChar *pChar )
{
	if( m_State != IS_DELAY )
		return false;

	return ioModeItem::IsCollision( pChar );
}

bool ioShuffleBuffItem::IsAvailableRange( ioBaseChar* pTarget, float fRange )
{
	if( m_State != IS_DELAY )
		return false;

	if( !pTarget )
		return false;

	D3DXVECTOR3 vDiff = pTarget->GetWorldPosition() - m_vPosition;
	float fLength = D3DXVec3Length( &vDiff );
	if( fLength > fRange )
		return false;

	return true;
}

void ioShuffleBuffItem::AddNewBuff( const CallBuffInfo& rkInfo, ioBaseChar* pOwner, ioBaseChar* pTarget )
{
	if( !pOwner || !pTarget )
		return;

	SP2Packet kPacket( CUPK_ADD_NEW_BUFF );
	kPacket << pTarget->GetCharName().c_str();
	kPacket << pOwner->GetCharName().c_str();
	kPacket << "";
	kPacket << pTarget->GetRandomSeed();
	kPacket << false;
	kPacket << 0.0f;
	kPacket << 0.0f;
	kPacket << 0.0f;
	kPacket << COWT_NONE;

	int iSize = static_cast<int>( rkInfo.m_szBuffNameVec.size() );
	kPacket << iSize;

	for( int i = 0; i < iSize; ++i )
	{
		const ioHashString& szBuffName = rkInfo.m_szBuffNameVec[i];
		pTarget->AddNewBuff( szBuffName, pOwner->GetCharName(), "", NULL );
		kPacket << szBuffName.c_str();
	}
	kPacket << false;

	P2PNetwork::SendToAllPlayingUser( kPacket );
}

const ioShuffleBuffItem::CallBuffInfo* ioShuffleBuffItem::OnBuffCall( ioBaseChar* pOwner, const BaseCharList &rkTargetList, DWORD dwCallBuff )
{	
	if( !COMPARE( dwCallBuff, 0, (DWORD)m_vCallBuffInfo.size() ) )
		return NULL;

	const CallBuffInfo& rkInfo = m_vCallBuffInfo[dwCallBuff];

	if( pOwner->IsNeedProcess() )
	{
		for( BaseCharList::const_iterator iter = rkTargetList.begin(); iter != rkTargetList.end(); ++iter )
		{
			ioBaseChar* pTarget = *iter;
			if( !pTarget )
				continue;

			if( IsNpcChar( pTarget ) )
				continue;

			switch( rkInfo.m_TargetType )
			{
			case BTT_OWENR:
				if( pOwner->GetCharName() == pTarget->GetCharName() )
				{
					AddNewBuff( rkInfo, pOwner, pTarget );
				}
				break;
			case BTT_ENEMY:
				if( pOwner->GetTeam() != pTarget->GetTeam() && IsAvailableRange( pTarget, rkInfo.m_fRange ) )
				{
					AddNewBuff( rkInfo, pOwner, pTarget );
				}
				break;
			case BTT_TEAM:
				if( pOwner != pTarget && pOwner->GetTeam() == pTarget->GetTeam() && IsAvailableRange( pTarget, rkInfo.m_fRange ) )
				{
					AddNewBuff( rkInfo, pOwner, pTarget );
				}
				break;
			}
		}
	}

	return &m_vCallBuffInfo[dwCallBuff];
}