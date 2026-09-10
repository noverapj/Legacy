

#include "stdafx.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

#include "ioCrecendoSkill.h"

ioCrecendoSkill::ioCrecendoSkill()
{
	ClearData();
}

ioCrecendoSkill::ioCrecendoSkill( const ioCrecendoSkill &rhs )
: ioAttackSkill( rhs ),
m_dwLoopDuration( rhs.m_dwLoopDuration ),
 m_SkillLoopMotion( rhs.m_SkillLoopMotion ),
 m_SkillLoopMotionRate( rhs.m_SkillLoopMotionRate ),
 m_SoundWaveKeyVec( rhs.m_SoundWaveKeyVec ),
 m_szUpKeyEffect( rhs.m_szUpKeyEffect ),
 m_szDownKeyEffect( rhs.m_szDownKeyEffect ),
 m_szLeftKeyEffect( rhs.m_szLeftKeyEffect ),
 m_szRightKeyEffect( rhs.m_szRightKeyEffect ),
 m_szLastKeyEffect( rhs.m_szLastKeyEffect ),
 m_szUpKeyFailedEffect( rhs.m_szUpKeyFailedEffect ),
 m_szDownKeyFailedEffect( rhs.m_szDownKeyFailedEffect ),
 m_szLeftKeyFailedEffect( rhs.m_szLeftKeyFailedEffect ),
 m_szRightKeyFailedEffect( rhs.m_szRightKeyFailedEffect ),
 m_szLastKeyFailedEffect( rhs.m_szLastKeyFailedEffect ),
 m_fEffectYGap( rhs.m_fEffectYGap ),
 m_fEffectXGap( rhs.m_fEffectXGap ),
 m_fEffectKeyWidth( rhs.m_fEffectKeyWidth ),
 m_fLastEffectYGap( rhs.m_fLastEffectYGap ),
 m_LastSoundWaveKey( rhs.m_LastSoundWaveKey ),
 m_dwSkillProtectTime( rhs.m_dwSkillProtectTime )
{
	ClearData();
}

ioCrecendoSkill::~ioCrecendoSkill()
{
}

void ioCrecendoSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];
	rkLoader.LoadString_e( "skill_loop_motion", "", szBuf, MAX_PATH );
	m_SkillLoopMotion = szBuf;
	m_SkillMotionName = m_SkillLoopMotion;

	m_SkillLoopMotionRate = rkLoader.LoadFloat_e( "skill_loop_motion_rate", FLOAT1 );

	rkLoader.LoadString_e( "up_key_effect", "", szBuf, MAX_PATH );
	m_szUpKeyEffect = szBuf;
	rkLoader.LoadString_e( "down_key_effect", "", szBuf, MAX_PATH );
	m_szDownKeyEffect = szBuf;
	rkLoader.LoadString_e( "left_key_effect", "", szBuf, MAX_PATH );
	m_szLeftKeyEffect = szBuf;
	rkLoader.LoadString_e( "right_key_effect", "", szBuf, MAX_PATH );
	m_szRightKeyEffect = szBuf;
	rkLoader.LoadString_e( "last_key_effect", "", szBuf, MAX_PATH );
	m_szLastKeyEffect = szBuf;

	rkLoader.LoadString_e( "up_key_failed_effect", "", szBuf, MAX_PATH );
	m_szUpKeyFailedEffect = szBuf;
	rkLoader.LoadString_e( "down_key_failed_effect", "", szBuf, MAX_PATH );
	m_szDownKeyFailedEffect = szBuf;
	rkLoader.LoadString_e( "left_key_failed_effect", "", szBuf, MAX_PATH );
	m_szLeftKeyFailedEffect = szBuf;
	rkLoader.LoadString_e( "right_key_failed_effect", "", szBuf, MAX_PATH );
	m_szRightKeyFailedEffect = szBuf;
	rkLoader.LoadString_e( "last_key_failed_effect", "", szBuf, MAX_PATH );
	m_szLastKeyFailedEffect = szBuf;

	m_fEffectYGap     = rkLoader.LoadFloat_e( "key_effect_y_gap", 0 );
	m_fEffectXGap     = rkLoader.LoadFloat_e( "key_effect_x_gap", 0 );
	m_fEffectKeyWidth = rkLoader.LoadFloat_e( "key_effect_key_width", 0 );

	m_fLastEffectYGap = rkLoader.LoadFloat_e( "last_key_effect_y_gap", 0 );

	m_dwLoopDuration  = rkLoader.LoadInt_e( "skill_loop_duration", 0 );

	int iWaveCount = rkLoader.LoadInt_e( "sound_wave_count", 0 );
	if( iWaveCount == 0 )
		return;

	m_SoundWaveKeyVec.reserve( iWaveCount );
	for( int iWave = 0; iWave < iWaveCount; ++iWave )
	{
		WaveKey WaveInfo;
	
		sprintf_e( szBuf, "sound_wave%d_key", iWave + 1 );
		WaveInfo.m_dwKey = rkLoader.LoadInt( szBuf, (int)WKS_NONE );

		sprintf_e( szBuf, "sound_wave%d_start_time", iWave + 1 );
		WaveInfo.m_dwStartTime = rkLoader.LoadInt( szBuf, 0 );

		sprintf_e( szBuf, "sound_wave%d_end_time",  iWave + 1 );
		WaveInfo.m_dwEndTime = rkLoader.LoadInt( szBuf, 0 );			
		
		sprintf_e( szBuf, "sound_wave%d_offset",  iWave + 1 );
		WaveInfo.m_fOffset = rkLoader.LoadFloat( szBuf, 0 );

		sprintf_e( szBuf, "sound_wave%d_need_gauge",  iWave + 1 );		
		WaveInfo.m_fNeedGague = rkLoader.LoadFloat( szBuf, 0 );

		sprintf_e( szBuf, "sound_wave%d_attack",  iWave + 1 );
		LoadForceInfo( szBuf,  WaveInfo.m_WaveAttack, rkLoader );
		LoadWeaponInfo( szBuf, WaveInfo.m_WaveAttack, rkLoader );
		
		m_SoundWaveKeyVec.push_back( WaveInfo );
	}

	m_LastSoundWaveKey.m_dwKey		  = rkLoader.LoadInt_e( "last_sound_wave_key", (int)WKS_LAST );	
	m_LastSoundWaveKey.m_dwStartTime  = rkLoader.LoadInt_e( "last_sound_wave_start_time", 0 );
	m_LastSoundWaveKey.m_dwEndTime    = rkLoader.LoadInt_e( "last_sound_wave_end_time", 0 );		
	m_LastSoundWaveKey.m_fOffset      = rkLoader.LoadFloat_e( "last_sound_wave_offset", 0 );	
	m_LastSoundWaveKey.m_fNeedGague   = rkLoader.LoadFloat_e( "last_sound_wave_need_gauge", 0 );
	m_LastSoundWaveKey.m_fTargetRange = rkLoader.LoadFloat_e( "last_sound_wave_auto_target_range", 0 );

	sprintf_e( szBuf, "last_sound_wave_attack",  iWave + 1 );
	LoadForceInfo( szBuf,  m_LastSoundWaveKey.m_WaveAttack, rkLoader );
	LoadWeaponInfo( szBuf, m_LastSoundWaveKey.m_WaveAttack, rkLoader );

	m_dwSkillProtectTime = rkLoader.LoadInt_e( "skill_protect_time", 0 );
}

ioSkill* ioCrecendoSkill::Clone()
{
	return new ioCrecendoSkill( *this );
}

void ioCrecendoSkill::ClearData()
{
	m_SkillState   = SS_DELAY;
	m_bSetHandMesh = false;

	m_dwSkillStartTime = 0;
	m_dwLoopStartTime  = 0;
	m_dwEndTime        = 0;

	m_iCurSoundWayIdx  = 0;
	m_dwCurSkillProtectTime = 0;
}

