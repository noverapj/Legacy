

#include "stdafx.h"

#include "ioBaseChar.h"

#include "ioDestroyCallBuff.h"

ioDestroyCallBuff::ioDestroyCallBuff()
{
	m_dwDelayEffectID = -1;
}

ioDestroyCallBuff::ioDestroyCallBuff( const ioDestroyCallBuff &rhs )
	: ioBuff( rhs ),
	m_szDelayEffect( rhs.m_szDelayEffect ),
	m_szRemoveBuffName( rhs.m_szRemoveBuffName ),
	m_bIsUseKeyDownDestroy( rhs.m_bIsUseKeyDownDestroy ),
	m_dwDestroyStartTime( rhs.m_dwDestroyStartTime )
{
	m_dwDelayEffectID = -1;

	m_bRcvDestroy = false;

	m_bDestroySeted = false;
}

ioDestroyCallBuff::~ioDestroyCallBuff()
{
}

void ioDestroyCallBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );
	//추가 데이터 읽기
	LoadDestroyProperty( rkLoader );
}

//추가함수//////////////////////////////
//데이터 읽기
void ioDestroyCallBuff::LoadDestroyProperty( ioINILoader &rkLoader )
{
	//대기 이펙트 설정
	char szBuf[MAX_PATH];	
	rkLoader.LoadString_e( "delay_effect", "", szBuf, MAX_PATH );
	m_szDelayEffect = szBuf;

	rkLoader.LoadString_e( "remove_buff_name", "", szBuf, MAX_PATH );
	m_szRemoveBuffName = szBuf;
	
	//폭발설정
	m_bIsUseKeyDownDestroy = rkLoader.LoadBool_e( "set_use_key_down_destroy", false );
	m_dwDestroyStartTime = rkLoader.LoadInt_e( "destroy_time", 0 );
}

ioBuff* ioDestroyCallBuff::Clone()
{
	return new ioDestroyCallBuff( *this );
}

void ioDestroyCallBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );

	m_dwDelayEffectID = -1;
			
	ioEffect *pEffect = pOwner->AttachEffect( m_szDelayEffect );			
	if( pEffect )
		m_dwDelayEffectID = pEffect->GetUniqueID();	

	m_dwDestroyEndTime = m_dwDestroyStartTime + FRAMEGETTIME();		
}

bool ioDestroyCallBuff::ApplyStartBuff(ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	m_dwDelayEffectID = -1;

	ioEffect *pEffect = pOwner->AttachEffect( m_szDelayEffect );			
	if( pEffect )
		m_dwDelayEffectID = pEffect->GetUniqueID();	

	m_dwDestroyEndTime = m_dwDestroyStartTime + FRAMEGETTIME();
	
	return true;
}

void ioDestroyCallBuff::ProcessBuff( float fTimePerSec )
{
	if( CheckDestroyTime() )
	{
		m_bDestroySeted = true;
	}
	else
	{
		if( CheckDestroy( m_pOwner ) )
		{
			m_bDestroySeted = true;
		}
	}

	if ( m_bDestroySeted )
	{
		CheckCallingProcess();
		SetReserveEndBuff();
		if ( !m_szRemoveBuffName.IsEmpty() )
		{
			m_pOwner->RemoveBuff( m_szRemoveBuffName, false );
		}
	}
}

bool ioDestroyCallBuff::CheckDestroy( ioBaseChar *pOwner )
{
	if( m_bRcvDestroy )
		return true;

	//생성한 캐릭터가 어택키를 눌렀으면
	if( pOwner->IsNeedProcess() && pOwner->IsAttackKey() )
	{
		//동기화처리
		if ( pOwner->IsNeedSendNetwork() == true )
		{
			SP2Packet kPacket( CUPK_APPLY_EXTRA_BUFF_INFO );
			kPacket << pOwner->GetCharName();
			kPacket << GetName();
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
		return true;
	}

	return false;
}

bool ioDestroyCallBuff::CheckDestroyTime()
{
	DWORD dwCurTime = FRAMEGETTIME();

	if (m_dwDestroyEndTime <= dwCurTime )
	{
		return true;
	}

	return false;
}

void ioDestroyCallBuff::EndBuff()
{
	if( m_dwDelayEffectID != -1 )
	{
		ioEffect *pEffect = g_EffectFactory.GetEffect( m_dwDelayEffectID );
		if( pEffect )
		{
			pEffect->EndEffectForce();
			m_dwDelayEffectID = -1;
		}
	}
	
	ioBuff::EndBuff();
}

void ioDestroyCallBuff::ApplyExtraBuffInfo( SP2Packet &rkPacket )
{
	m_bRcvDestroy = true;
}
