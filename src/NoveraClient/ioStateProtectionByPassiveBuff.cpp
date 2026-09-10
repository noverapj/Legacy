
#include "stdafx.h"

#include "ioStateProtectionByPassiveBuff.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

ioStateProtectionByPassiveBuff::ioStateProtectionByPassiveBuff()
{
	m_iCurActiveCount = 0;
	m_dwEffectID = -1;
}

ioStateProtectionByPassiveBuff::ioStateProtectionByPassiveBuff( const ioStateProtectionByPassiveBuff &rhs )
	: ioBuff( rhs ),
	  m_fDamageRate( rhs.m_fDamageRate ),
	  m_bNoBlockState( rhs.m_bNoBlockState ),
	  m_bNoDefenseBreakState( rhs.m_bNoDefenseBreakState ),
	  m_ProtectSide( rhs.m_ProtectSide ),
	  m_bDisableDownState( rhs.m_bDisableDownState ),
	  m_HPCancelType( rhs.m_HPCancelType ),
	  m_szPassiveSkillName( rhs.m_szPassiveSkillName ),
	  m_fUsePassiveGauge( rhs.m_fUsePassiveGauge ),
	  m_ACList( rhs.m_ACList ),
	  m_szWoundEffect( rhs.m_szWoundEffect ),
	  m_fOutLineR(rhs.m_fOutLineR),
	  m_fOutLineG(rhs.m_fOutLineG),
	  m_fOutLineB(rhs.m_fOutLineB),
	  m_fOutLineAlpha(rhs.m_fOutLineAlpha),
	  m_fOutLineThickness(rhs.m_fOutLineThickness),
	  m_bEnableOutLine(rhs.m_bEnableOutLine)
{
	m_iCurActiveCount = 0;
	m_dwEffectID = -1;
}

ioStateProtectionByPassiveBuff::~ioStateProtectionByPassiveBuff()
{
}

void ioStateProtectionByPassiveBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	m_fDamageRate = rkLoader.LoadFloat( "damage_rate", 0.0f );
	m_bNoBlockState = rkLoader.LoadBool_e( "no_block_state", false );
	m_bNoDefenseBreakState = rkLoader.LoadBool_e( "no_defense_break_state", false );

	m_bEnableOutLine = rkLoader.LoadBool_e( "outline_enable", false );
	m_fOutLineR = rkLoader.LoadFloat_e( "outline_color_red", FLOAT1 );
	m_fOutLineG = rkLoader.LoadFloat_e( "outline_color_green", FLOAT1 );
	m_fOutLineB = rkLoader.LoadFloat_e( "outline_color_blue", 0.0f );
	m_fOutLineAlpha = rkLoader.LoadFloat_e( "outline_color_alpha", FLOAT05 );
	m_fOutLineThickness = rkLoader.LoadFloat_e( "outline_thickness", 2.0f );

	m_ProtectSide = (AttackSide)rkLoader.LoadInt_e( "protect_side", AS_NONE );
	m_bDisableDownState = rkLoader.LoadBool_e( "disable_down_state", false );

	m_HPCancelType = (HPProtectType)rkLoader.LoadInt_e( "zero_hp_cancel_type", HPT_ZERO_CANCEL );

	char szBuf[MAX_PATH], szKey[MAX_PATH];
	rkLoader.LoadString_e( "check_passive_skill", "", szBuf, MAX_PATH );
	m_szPassiveSkillName = szBuf;

	rkLoader.LoadString_e( "wound_effect", "", szBuf, MAX_PATH );
	m_szWoundEffect = szBuf;
	
	m_fUsePassiveGauge = rkLoader.LoadFloat_e( "use_passive_gauge", 0.0f );

	m_ACList.clear();
	m_ACList.reserve( m_iMaxActiveCount );
	for (int i=0; i<m_iMaxActiveCount; i++)
	{
		ActiveCountInfo sACInfo;
		wsprintf( szKey, "active_count%d_min", i+1 );
		sACInfo.fMinRate = rkLoader.LoadFloat( szKey, 0.0f );
		wsprintf( szKey, "active_count%d_max", i+1 );
		sACInfo.fMaxRate = rkLoader.LoadFloat( szKey, 0.0f );
		wsprintf( szKey, "active_count%d_cnt", i+1 );
		sACInfo.nActiveCnt = rkLoader.LoadInt( szKey, 0 );
		wsprintf( szKey, "active_count%d_effect", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		sACInfo.szEffect = szBuf;

		m_ACList.push_back( sACInfo );
	}
}

ioBuff* ioStateProtectionByPassiveBuff::Clone()
{
	return new ioStateProtectionByPassiveBuff( *this );
}

void ioStateProtectionByPassiveBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );	
	
	if( m_bEnableOutLine )
	{
		D3DCOLORVALUE kColor;
		Help::InitColorValue( kColor, m_fOutLineR, m_fOutLineG, m_fOutLineB, m_fOutLineAlpha );
		m_pOwner->ApplyOutLineChange( COT_BUFF_PROTECTION, kColor, m_fOutLineThickness );
	}	

	m_iCurActiveCount = 0;
	m_dwEffectID = -1;
}

bool ioStateProtectionByPassiveBuff::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;
	
	if( m_bEnableOutLine )
	{
		D3DCOLORVALUE kColor;
		Help::InitColorValue( kColor, m_fOutLineR, m_fOutLineG, m_fOutLineB, m_fOutLineAlpha );
		m_pOwner->ApplyOutLineChange( COT_BUFF_PROTECTION, kColor, m_fOutLineThickness );
	}

	m_iCurActiveCount = 0;
	m_dwEffectID = -1;

	return true;
}

void ioStateProtectionByPassiveBuff::EndBuff()
{
	if( m_bEnableOutLine )
	{
		m_pOwner->RestoreOutLineChange( COT_BUFF_PROTECTION );
	}

	//이펙트 해제
	if ( m_dwEffectID != -1)
	{
		m_pOwner->EndEffect( m_dwEffectID );
		m_dwEffectID = -1;
	}

	ioBuff::EndBuff();	
}

void ioStateProtectionByPassiveBuff::ProcessBuff( float fTimePerSec )
{
	CheckPassiveGauge( m_pOwner );
}

float ioStateProtectionByPassiveBuff::GetProtectionRate()
{
	return m_fDamageRate;
}

//카운트 감소
void ioStateProtectionByPassiveBuff::CheckActiveCount()
{
	if ( m_iCurActiveCount <= 0 )
		return;

	m_iCurActiveCount--;
	m_iCurActiveCount = max( m_iCurActiveCount, 0 );
	
	if ( m_pOwner )
	{
		//이펙트 변경(카운트 소비시)
		int nACCnt = (int)m_ACList.size();
		if ( m_iCurActiveCount > 0 && m_iCurActiveCount < nACCnt )
		{
			const ActiveCountInfo &sACInfo = m_ACList[m_iCurActiveCount-1];		
			SetEffect( m_pOwner, sACInfo.szEffect );
		}
		else if ( m_iCurActiveCount == 0 )
		{
			if ( m_dwEffectID != -1)
			{
				m_pOwner->EndEffect( m_dwEffectID );
				m_dwEffectID = -1;
			}
		}
		
		//피격 이펙트 생성
		ioPlayStage *pStage = m_pOwner->GetCreator();
		if ( pStage )
		{
			D3DXVECTOR3 vPos = m_pOwner->GetMidPositionByRate();
			D3DXVECTOR3 vScale = D3DXVECTOR3(1.0f, FLOAT1, FLOAT1);
			pStage->CreateMapEffect( m_szWoundEffect, vPos, vScale );
		}

		m_pOwner->UsePassiveSkillGauge( m_szPassiveSkillName, m_fUsePassiveGauge );
	}	
}

//방어 가능한 상태인가
bool ioStateProtectionByPassiveBuff::IsProtectState() const
{
	if ( m_iCurActiveCount > 0 )
	{
		return true;
	}

	return false;
}

//패시브 게이지 확인
void ioStateProtectionByPassiveBuff::CheckPassiveGauge( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;
	
	float fGaugeRate = m_pOwner->GetPassiveSkillGaugeRate( m_szPassiveSkillName );

	int nACCnt = (int)m_ACList.size();
	for (int i=0;i<nACCnt; i++)
	{
		const ActiveCountInfo &sACInfo = m_ACList[i];
		if ( sACInfo.fMinRate <= fGaugeRate && sACInfo.fMaxRate > fGaugeRate )
		{
			if ( m_iCurActiveCount != sACInfo.nActiveCnt )
			{
				m_iCurActiveCount = sACInfo.nActiveCnt;
				SetEffect( pOwner, sACInfo.szEffect );
				return;
			}
		}
	}	
}

void ioStateProtectionByPassiveBuff::SetEffect( ioBaseChar *pOwner, ioHashString szEffectName )
{
	if ( !pOwner )
		return;

	//이펙트 해제
	if ( m_dwEffectID != -1)
	{
		pOwner->EndEffect( m_dwEffectID	);
		m_dwEffectID = -1;
	}

	if ( szEffectName.IsEmpty() )
		return;

	ioEffect *pEffect = pOwner->AttachEffect( szEffectName );
	if ( pEffect )
	{
		m_dwEffectID = pEffect->GetUniqueID();
	}
}