bool ioCrecendoSkill::OnSkillStart( ioBaseChar *pChar )
{
	ioAttackSkill::OnSkillStart( pChar );

	ClearData();
	
	m_dwSkillStartTime = FRAMEGETTIME();

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}

	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}	
	
	if( m_dwSkillProtectTime > 0 )
		m_dwCurSkillProtectTime = FRAMEGETTIME() + m_dwSkillProtectTime;

	pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge( pChar->GetCharName() ) );

	return true;
}

bool ioCrecendoSkill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

void ioCrecendoSkill::SetLoopState( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	m_SkillState = SS_LOOP;	

	ioEntityGroup *pGrp = pChar->GetGroup();

	int iAniID = pGrp->GetAnimationIdx( m_SkillLoopMotion );
	float fAniRate = FLOAT1;
	if( m_SkillLoopMotionRate > 0.0f )
		fAniRate = m_SkillLoopMotionRate;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fAniRate );

	m_dwLoopStartTime = FRAMEGETTIME();	
}

void ioCrecendoSkill::SetEndState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	m_SkillState = SS_END;	

	pChar->ClearSkillReserveKey();
	pChar->ClearReserveKeyInput();

	m_dwEndTime = FRAMEGETTIME();
	ChangeFailedKeyEffect( pChar, pStage );

#ifdef _DEBUG
	LOG.PrintTimeAndLog(0, "%s - %d", __FUNCTION__, m_dwEndTime );
#endif

	if( pChar->IsNeedProcess() && pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SSC_END;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioCrecendoSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwPreDelay = GetPreDelayTime();

#ifdef _DEBUG
	DWORD dwCurTime = FRAMEGETTIME();
	if( GetKey( pChar ) != WKS_NONE )
	{
		LOG.PrintTimeAndLog(0, "---------------------------------------------------------------------");
		LOG.PrintTimeAndLog(0, "ioCrecendoSkill - Key : %d, Input Time : %d", GetKey( pChar ), dwCurTime );
		LOG.PrintTimeAndLog(0, "---------------------------------------------------------------------");
	}

	if( GetMixKey( pChar ) != WKS_NONE )
	{
		LOG.PrintTimeAndLog(0, "---------------------------------------------------------------------");
		LOG.PrintTimeAndLog(0, "ioCrecendoSkill - Last Key : %d, Input Time : %d", GetMixKey( pChar ), dwCurTime );
		LOG.PrintTimeAndLog(0, "---------------------------------------------------------------------");
	}
	
#endif

	ProcessKeyEffect( pChar, pStage );

	switch( m_SkillState )
	{
	case SS_DELAY:
		if( m_dwPreDelayStartTime + dwPreDelay < FRAMEGETTIME() )
		{
			m_dwPreDelayStartTime = 0;
			EndPreDelayEffect( pChar );

			SetLoopState( pChar );
			CreateKeyRandom( pChar, pStage );
			CreateKeyEffectByWaveKey( pChar, pStage );
		}
		break;
	case SS_LOOP:
		{
			PrcoessLoopState( pChar, pStage );
		}
		break;
	case SS_END:
		{
		}
		break;
	}
}

void ioCrecendoSkill::PrcoessLoopState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar->IsNeedProcess() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( 0 < m_dwLoopStartTime && m_dwLoopStartTime + m_dwLoopDuration < dwCurTime )
	{		
		SetEndState( pChar, pStage );
	}
		
	if( m_iCurSoundWayIdx < (int)m_SoundWaveKeyVec.size() )
	{
		//일반적인 방향키 입력일 경우
		ProcessKeyInput( pChar, pStage, dwCurTime );
	}
	else
	{
		//마지막 조합 키 입력 일경우
		ProcessLastKeyInput( pChar, pStage, dwCurTime );
	}
}

void ioCrecendoSkill::ProcessKeyInput( ioBaseChar *pChar, ioPlayStage *pStage, DWORD dwCurTime )
{
	const WaveKey& rkInfo = m_SoundWaveKeyVec[m_iCurSoundWayIdx];
	if( m_dwLoopStartTime + rkInfo.m_dwStartTime <= dwCurTime && dwCurTime <= m_dwLoopStartTime + rkInfo.m_dwEndTime )
	{
		if( GetKey( pChar ) == WKS_NONE )
			return;

		DWORD dwCurKey = rkInfo.m_dwKey;
		if( dwCurKey == WKS_NONE )
			dwCurKey = rkInfo.m_dwRandKey;

		if( GetKey( pChar ) == dwCurKey )
		{
			pChar->ReduceCurSkillGaugeBySkill( NULL, rkInfo.m_fNeedGague );
			RemoveKeyEffect( pChar );
			AttackFire( pChar, rkInfo.m_WaveAttack, pChar->GetWorldPosition() );
			m_iCurSoundWayIdx++;
		}
		else
		{			
			SetEndState( pChar, pStage );
		}
	}
	else if( m_dwLoopStartTime + rkInfo.m_dwEndTime < dwCurTime )
	{
		SetEndState( pChar, pStage );
	}
}

void ioCrecendoSkill::ProcessLastKeyInput( ioBaseChar *pChar, ioPlayStage *pStage, DWORD dwCurTime )
{
	if( m_dwLoopStartTime + m_LastSoundWaveKey.m_dwStartTime <= dwCurTime && dwCurTime <= m_dwLoopStartTime + m_LastSoundWaveKey.m_dwEndTime )
	{
		if( GetMixKey( pChar ) == WKS_NONE )
			return;

		DWORD dwCurKey = m_LastSoundWaveKey.m_dwKey;
		if( dwCurKey == WKS_NONE )
			dwCurKey = m_LastSoundWaveKey.m_dwRandKey;

		if( GetMixKey( pChar ) == dwCurKey )
		{
			pChar->ReduceCurSkillGaugeBySkill( NULL, m_LastSoundWaveKey.m_fNeedGague );

			ioBaseChar* pTarget = FindTarget( pChar, pStage, m_LastSoundWaveKey.m_fTargetRange );
			if( pTarget )
			{
				AttackFire( pChar, m_LastSoundWaveKey.m_WaveAttack, pTarget->GetWorldPosition() );
			}
			else
			{
				D3DXVECTOR3 vDir     = pChar->GetTargetRot() * D3DXVECTOR3(0.0f, 0.0f, -FLOAT1);
				D3DXVECTOR3 vFirePos = pChar->GetWorldPosition() + m_LastSoundWaveKey.m_fOffset * vDir;
				AttackFire( pChar, m_LastSoundWaveKey.m_WaveAttack, vFirePos );
			}

			RemoveLastKeyEffect( pChar );
			SetEndState( pChar, pStage );
		}
		else if( m_dwLoopStartTime + m_LastSoundWaveKey.m_dwEndTime < dwCurTime )
		{			
			SetEndState( pChar, pStage );
		}
	}
}

void ioCrecendoSkill::ProcessKeyEffect( ioBaseChar* pChar, ioPlayStage *pStage )
{
	if( !pChar )
		return;

	if( !pStage )
		return;

	if( !pChar->IsNeedProcess() ) 
		return;

	D3DXVECTOR3 vPos = pChar->GetWorldPosition();

	float fWidth = m_KeyMapEffectVec.size() * m_fEffectKeyWidth * FLOAT05 + m_KeyMapEffectVec.size()-1 * m_fEffectXGap;

	int i = 0;
	for( KeyMapEffectVec::const_iterator iter = m_KeyMapEffectVec.begin(); iter != m_KeyMapEffectVec.end(); ++iter, ++i )
	{
		const KeyMapEffect& rkInfo = *iter;
		if( !COMPARE( rkInfo.m_dwKey, WKS_UP, WKS_MAX ) )
			continue;

		ioMapEffect* pMapEffect = pStage->FindMapEffect( rkInfo.m_MapEffectID );
		if( pMapEffect )
		{
			float fXPos = vPos.x - fWidth + m_fEffectKeyWidth * i;
			if( i > 0 )
				fXPos += m_fEffectXGap;

			D3DXVECTOR3 vEffectPos( fXPos, vPos.y + m_fEffectYGap, vPos.z );
			pMapEffect->SetWorldPosition( vEffectPos );
			++i;
		}
	}

	ioMapEffect* pMapEffect = pStage->FindMapEffect( m_LastKeyMapEffect.m_MapEffectID );
	if( pMapEffect )
	{
		D3DXVECTOR3 vEffectPos( vPos.x, vPos.y + m_fLastEffectYGap, vPos.z );
		pMapEffect->SetWorldPosition( vEffectPos );
	}
}

void ioCrecendoSkill::CreateKeyRandom( ioBaseChar *pChar, ioPlayStage *pStage )
{
	 if( !pChar )
		 return;

	 if( !pStage )
		 return;

	 if( !pChar->IsNeedProcess() ) 
		 return;

	 srand( timeGetTime() );
	 for( WaveKeyVec::iterator iter = m_SoundWaveKeyVec.begin(); iter != m_SoundWaveKeyVec.end(); ++iter )
	 {
		 WaveKey& rkKey = *iter;

		 if( rkKey.m_dwKey == WKS_NONE )		 
			 rkKey.m_dwRandKey = rand() % WKS_LEFT + 1;
	 }

#ifdef _DEBUG
	 DWORD dwCurTime = FRAMEGETTIME();
	 LOG.PrintTimeAndLog(0, "--key time table-----------------------------------------------------");
	 for( WaveKeyVec::iterator iter = m_SoundWaveKeyVec.begin(); iter != m_SoundWaveKeyVec.end(); ++iter )
	 {
		 WaveKey& rkKey = *iter;
		 LOG.PrintTimeAndLog(0, "key : %d, start time : %d, end time : %d", rkKey.m_dwKey, rkKey.m_dwStartTime + dwCurTime, rkKey.m_dwEndTime + dwCurTime );		 
	 }

	 LOG.PrintTimeAndLog(0, " last key : %d, start time : %d, end time : %d", m_LastSoundWaveKey.m_dwKey, m_LastSoundWaveKey.m_dwStartTime + dwCurTime, m_LastSoundWaveKey.m_dwEndTime + dwCurTime );	
	 LOG.PrintTimeAndLog(0, "---------------------------------------------------------------------");
#endif
 }

void ioCrecendoSkill::CreateKeyEffectByWaveKey( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar )
		return;

	if( !pStage )
		return;

	if( !pChar->IsNeedProcess() ) 
		return;
		
	for( WaveKeyVec::const_iterator iter = m_SoundWaveKeyVec.begin(); iter != m_SoundWaveKeyVec.end(); ++iter )
	{
		const WaveKey rkKey = *iter;

		DWORD dwCurKey = rkKey.m_dwKey;
		if( dwCurKey == WKS_NONE )
			dwCurKey = rkKey.m_dwRandKey;

		if( !COMPARE( dwCurKey, WKS_UP, WKS_MAX ) )
			continue;
		
		KeyMapEffect KeyEffect;
		KeyEffect.m_dwKey       = dwCurKey;
		KeyEffect.m_MapEffectID = CreateKeyEffect( pChar, pStage, dwCurKey );			
		m_KeyMapEffectVec.push_back( KeyEffect );
	}
	
	DWORD dwCurKey = m_LastSoundWaveKey.m_dwKey;
	if( dwCurKey == WKS_NONE )
		dwCurKey = m_LastSoundWaveKey.m_dwRandKey;

	m_LastKeyMapEffect.Clear();
	m_LastKeyMapEffect.m_dwKey       = dwCurKey;
	m_LastKeyMapEffect.m_MapEffectID = CreateKeyEffect( pChar, pStage, dwCurKey );
	
}

UniqueObjID ioCrecendoSkill::CreateKeyEffect( ioBaseChar *pChar, ioPlayStage *pStage, DWORD dwCurKey, bool bFailed )
{
	switch( dwCurKey )
	{
	case WKS_UP:
		{
			if( bFailed )
			{
				ioMapEffect* pMapEffect = pStage->CreateMapEffect( m_szUpKeyFailedEffect, ioMath::VEC3_ZERO, D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 ) );
				if( pMapEffect )
				{
					return pMapEffect->GetUniqueID();
				}
			}
			else
			{
				ioMapEffect* pMapEffect = pStage->CreateMapEffect( m_szUpKeyEffect, ioMath::VEC3_ZERO, D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 ) );
				if( pMapEffect )
				{
					return pMapEffect->GetUniqueID();
				}
			}
		}
		break;
	case WKS_DOWN:
		{
			if( bFailed )
			{
				ioMapEffect* pMapEffect = pStage->CreateMapEffect( m_szDownKeyFailedEffect, ioMath::VEC3_ZERO, D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 ) );
				if( pMapEffect )
				{
					return pMapEffect->GetUniqueID();
				}
			}
			else
			{
				ioMapEffect* pMapEffect = pStage->CreateMapEffect( m_szDownKeyEffect, ioMath::VEC3_ZERO, D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 ) );
				if( pMapEffect )
				{
					return pMapEffect->GetUniqueID();
				}
			}
		}
		break;
	case WKS_LEFT:
		{
			if( bFailed )
			{
				ioMapEffect* pMapEffect = pStage->CreateMapEffect( m_szLeftKeyFailedEffect, ioMath::VEC3_ZERO, D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 ) );
				if( pMapEffect )
				{
					return pMapEffect->GetUniqueID();
				}
			}
			else
			{
				ioMapEffect* pMapEffect = pStage->CreateMapEffect( m_szLeftKeyEffect, ioMath::VEC3_ZERO, D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 ) );
				if( pMapEffect )
				{
					return pMapEffect->GetUniqueID();
				}
			}
		}
		break;
	case WKS_RIGHT:
		{
			if( bFailed )
			{
				ioMapEffect* pMapEffect = pStage->CreateMapEffect( m_szRightKeyFailedEffect, ioMath::VEC3_ZERO, D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 ) );
				if( pMapEffect )
				{
					return pMapEffect->GetUniqueID();
				}
			}
			else
			{
				ioMapEffect* pMapEffect = pStage->CreateMapEffect( m_szRightKeyEffect, ioMath::VEC3_ZERO, D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 ) );
				if( pMapEffect )
				{
					return pMapEffect->GetUniqueID();
				}
			}
		}
		break;
	case WKS_LAST:
		{
			if( bFailed )
			{
				ioMapEffect* pMapEffect = pStage->CreateMapEffect( m_szLastKeyFailedEffect, ioMath::VEC3_ZERO, D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 ) );
				if( pMapEffect )
				{
					return pMapEffect->GetUniqueID();
				}
			}
			else
			{
				ioMapEffect* pMapEffect = pStage->CreateMapEffect( m_szLastKeyEffect, ioMath::VEC3_ZERO, D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 ) );
				if( pMapEffect )
				{
					return pMapEffect->GetUniqueID();
				}
			}
		}
		break;
	}

	return -1;
}

void ioCrecendoSkill::ChangeFailedKeyEffect( ioBaseChar* pChar, ioPlayStage *pStage )
{
	if( !m_KeyMapEffectVec.empty() )
	{
		const KeyMapEffect& rkInfo = *m_KeyMapEffectVec.begin();
		
		KeyMapEffect KeyEffect;
		KeyEffect.m_dwKey       = rkInfo.m_dwKey;
		KeyEffect.m_MapEffectID = CreateKeyEffect( pChar, pStage, KeyEffect.m_dwKey, true );
		RemoveKeyEffect( pChar );		
		m_KeyMapEffectVec.insert( m_KeyMapEffectVec.begin(), KeyEffect );		
	}
	else if( m_LastKeyMapEffect.m_MapEffectID != -1 )
	{
		KeyMapEffect KeyEffect;
		KeyEffect.m_dwKey       = m_LastKeyMapEffect.m_dwKey;
		KeyEffect.m_MapEffectID = CreateKeyEffect( pChar, pStage, KeyEffect.m_dwKey, true );
		RemoveLastKeyEffect( pChar );
		m_LastKeyMapEffect = KeyEffect;
	}
}

void ioCrecendoSkill::RemoveKeyEffect( ioBaseChar* pChar )
{
	if( !m_KeyMapEffectVec.empty() )
	{
		const KeyMapEffect& rkInfo = *m_KeyMapEffectVec.begin();
		if( pChar )
		{
			pChar->EndMapEffect( rkInfo.m_MapEffectID, false );
			m_KeyMapEffectVec.erase( m_KeyMapEffectVec.begin() );
		}
	}
}

void ioCrecendoSkill::RemoveLastKeyEffect( ioBaseChar* pChar )
{
	if( pChar )
	{
		pChar->EndMapEffect( m_LastKeyMapEffect.m_MapEffectID, false );	
		m_LastKeyMapEffect.Clear();
	}
}

void ioCrecendoSkill::DestroyKeyEffect( ioBaseChar* pChar )
{
	KeyMapEffectVec::iterator iter = m_KeyMapEffectVec.begin();
	for( ; iter != m_KeyMapEffectVec.end(); ++iter )
	{
		KeyMapEffect& rkInfo = *iter;
		if( pChar )
			pChar->EndMapEffect( rkInfo.m_MapEffectID, false );
	}

	m_KeyMapEffectVec.clear();

	if( pChar )
		pChar->EndMapEffect( m_LastKeyMapEffect.m_MapEffectID, false );

	m_LastKeyMapEffect.Clear();
}

DWORD ioCrecendoSkill::GetKey( ioBaseChar *pChar )
{
	if( !pChar )
		return WKS_NONE;

	if( pChar->IsUpKeyRelease() && !pChar->IsDownKeyRelease() && !pChar->IsLeftKeyRelease() && !pChar->IsRightKeyRelease() )
	{
		return WKS_UP;
	}
	else if( !pChar->IsUpKeyRelease() && pChar->IsDownKeyRelease() && !pChar->IsLeftKeyRelease() && !pChar->IsRightKeyRelease() )
	{
		return WKS_DOWN;
	}
	else if( !pChar->IsUpKeyRelease() && !pChar->IsDownKeyRelease() && pChar->IsLeftKeyRelease() && !pChar->IsRightKeyRelease() )
	{
		return WKS_LEFT;
	}
	else if( !pChar->IsUpKeyRelease() && !pChar->IsDownKeyRelease() && !pChar->IsLeftKeyRelease() && pChar->IsRightKeyRelease() )
	{
		return WKS_RIGHT;
	}

	return WKS_NONE;
}

DWORD ioCrecendoSkill::GetMixKey( ioBaseChar *pChar )
{
	ioUserKeyInput::DirMixInput eMixKey = pChar->GetCurDirMixKey();	
	switch( eMixKey )
	{
	case ioUserKeyInput::DMI_LEFT_UP_RIGHT:
		return WKS_LAST;
	}

	return WKS_NONE;
}

ioBaseChar* ioCrecendoSkill::FindTarget( ioBaseChar *pChar, ioPlayStage* pStage, float fTargetRange )
{
	if( !pChar )
		return NULL;

	if( !pStage )
		return NULL;

	const BaseCharList &rkCharList = pStage->GetBaseCharList();
	D3DXVECTOR3 vPos = pChar->GetWorldPosition();

	ioBaseChar* pRetTarget = NULL;
	BaseCharList::const_iterator iter;
	for( iter=rkCharList.begin() ; iter!=rkCharList.end() ; ++iter )
	{
		ioBaseChar *pTarget = *iter;

		if( pTarget == pChar )
			continue;

		if( pTarget->GetTeam() == pChar->GetTeam() )
			continue;

		if( pTarget->IsSystemState() )
			continue;
		
		if( !pTarget->IsEnableTarget() ) 
			continue;


		D3DXVECTOR3 vDistNewTarget = vPos - pTarget->GetWorldPosition();
		float fNewDistSq = D3DXVec3Length( &vDistNewTarget );
		if( fNewDistSq > fTargetRange )
			continue;

		if( pRetTarget )
		{
			D3DXVECTOR3 vDistOldTarget = vPos - pRetTarget->GetWorldPosition();			
			float fOldDistSq = D3DXVec3Length( &vDistOldTarget );			
			if( fOldDistSq > fNewDistSq )
			{
				pRetTarget = pTarget;
			}
		}
		else
		{
			pRetTarget =  pTarget;
		}
	}

	return pRetTarget;
}

void ioCrecendoSkill::AttackFire( ioBaseChar* pChar, const AttackAttribute& Attack, const D3DXVECTOR3& vFirePos )
{
	for( WeaponInfoList::const_iterator iter = Attack.m_vWeaponInfoList.begin(); iter != Attack.m_vWeaponInfoList.end(); ++iter )
	{
		DWORD dwCurBaseIndex = pChar->IncreaseWeaponIndexBase();
		pChar->SkillFireExplicit( (*iter),	GetName(), dwCurBaseIndex,	vFirePos );

		if( pChar->IsNeedProcess() && pChar->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
			kPacket << pChar->GetCharName();
			kPacket << GetName();
			kPacket << SSC_ATTACK;
			kPacket << vFirePos;
			kPacket << m_iCurSoundWayIdx;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioCrecendoSkill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );
}

void ioCrecendoSkill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	DWORD dwCurTime = FRAMEGETTIME();
	int iType;
	rkPacket >> iType;
	switch( iType )
	{
	case SSC_ATTACK:
		{
			D3DXVECTOR3 vFirePos;
			rkPacket >> vFirePos;
			rkPacket >> m_iCurSoundWayIdx;
						
			if( m_iCurSoundWayIdx < (int)m_SoundWaveKeyVec.size() )
			{				
				AttackFire( pChar, m_SoundWaveKeyVec[m_iCurSoundWayIdx].m_WaveAttack, vFirePos );
			}
			else
			{
				AttackFire( pChar, m_LastSoundWaveKey.m_WaveAttack, vFirePos );
			}
		}
		break;
	case SSC_END:
		{
			SetEndState( pChar, pStage );
		}
		break;
	}
}

void ioCrecendoSkill::OnSkillEnd( ioBaseChar *pChar )
{
	ioAttackSkill::OnSkillEnd( pChar );

	if( GetOwnerItem() )
		GetOwnerItem()->ClearCurChargeRateForSkill();

	// 기본무기 장착
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( true );
	}

	if( !m_HandMesh.IsEmpty() )
	{
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
	}
	DestroyKeyEffect( pChar );	

	ClearData();
}

bool ioCrecendoSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_dwPreDelayStartTime > 0 )
		return false;

	switch( m_SkillState )
	{
	case SS_DELAY:
	case SS_LOOP:
		return false;
	case SS_END:
		if( FRAMEGETTIME() < m_dwEndTime + 500 )
		{
			return false;
		}
	}

	return true;
}

bool ioCrecendoSkill::IsProtected( int iDefenseBreakType ) const
{
	if( DBT_NONE != (DefenseBreakType)m_ProtectCancelType )
	{
		if( iDefenseBreakType == m_ProtectCancelType )   // 스킬 무적 파괴
			return false;
	}

	if( IsPreDelayProtect() )
	{
		if( m_dwPreDelayStartTime != 0 )
			return true;
	}

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwCurSkillProtectTime > dwCurTime )
	{
		return true;
	}

	return false;
}