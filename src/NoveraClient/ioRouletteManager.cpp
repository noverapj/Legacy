
#include "stdafx.h"

#include "ioRouletteManager.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

#include "GUI/RouletteWnd.h"
#include "GUI/RouletteHelpWnd.h"

template<> ioRouletteManager* Singleton< ioRouletteManager >::ms_Singleton = 0;

ioRouletteManager::ioRouletteManager()
{
	m_pWheel = NULL;
	m_pArrow = NULL;

	m_dwEffectDelayID = -1;
	m_dwEffectStartID = -1;
	m_dwEffectResultID = -1;

	InitForRestart();
}

ioRouletteManager::~ioRouletteManager()
{
}

ioRouletteManager& ioRouletteManager::GetSingleton()
{
	return Singleton< ioRouletteManager >::GetSingleton();
}

void ioRouletteManager::Initialize()
{
	if( m_pPlayStage )
	{
		if( m_dwEffectResultID != -1 )
			m_pPlayStage->DestroyMapEffect( m_dwEffectResultID );
		if( m_dwEffectStartID != -1 )
			m_pPlayStage->DestroyMapEffect( m_dwEffectStartID );
		if( m_dwEffectDelayID != -1 )
			m_pPlayStage->DestroyMapEffect( m_dwEffectDelayID );

		m_dwEffectDelayID  = -1;
		m_dwEffectStartID  = -1;
		m_dwEffectResultID = -1;
	}

	m_vRouletteList.clear();
	m_pPlayStage->DestroyAllRouletteEntity();

	m_RouletteState = RS_READY;
	m_ArrowState    = AS_NONE;

	m_dwEnableSpinStartTime = 0;
	m_dwSpinStartTime = 0;
	m_dwSpinEndTime = 0;

	m_fCurAngle = 0.0f;
	m_fResultMinAngle = 0.0f;
	m_fResultMaxAngle =FLOAT90;
	m_fResultAngle = 0.0f;

	m_fCurArrowAngle = 0.0f;

	m_fCurSpinSpeed = 0.0f;

	m_vJoinUser.clear();
	m_bEnableJoin = true;

	D3DXQUATERNION qtRot;
	D3DXQuaternionRotationAxis( &qtRot, &ioMath::UNIT_Z, 0.0f );

	m_qtOriginal = qtRot;
}

void ioRouletteManager::InitForRestart()
{
	m_RouletteState = RS_READY;
	m_ArrowState    = AS_NONE;

	m_dwEnableSpinStartTime = 0;
	m_dwSpinStartTime = 0;
	m_dwSpinEndTime = 0;

	//m_fCurAngle = 0.0f;
	m_fResultMinAngle = 0.0f;
	m_fResultMaxAngle =FLOAT90;
	m_fResultAngle = 0.0f;

	m_fCurArrowAngle = 0.0f;

	m_fCurSpinSpeed = 0.0f;

	m_vJoinUser.clear();
	m_bEnableJoin = true;

	D3DXQUATERNION qtRot;
	D3DXQuaternionRotationAxis( &qtRot, &ioMath::UNIT_Z, 0.0f );

	m_qtOriginal = qtRot;

	int iSize = m_vRouletteInfoList.size();
	for( int i=0; i<iSize; ++i )
	{
		RouletteInfo kInfo = m_vRouletteInfoList[i];
		ioRoulette *pRoulette = GetRouletteEntity( kInfo.m_iType, kInfo.m_iSubIndex );
		if( pRoulette )
		{
			D3DXVECTOR3 vPos = m_vRoulettePosition + kInfo.m_vOffset * m_vRouletteScale;
			D3DXVECTOR3 vScale = m_vRouletteScale * kInfo.m_vScale;
			pRoulette->SetWorldPosition( vPos );
			pRoulette->SetWorldScale( vScale );
		}
	}
}

void ioRouletteManager::LoadRoulette()
{
	ioINILoader_e kLoader( "config/sp2_roulette.ini" );

	char szBuf[MAX_PATH] = "", szKey[MAX_PATH] = "";

	kLoader.SetTitle_e( "common" );
	m_iNeedCoin = kLoader.LoadInt_e( "roulette_coin", 0 );
	m_dwSpinDuration = (DWORD)kLoader.LoadInt_e( "spin_duration", 0 );
	m_dwSpinStopTime = (DWORD)kLoader.LoadInt_e( "spin_stop_time", 0 );
	m_dwJoinEndTime = (DWORD)kLoader.LoadInt_e( "join_end_time", 0 );
	m_dwRestartTime = (DWORD)kLoader.LoadInt_e( "restart_time", 0 );
	m_fSpinSpeed = kLoader.LoadFloat_e( "spin_speed", 0.0f );
	m_fReduceRate = kLoader.LoadFloat_e( "reduce_rate", FLOAT1 );
	m_fEnableRange = kLoader.LoadFloat_e( "enable_start_range", 0.0f );
	m_fEnableAngle = kLoader.LoadFloat_e( "enable_start_angle", 0.0f );

	int iSize = kLoader.LoadInt_e( "roulette_count", 0 );
	m_vRouletteInfoList.clear();
	m_vRouletteInfoList.reserve( iSize );
	for( int i=0; i<iSize; ++i )
	{
		wsprintf_e( szKey, "roulette%d", i+1 );
		kLoader.SetTitle( szKey );

		char szStruct[MAX_PATH];
		kLoader.LoadString( "model", "", szStruct, MAX_PATH );

		int iType = kLoader.LoadInt_e( "type", 0 );
		int iSubIndex     = kLoader.LoadInt_e( "sub_index", 0 );

		D3DXVECTOR3 vOffset;
		vOffset.x = kLoader.LoadFloat_e( "offset_x", 0.0f );
		vOffset.y = kLoader.LoadFloat_e( "offset_y", 0.0f );
		vOffset.z = kLoader.LoadFloat_e( "offset_z", 0.0f );

		D3DXVECTOR3 vScale;
		vScale.x = kLoader.LoadFloat_e( "scale_x", FLOAT1 );
		vScale.y = kLoader.LoadFloat_e( "scale_y", FLOAT1 );
		vScale.z = kLoader.LoadFloat_e( "scale_z", FLOAT1 );

		RouletteInfo kInfo( iType, iSubIndex, szStruct, vOffset, vScale );
		m_vRouletteInfoList.push_back(kInfo);
	}

	kLoader.SetTitle_e( "result" );
	int iGroupSize = kLoader.LoadInt_e( "result_group_count", 0 );
	m_vResultGroupList.clear();
	m_vResultGroupList.reserve( iGroupSize );
	for( int i=0; i<iGroupSize; ++i )
	{
		ResultGroup kGroupInfo;
		wsprintf_e( szKey, "group%d_min_players", i+1 );
		kGroupInfo.m_iMinPlayers = kLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "group%d_max_players", i+1 );
		kGroupInfo.m_iMaxPlayers = kLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "group%d_result_count", i+1 );
		int iSize = kLoader.LoadInt( szKey, 0 );
		kGroupInfo.m_vRouletteResultList.clear();
		kGroupInfo.m_vRouletteResultList.reserve( iSize );
		IntVec vRateList;
		vRateList.clear();
		for( int j=0; j<iSize; ++j )
		{
			wsprintf_e( szKey, "group%d_result%d_rate", i+1, j+1 );
			int iRate = kLoader.LoadInt( szKey, 0 );
			vRateList.push_back( iRate );
		}

		float fPrevMin = 0.0f;
		for( int j=0; j<iSize; ++j )
		{
			float fMax = 0.0f, fMin = 0.0f;

			if( j == 0 )
			{
				fMin = 0.0f;
				fMax = 360.0f * (float)( vRateList[j] / FLOAT100 );
			}
			else
			{
				fMin = fPrevMin + 360.0f * (float)( vRateList[j-1] / FLOAT100 );
				fMax = fMin + 360.0f * (float)( vRateList[j] / FLOAT100 );
				fMax = min( fMax, 360.0f );
				fPrevMin = fMin;
			}

			D3DXQUATERNION qtRot;
			D3DXQuaternionRotationAxis( &qtRot, &ioMath::UNIT_Z, DEGtoRAD(fMin) );
			D3DXVECTOR3 vDir = ioMath::UNIT_Y;
			vDir = qtRot * vDir;
			D3DXVec3Normalize( &vDir, &vDir );
			ResultData kData( fMax, fMin, vDir, vRateList[j] );
			kGroupInfo.m_vRouletteResultList.push_back( kData );
		}

		m_vResultGroupList.push_back( kGroupInfo );
	}

	kLoader.SetTitle_e( "effect" );
	kLoader.LoadString( "button_on", "", szBuf, MAX_PATH );
	m_EffectButton = szBuf;
	kLoader.LoadString( "delay", "", szBuf, MAX_PATH );
	m_EffectDelay  = szBuf;
	kLoader.LoadString( "result", "", szBuf, MAX_PATH );
	m_EffectResult = szBuf;
	kLoader.LoadString( "start", "", szBuf, MAX_PATH );
	m_EffectStart  = szBuf;

	kLoader.LoadString( "event_start", "", szBuf, MAX_PATH );
	m_EventStartEffect  = szBuf;
	kLoader.LoadString( "event_end", "", szBuf, MAX_PATH );
	m_EventEndEffect    = szBuf;
}

void ioRouletteManager::LoadRouletteMapInfo( const char* szFileName )
{
	// 룰렛 회전판
	ioINILoader kLoader( szFileName );

	kLoader.SetTitle_e( "roulette" );

	m_vRoulettePosition.x = kLoader.LoadFloat_e( "position_x", 0.0f );
	m_vRoulettePosition.y = kLoader.LoadFloat_e( "position_y", 0.0f );
	m_vRoulettePosition.z = kLoader.LoadFloat_e( "position_z", 0.0f );

	m_vRouletteScale.x = kLoader.LoadFloat_e( "scale_x", FLOAT1 );
	m_vRouletteScale.y = kLoader.LoadFloat_e( "scale_y", FLOAT1 );
	m_vRouletteScale.z = kLoader.LoadFloat_e( "scale_z", FLOAT1 );
}

void ioRouletteManager::CreateRouletteAllEntity( ioPlayMode* pMode	)
{
	int iSize = m_vRouletteInfoList.size();
	m_vRouletteList.clear();
	m_vRouletteList.reserve( iSize );

	for( int i=0; i<iSize; ++i )
	{
		RouletteInfo kInfo = m_vRouletteInfoList[i];
		ioEntityGroup *pGrp = m_pPlayStage->CreateEntityGrpAndSceneNode( kInfo.m_ModelName, true );
		if( !pGrp )
		{
			LOG.PrintTimeAndLog( 0, "ioRouletteManager::CreateRouletteAllEntity - %s Fail!", kInfo.m_ModelName );
			continue;
		}
	
		if( kInfo.m_iType == RT_WHEEL && kInfo.m_iSubIndex != 0 )
			continue;
		else if( kInfo.m_iType == RT_PRIZE && kInfo.m_iSubIndex != 0 )
			continue;
		else if( kInfo.m_iType == RT_BUTTON )
		{
			if( g_MyInfo.GetEtcCoinMoney( ioEtcItem::EIT_ETC_ROULETTE_COIN ) > 0  && kInfo.m_iSubIndex == BT_OFF )
				continue;
			else if( g_MyInfo.GetEtcCoinMoney( ioEtcItem::EIT_ETC_ROULETTE_COIN ) <= 0 && kInfo.m_iSubIndex == BT_ON )
				continue;
		}

		ioRoulette *pRoulette = new ioRoulette( pGrp, pMode );
		if( pRoulette )
		{
			D3DXVECTOR3 vPos = m_vRoulettePosition + kInfo.m_vOffset * m_vRouletteScale;
			D3DXVECTOR3 vScale = m_vRouletteScale * kInfo.m_vScale;
			pRoulette->InitRouletteInfo( m_pPlayStage, kInfo.m_iType, kInfo.m_iSubIndex, vPos, vScale );
			pRoulette->SetShadowCastEnable( true );
			m_vRouletteList.push_back( pRoulette );
		}
	}
}

ioRoulette *ioRouletteManager::CreateRouletteEntity( ioPlayMode* pMode, int iType, int iSubIndex )
{
	if( !m_pPlayStage )
		return NULL;

	int iSize = m_vRouletteInfoList.size();
	
	for( int i=0; i<iSize; ++i )
	{
		RouletteInfo kInfo = m_vRouletteInfoList[i];
		if( kInfo.m_iType != iType || kInfo.m_iSubIndex != iSubIndex )
			continue;

		ioEntityGroup *pGrp = m_pPlayStage->CreateEntityGrpAndSceneNode( kInfo.m_ModelName, true );
		if( !pGrp )
		{
			LOG.PrintTimeAndLog( 0, "ioRouletteManager::CreateRouletteEntity - %s Fail!", kInfo.m_ModelName );
			continue;
		}

		ioRoulette *pRoulette = new ioRoulette( pGrp, pMode );
		if( pRoulette )
		{
			D3DXVECTOR3 vPos = m_vRoulettePosition + kInfo.m_vOffset * m_vRouletteScale;
			D3DXVECTOR3 vScale = m_vRouletteScale * kInfo.m_vScale;
			pRoulette->InitRouletteInfo( m_pPlayStage, kInfo.m_iType, kInfo.m_iSubIndex, vPos, vScale );

			m_vRouletteList.push_back( pRoulette );
			m_pPlayStage->AddGameEntity( pRoulette );

			return pRoulette;
		}
	}

	return NULL;
}

void ioRouletteManager::ProcessRoulette()
{
	if( !IsRouletteEvent() )
		return;

	switch( m_RouletteState )
	{
	case RS_READY:
		{
			if( g_MyInfo.GetEtcCoinMoney( ioEtcItem::EIT_ETC_ROULETTE_COIN ) > 0 )
			{
				ioRoulette *pRoulette = GetRouletteEntity( RT_BUTTON, BT_OFF );
				if( pRoulette )
				{
					EraseRouletteEntity( RT_BUTTON, BT_OFF );
					if( m_pPlayStage )
					{
						ioPlayMode *pMode = m_pPlayStage->GetPlayMode();
						if( pMode )
						{
							CreateRouletteEntity( pMode, RT_BUTTON, BT_ON );
						}
					}
				}
			}
			else
			{
				ioRoulette *pRoulette = GetRouletteEntity( RT_BUTTON, BT_ON );
				if( pRoulette )
				{
					EraseRouletteEntity( RT_BUTTON, BT_ON );
					if( m_pPlayStage )
					{
						ioPlayMode *pMode = m_pPlayStage->GetPlayMode();
						if( pMode )
						{
							CreateRouletteEntity( pMode, RT_BUTTON, BT_OFF );
						}
					}
				}
			}
		}
		break;
	case RS_SPIN:	// 회전중
		ProcessWheel();
		ProcessArrow();
		break;
	case RS_END:	// 완전종료
		ProcessArrow();
		if( m_dwEnableSpinStartTime > 0 && m_dwEnableSpinStartTime < FRAMEGETTIME() )
		{
			SetRouletteReady();
		}
		break;
	}

	if( EnableRouletteSpin( g_MyInfo.GetPublicID() ) && CheckCharRange( g_MyInfo.GetPublicID() ) )
	{
		RouletteCenterHelpWnd *pWnd = dynamic_cast<RouletteCenterHelpWnd*>( g_GUIMgr.FindWnd(ROULETTE_CENTER_HELP_WND) );
		if( pWnd )
			pWnd->SetInfo();
	}
}

void ioRouletteManager::ProcessWheel()
{
	if( !m_pWheel )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();
	if( fTimePerSec <= 0.0f )
		return;

	if( m_dwSpinStartTime + m_dwSpinStopTime > dwCurTime )
	{
		float fRate = (fTimePerSec*1000) / m_fCurSpinSpeed;
		m_fCurAngle += 360.0f * fRate;
		float fAngle = ioMath::ArrangeHead( m_fCurAngle );
		D3DXQUATERNION qtRot, qtResult;
		D3DXQuaternionRotationAxis( &qtRot, &ioMath::UNIT_Z, DEGtoRAD(fAngle) );
		qtResult = m_qtOriginal * qtRot;
		m_pWheel->SetWorldOrientation( qtResult );
	}
	else
	{
		float fRate = (fTimePerSec*1000) / m_fCurSpinSpeed;
		
		m_fCurAngle += max( 0.35f, ( 360.0f * fRate ) );
		float fAngle = ioMath::ArrangeHead( m_fCurAngle );
		m_fCurSpinSpeed += ( m_fCurSpinSpeed * m_fReduceRate ) * fTimePerSec;

		D3DXQUATERNION qtRot, qtResult;
		D3DXQuaternionRotationAxis( &qtRot, &ioMath::UNIT_Z, DEGtoRAD(fAngle) );
		qtResult = m_qtOriginal * qtRot;
		m_pWheel->SetWorldOrientation( qtResult );

		if( m_dwSpinEndTime > 0 && m_dwSpinEndTime < FRAMEGETTIME() && COMPARE( fAngle, m_fResultMinAngle, m_fResultMaxAngle ) )
		{
			float fGap = fAngle - m_fResultAngle;
			if( COMPARE( fGap, -5.0f, 5.0f ) )
			{
				SetRouletteEnd();
			}
		}
	}

	if( m_RouletteState == RS_SPIN && m_bEnableJoin && m_dwSpinStartTime + m_dwJoinEndTime < dwCurTime )
	{
		if( !m_vJoinUser.empty() && m_pPlayStage )
		{
			ioBaseChar *pChar = m_pPlayStage->GetBaseChar( m_vJoinUser[0] );
			if( pChar && pChar->IsOwnerChar() )
			{
				SP2Packet kPacket( CTPK_ROULETTE_JOIN_END );
				TCPNetwork::SendToServer( kPacket );

				m_bEnableJoin = false;
			}
		}
		else if( m_vJoinUser.empty() )
		{
			m_RouletteState = RS_END;
			m_dwEnableSpinStartTime = FRAMEGETTIME() + m_dwRestartTime;
			m_bEnableJoin = false;
		}

		RouletteWnd *pWnd = dynamic_cast<RouletteWnd*>( g_GUIMgr.FindWnd(ROULETTE_WND) );
		if( pWnd )
			pWnd->SetState( RouletteWnd::US_JOIN_END );
	}
}

void ioRouletteManager::ProcessArrow()
{
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();
	if( fTimePerSec <= 0.0f )
		return;

	if( !m_pWheel )
		return;

	switch( m_ArrowState )
	{
	case AS_NONE:
		{
			if( m_RouletteState == RS_END )
				return;

			CheckArrowStumble();
		}
		break;
	case AS_STUMBLE:
		{
			float fRate = (fTimePerSec*1000) / m_fCurSpinSpeed;
			m_fCurArrowAngle += max( 0.1f, ( 360.0f * fRate ) );
			float fAngle = ioMath::ArrangeHead( m_fCurArrowAngle );
			fAngle *= 1.2f;
			D3DXQUATERNION qtRot, qtResult;
			D3DXVECTOR3 vAxis = -ioMath::UNIT_Z;
			D3DXQuaternionRotationAxis( &qtRot, &vAxis, DEGtoRAD(fAngle) );
			qtResult = m_qtOriginal * qtRot;
			m_pArrow->SetWorldOrientation( qtResult );

			if( m_fCurArrowAngle >= 22.5f )
			{
				m_ArrowState = AS_RETURN;
				return;
			}

			if( m_RouletteState == RS_END )
				m_ArrowState = AS_RETURN;
		}
		break;
	case AS_RETURN:
		{
			float fRate = (fTimePerSec*1000) / 500;
			m_fCurArrowAngle -= 360.0f * fRate;
			float fAngle = ioMath::ArrangeHead( m_fCurArrowAngle );

			if( m_fCurArrowAngle <= 0.0f )
			{
				m_pArrow->SetWorldOrientation( m_qtOriginal );
				m_ArrowState = AS_NONE;
				m_fCurArrowAngle = 0.0f;

				if( m_pArrow )
				{
					ioEntityGroup *pGrp = m_pArrow->GetGroup();
					g_SoundMgr.PlaySound( "interface\\public_alert_00.wav", pGrp->GetParentSceneNode() );
				}
			}
			else
			{
				D3DXQUATERNION qtRot, qtResult;
				D3DXVECTOR3 vAxis = -ioMath::UNIT_Z;
				D3DXQuaternionRotationAxis( &qtRot, &vAxis, DEGtoRAD(fAngle) );
				qtResult = m_qtOriginal * qtRot;
				m_pArrow->SetWorldOrientation( qtResult );
			}
		}
		break;
	}
}

bool ioRouletteManager::CheckArrowStumble()
{
	int iArray = GetCurResultGroupArray();
	int iSize = m_vResultGroupList.size();
	if( iSize > iArray )
	{
		int iResult = m_vResultGroupList[iArray].m_vRouletteResultList.size();
		for( int i=0; i<iResult; ++i )
		{
			ResultData kData = m_vResultGroupList[iArray].m_vRouletteResultList[i];

			D3DXVECTOR3 vArrowDir = ioMath::UNIT_Y;
			D3DXVECTOR3 vDir = kData.m_vDirection;
			float fDotValue = D3DXVec3Dot( &vDir, &vArrowDir );
			float fAngle = RADtoDEG( acosf(fDotValue) );

			if( fAngle < FLOAT10 && vDir.x >= 0.0f )
			{
				m_ArrowState = AS_STUMBLE;
				return true;
			}
		}
	}
	return false;
}

void ioRouletteManager::SetRouletteReady()
{
	if( !m_pPlayStage )
		return;

	if( m_RouletteState == RS_READY )
		return;

	m_RouletteState = RS_READY;

	InitForRestart();

	RouletteWnd *pWnd = dynamic_cast<RouletteWnd*>( g_GUIMgr.FindWnd(ROULETTE_WND) );
	if( pWnd )
		pWnd->SetState( RouletteWnd::US_READY );

	EraseRouletteEntity( RT_BUTTON, BT_OFF );
	EraseRouletteEntity( RT_BUTTON, BT_ON );
	ioPlayMode *pMode = m_pPlayStage->GetPlayMode();
	if( pMode )
	{
		if( g_MyInfo.GetEtcCoinMoney( ioEtcItem::EIT_ETC_ROULETTE_COIN ) > 0 )
			CreateRouletteEntity( pMode, RT_BUTTON, BT_ON );
		else
			CreateRouletteEntity( pMode, RT_BUTTON, BT_OFF );
	}

	if( m_dwEffectDelayID == -1 )
	{
		ioMapEffect *pEffect = m_pPlayStage->CreateMapEffect( m_EffectDelay, m_vRoulettePosition, m_vRouletteScale );
		if( pEffect )
		{
			m_dwEffectDelayID = pEffect->GetUniqueID();
		}
	}
	if( m_dwEffectResultID != -1 )
	{
		m_pPlayStage->DestroyMapEffect( m_dwEffectResultID );
		m_dwEffectResultID = -1;
	}
}

void ioRouletteManager::SetRouletteSpin( const ioHashString & szUserName, bool bSend )
{
	if( !m_pPlayStage )
		return;

	ioBaseChar *pChar = m_pPlayStage->GetBaseChar( szUserName );
	if( pChar && pChar->IsNeedProcess() && pChar->IsNeedSendNetwork() && bSend )
	{
		SP2Packet kPacket( CTPK_ROULETTE_START );
		TCPNetwork::SendToServer( kPacket );

		EraseRouletteEntity( RT_BUTTON, BT_ON );
		CreateRouletteEntity( m_pPlayStage->GetPlayMode(), RT_BUTTON, BT_OFF );

		m_pPlayStage->CreateMapEffect( m_EffectButton, m_vRoulettePosition, m_vRouletteScale );
	}

	if( m_RouletteState != RS_SPIN )
	{
		InitForRestart();
		ResetEntity();
		m_fCurAngle = 0.0f;
		m_RouletteState = RS_SPIN;
		m_fCurSpinSpeed = m_fSpinSpeed;
		m_dwSpinStartTime = m_dwSpinEndTime = FRAMEGETTIME();
		m_dwSpinEndTime += m_dwSpinDuration;
	}

	if( m_dwEffectDelayID != -1 )
	{
		m_pPlayStage->DestroyMapEffect( m_dwEffectDelayID );
		m_dwEffectDelayID = -1;
	}
	if( m_dwEffectStartID == -1 )
	{
		ioMapEffect *pEffect = m_pPlayStage->CreateMapEffect( m_EffectStart, m_vRoulettePosition, m_vRouletteScale );
		if( pEffect )
		{
			m_dwEffectStartID = pEffect->GetUniqueID();
		}
	}

	ioHashStringVec::iterator iter = std::find( m_vJoinUser.begin(), m_vJoinUser.end(), szUserName );
	if( iter == m_vJoinUser.end() )
		m_vJoinUser.push_back( szUserName );

	RouletteWnd *pWnd = dynamic_cast<RouletteWnd*>( g_GUIMgr.FindWnd(ROULETTE_WND) );
	if( pWnd )
		pWnd->SetState( RouletteWnd::US_SPIN );
}

void ioRouletteManager::SetRouletteEnd()
{
	if( m_RouletteState == RS_END )
		return;

	m_RouletteState = RS_END;

	if( !m_vJoinUser.empty() && m_pPlayStage )
	{
		ioBaseChar *pChar = m_pPlayStage->GetBaseChar( m_vJoinUser[0] );
		if( pChar && pChar->IsNeedProcess() && pChar->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CTPK_ROULETTE_END );
			TCPNetwork::SendToServer( kPacket );
		}
	}
	else if( m_vJoinUser.empty() )
	{
		m_dwEnableSpinStartTime = FRAMEGETTIME() + m_dwRestartTime;
	}

	RouletteWnd *pWnd = dynamic_cast<RouletteWnd*>( g_GUIMgr.FindWnd(ROULETTE_WND) );
	if( pWnd )
		pWnd->SetState( RouletteWnd::US_END );

	if( m_dwEffectStartID != -1 )
	{
		m_pPlayStage->DestroyMapEffect( m_dwEffectStartID );
		m_dwEffectStartID = -1;
	}
	if( m_dwEffectResultID == -1 )
	{
		ioMapEffect *pEffect = m_pPlayStage->CreateMapEffect( m_EffectResult, m_vRoulettePosition, m_vRouletteScale );
		if( pEffect )
			m_dwEffectResultID = pEffect->GetUniqueID();
	}

	if( m_pWheel )
	{
		ioEntityGroup *pGrp = m_pWheel->GetGroup();
		g_SoundMgr.PlaySound( "interface\\compound_ok_02.wav", pGrp->GetParentSceneNode() );
	}
}

bool ioRouletteManager::EnableRouletteSpin( const ioHashString & szUserName )
{
	if( !IsRouletteEvent() )
		return false;

	if( !m_pPlayStage )
		return false;

	if( m_pPlayStage->GetModeType() != MT_TRAINING )
		return false;

	if( m_RouletteState == RS_END )
		return false;
	
	if( IsJoinRoulette( szUserName ) )
		return false;

	if( g_MyInfo.GetEtcCoinMoney( ioEtcItem::EIT_ETC_ROULETTE_COIN ) < m_iNeedCoin )
		return false;

	if( !m_bEnableJoin )
		return false;

	return true;
}

bool ioRouletteManager::CheckCharRange( const ioHashString & szUserName )
{
	if( !m_pPlayStage )
		return false;
	
	ioBaseChar *pChar = m_pPlayStage->GetBaseChar( szUserName );
	if( !pChar )
		return false;

	D3DXVECTOR3 vRoulettePos = GetButtonPositon();
	D3DXVECTOR3 vCharPos = pChar->GetWorldPosition();
	D3DXVECTOR3 vDiff = vRoulettePos - vCharPos;
	float fRange = D3DXVec3Length( &vDiff );
	if( fRange < m_fEnableRange )
	{
		vRoulettePos.y = 0.0f;
		vCharPos.y = 0.0f;
		D3DXVECTOR3 vRouletteDir = -ioMath::UNIT_Z;
		D3DXVECTOR3 vDir = vCharPos - vRoulettePos;
		D3DXVec3Normalize( &vDir, &vDir );
		float fDot = D3DXVec3Dot( &vDir, &vRouletteDir );
		float fAngle = RADtoDEG( acosf(fDot) );
		if( fAngle <= m_fEnableAngle/2 )
		{
			return true;
		}
	}

	return false;
}

void ioRouletteManager::OnRouletteStart( SP2Packet &rkPacket )
{
	int iType;
	rkPacket >> iType;

	switch( iType )
	{
	case ROULETTE_START_SUCCESS:
		{
			ioHashString szUserName;
			rkPacket >> szUserName;
			
			m_vJoinUser.clear();
			SetRouletteSpin( szUserName, false );

			m_fCurAngle = 0.0f;
			m_RouletteState = RS_SPIN;
			m_fCurSpinSpeed = m_fSpinSpeed;
			m_dwSpinStartTime = m_dwSpinEndTime = FRAMEGETTIME();
			m_dwSpinEndTime += m_dwSpinDuration;
		}
		break;
	case ROULETTE_START_NOT_ENOUGH_COIN:
		{
			RemoveJoinUser( g_MyInfo.GetPublicID() );
			m_bEnableJoin = false;
		}
		break;
	case ROULETTE_JOIN_SUCCESS:
		{
			m_vJoinUser.clear();
			int iSize = 0;
			rkPacket >> iSize;
			for( int i=0; i<iSize; ++i )
			{
				ioHashString szUserName;
				rkPacket >> szUserName;
				m_vJoinUser.push_back( szUserName );
			}

			int iCurArray = GetCurResultGroupArray();
			ioRoulette *pWheel = GetRouletteEntity( RT_WHEEL, iCurArray-1 );
			ioRoulette *pPrize = GetRouletteEntity( RT_PRIZE, iCurArray-1 );
			if( !pWheel || !pPrize )
				return;
			EraseRouletteEntity( RT_WHEEL, iCurArray-1 );
			EraseRouletteEntity( RT_PRIZE, iCurArray-1 );

			if( m_pPlayStage )
			{
				ioPlayMode *pMode = m_pPlayStage->GetPlayMode();
				if( pMode )
				{
					ioRoulette *pWheel = CreateRouletteEntity( pMode, RT_WHEEL, iCurArray );
					if( pWheel )
						m_pWheel = pWheel;

					CreateRouletteEntity( pMode, RT_PRIZE, iCurArray );
				}
			}
		}
		break;
	case ROULETTE_JOIN_TIME_OVER:
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, "아직 참여할 수 없습니다." );
			RemoveJoinUser( g_MyInfo.GetPublicID() );
			m_bEnableJoin = false;
			m_RouletteState = RS_END;
			m_dwEnableSpinStartTime = FRAMEGETTIME() + m_dwRestartTime;
			RouletteWnd *pWnd = dynamic_cast<RouletteWnd*>( g_GUIMgr.FindWnd(ROULETTE_WND) );
			if( pWnd )
				pWnd->SetState( RouletteWnd::US_END );
		}
		break;
	case ROULETTE_JOIN_ALREADY:
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, "이미 참여했습니다." );
		}
		break;
	}
}

void ioRouletteManager::OnRouletteJoinEnd( SP2Packet &rkPacket )
{
	int iResultAngle;
	rkPacket >> iResultAngle;
	m_fResultAngle = (float)iResultAngle;
	m_bEnableJoin = false;

	int iSize = m_vResultGroupList.size();
	for( int i=0; i<iSize; ++i )
	{
		int iUserCount = m_vJoinUser.size();
		ResultGroup kInfo = m_vResultGroupList[i];
		if( COMPARE( iUserCount, kInfo.m_iMinPlayers, kInfo.m_iMaxPlayers+1 ) )
		{
			int iResultSize = kInfo.m_vRouletteResultList.size();
			for( int j=0; j<iResultSize; ++j )
			{
				if( COMPARE( m_fResultAngle, kInfo.m_vRouletteResultList[j].m_fAngleMin, kInfo.m_vRouletteResultList[j].m_fAngleMax ) )
				{
					m_fResultMinAngle = kInfo.m_vRouletteResultList[j].m_fAngleMin;
					m_fResultMaxAngle = kInfo.m_vRouletteResultList[j].m_fAngleMax;
				}
			}
		}
	}

	RouletteWnd *pWnd = dynamic_cast<RouletteWnd*>( g_GUIMgr.FindWnd(ROULETTE_WND) );
	if( pWnd )
		pWnd->SetState( RouletteWnd::US_JOIN_END );
}

void ioRouletteManager::OnRouletteEnd( SP2Packet &rkPacket )
{
	bool bDirectEnd = false;
	rkPacket >> bDirectEnd;

	if( !bDirectEnd )
	{
		int iSize = m_vJoinUser.size();
		for( int i=0; i<iSize; ++i )
		{
			if( !m_pPlayStage )
				continue;

			ioBaseChar *pChar = m_pPlayStage->GetBaseChar( m_vJoinUser[i] );
			if( pChar )
			{
				if( pChar->IsOwnerChar() )
				{
					g_MyInfo.UseEtcCoinMoney( ioEtcItem::EIT_ETC_ROULETTE_COIN, m_iNeedCoin );
				}
			}
		}
	}
	else
	{
		SetRouletteEnd();
	}

	m_dwEnableSpinStartTime = FRAMEGETTIME() + m_dwRestartTime;
}

void ioRouletteManager::OnRouletteException( SP2Packet &rkPacket )
{
	// 종료시간이 되기 전에 종료 패킷을 날린 경우 종료시간을 재조정해준다.
	DWORD dwRemainEndTime;
	rkPacket >> dwRemainEndTime;

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwSpinEndTime = dwCurTime + dwRemainEndTime;
	if( m_dwSpinEndTime >= m_dwSpinDuration )
		m_dwSpinStartTime = m_dwSpinEndTime - m_dwSpinDuration;
	else
		m_dwSpinStartTime = 0;

	if( m_RouletteState == RS_END )
	{
		m_RouletteState = RS_SPIN;
	}
}

void ioRouletteManager::ApplyRouletteInfo( SP2Packet &rkPacket )
{
	if( !IsRouletteEvent() )
		return;

	bool bSpin = false;
	rkPacket >> bSpin;
	if( !bSpin )
	{
		m_RouletteState = RS_READY;
		return;
	}
	else
	{
		m_RouletteState = RS_SPIN;
	}

	// 시간 계산
	DWORD dwRemainEndTime;
	rkPacket >> dwRemainEndTime;
	DWORD dwCurTime = FRAMEGETTIME();
	m_dwSpinEndTime = dwCurTime + dwRemainEndTime;
	if( m_dwSpinEndTime >= m_dwSpinDuration )
		m_dwSpinStartTime = m_dwSpinEndTime - m_dwSpinDuration;
	else
		m_dwSpinStartTime = 0;

	m_fCurSpinSpeed = m_fSpinSpeed;

	// 참여 인원
	int iPlayerCount;
	rkPacket >> iPlayerCount;
	m_vJoinUser.clear();
	for( int i=0; i<iPlayerCount; ++i )
	{
		ioHashString szUserName;
		rkPacket >> szUserName;
		m_vJoinUser.push_back( szUserName );
	}

	// 참여 인원을 받았으면 룰렛 엔티티를 변경해준다.
	int iCurArray = GetCurResultGroupArray();
	ResetEntity( iCurArray );

	// 참여 가능 여부 체크
	bool bResult = false;
	rkPacket >> bResult;
	if( !bResult )
	{
		m_bEnableJoin = true;

		RouletteWnd *pWnd = dynamic_cast<RouletteWnd*>( g_GUIMgr.FindWnd(ROULETTE_WND) );
		if( pWnd )
			pWnd->SetState( RouletteWnd::US_SPIN );
		return;
	}
	else
	{
		m_bEnableJoin = false;
		m_dwEnableSpinStartTime = FRAMEGETTIME() + m_dwRestartTime;

		RouletteWnd *pWnd = dynamic_cast<RouletteWnd*>( g_GUIMgr.FindWnd(ROULETTE_WND) );
		if( pWnd )
		{
			if( m_dwSpinEndTime < FRAMEGETTIME() )
			{
				pWnd->SetState( RouletteWnd::US_END );
				SetRouletteEnd();
			}
			else if( m_dwSpinStartTime + m_dwJoinEndTime < dwCurTime )
			{
				pWnd->SetState( RouletteWnd::US_JOIN_END );
			}
			else
			{
				pWnd->SetState( RouletteWnd::US_END );
				SetRouletteEnd();
			}
		}
	}

	int iResultAngle = 0;
	rkPacket >> iResultAngle;
	if( !COMPARE( iResultAngle, 0, 360 ) )
		iResultAngle = 359;

	m_fResultAngle = static_cast<int>(iResultAngle);

	int iSize = m_vResultGroupList.size();
	for( int i=0; i<iSize; ++i )
	{
		int iUserCount = m_vJoinUser.size();
		ResultGroup kInfo = m_vResultGroupList[i];
		if( COMPARE( iUserCount, kInfo.m_iMinPlayers, kInfo.m_iMaxPlayers+1 ) )
		{
			int iResultSize = kInfo.m_vRouletteResultList.size();
			for( int j=0; j<iResultSize; ++j )
			{
				if( COMPARE( m_fResultAngle, kInfo.m_vRouletteResultList[j].m_fAngleMin, kInfo.m_vRouletteResultList[j].m_fAngleMax ) )
				{
					m_fResultMinAngle = kInfo.m_vRouletteResultList[j].m_fAngleMin;
					m_fResultMaxAngle = kInfo.m_vRouletteResultList[j].m_fAngleMax;
				}
			}
		}
	}
}

void ioRouletteManager::RemoveJoinUser( const ioHashString &rkName )
{
	ioHashStringVec::iterator iter = m_vJoinUser.begin();
	for(;iter != m_vJoinUser.end();iter++)
	{
		if( *iter == rkName )
		{
			m_vJoinUser.erase( iter );
			return;
		}
	}
}

void ioRouletteManager::RemoveJoinPlayer( const ioHashString &szUserName )
{
	if( m_vJoinUser.empty() )
		return;

	ioHashStringVec::iterator iter = m_vJoinUser.begin();
	while( iter != m_vJoinUser.end() )
	{
		if( szUserName == *iter )
		{
			iter = m_vJoinUser.erase( iter );

			int iCurArray = GetCurResultGroupArray();
			ioRoulette *pWheel = GetRouletteEntity( RT_WHEEL, iCurArray+1 );
			ioRoulette *pPrize = GetRouletteEntity( RT_PRIZE, iCurArray+1 );
			if( !pWheel || !pPrize )
				return;
			EraseRouletteEntity( RT_WHEEL, iCurArray+1 );
			EraseRouletteEntity( RT_PRIZE, iCurArray+1 );

			if( m_pPlayStage )
			{
				ioPlayMode *pMode = m_pPlayStage->GetPlayMode();
				if( pMode )
				{
					ioRoulette *pWheel = CreateRouletteEntity( pMode, RT_WHEEL, iCurArray );
					if( pWheel )
						m_pWheel = pWheel;

					CreateRouletteEntity( pMode, RT_PRIZE, iCurArray );
				}
			}
		}
		else
			++iter;
	}
}

bool ioRouletteManager::IsRouletteEvent()
{
 	if( m_RouletteState != RS_READY )
 		return true;

	return g_EventMgr.IsAlive( EVT_ROULETTE, g_MyInfo.GetChannelingType() );
}

bool ioRouletteManager::IsJoinRoulette( const ioHashString &szUserName )
{
	int iSize = m_vJoinUser.size();
	for( int i=0; i<iSize; ++i )
	{
		if( m_vJoinUser[i] == szUserName )
			return true;
	}
	return false;
}

D3DXVECTOR3 ioRouletteManager::GetRoulettePos()
{
	return m_vRoulettePosition;
}

D3DXVECTOR3 ioRouletteManager::GetButtonPositon()
{
	ioRoulette *pRoulette = GetRouletteEntity( RT_BUTTON );
	if( pRoulette )
		return pRoulette->GetWorldPosition();

	return ioMath::VEC3_ZERO;
}

void ioRouletteManager::CheckRouletteStruct()
{
	if( !m_pPlayStage )
		return;

	ioPlayMode *pMode = m_pPlayStage->GetPlayMode();
	if( !pMode )
		return;

	if( pMode->GetModeType() != MT_TRAINING )
		return;

	if( IsRouletteEvent() )
	{
		CreateRouletteAllEntity( pMode );

		RouletteList::iterator iter = m_vRouletteList.begin();
		for( ; iter != m_vRouletteList.end(); ++iter )
		{
			ioRoulette *pRoulette = *iter;
			if( pRoulette )
			{
				m_pPlayStage->AddGameEntity( pRoulette );

				if( pRoulette->GetRouletteType() == RT_WHEEL )
					m_pWheel = pRoulette;
				else if( pRoulette->GetRouletteType() == RT_ARROW )
					m_pArrow = pRoulette;
			}
		}

		if( !g_GUIMgr.IsShow( ROULETTE_WND ) )
			g_GUIMgr.ShowWnd( ROULETTE_WND );
	}

	if( m_dwEffectDelayID == -1 )
	{
		ioMapEffect *pEffect = m_pPlayStage->CreateMapEffect( m_EffectDelay, m_vRoulettePosition, m_vRouletteScale );
		if( pEffect )
		{
			m_dwEffectDelayID = pEffect->GetUniqueID();
		}
	}
}

void ioRouletteManager::CheckRouletteStruct( bool bAlive )
{
	if( !m_pPlayStage )
		return;

	ioPlayMode *pMode = m_pPlayStage->GetPlayMode();
	if( !pMode )
		return;

	if( pMode->GetModeType() != MT_TRAINING )
		return;

	bool bExistStruct = IsExistRouletteEntity();

	char szTitle[MAX_PATH] = "", szBuf[MAX_PATH] = "";

	if( bAlive && !bExistStruct )
	{
		CreateRouletteAllEntity( pMode );

		RouletteList::iterator iter = m_vRouletteList.begin();
		for( ; iter != m_vRouletteList.end(); ++iter )
		{
			ioRoulette *pRoulette = *iter;
			if( pRoulette )
			{
				m_pPlayStage->AddGameEntity( pRoulette );

				if( pRoulette->GetRouletteType() == RT_WHEEL )
					m_pWheel = pRoulette;
				else if( pRoulette->GetRouletteType() == RT_ARROW )
					m_pArrow = pRoulette;
			}
		}

		if( !g_GUIMgr.IsShow( ROULETTE_WND ) )
			g_GUIMgr.ShowWnd( ROULETTE_WND );

		if( !m_EventStartEffect.IsEmpty() )
			m_pPlayStage->CreateMapEffect( m_EventStartEffect, m_vRoulettePosition, m_vRouletteScale );

		if( m_dwEffectDelayID == -1 )
		{
			ioMapEffect *pEffect = m_pPlayStage->CreateMapEffect( m_EffectDelay, m_vRoulettePosition, m_vRouletteScale );
			if( pEffect )
			{
				m_dwEffectDelayID = pEffect->GetUniqueID();
			}
		}

		SetRouletteReady();
	}
	else if( !bAlive && bExistStruct )
	{
		m_pWheel = NULL;
		m_pArrow = NULL;
		
		RouletteList::iterator iter = m_vRouletteList.begin();
		for( ; iter != m_vRouletteList.end(); ++iter )
		{
			ioRoulette *pRoulette = *iter;
			if( pRoulette )
				pRoulette->SetReserveDestroy();
		}
		m_vRouletteList.clear();

		if( g_GUIMgr.IsShow( ROULETTE_WND ) )
			g_GUIMgr.HideWnd( ROULETTE_WND );

		if( !m_EventEndEffect.IsEmpty() )
			m_pPlayStage->CreateMapEffect( m_EventEndEffect, m_vRoulettePosition, m_vRouletteScale );

		if( m_dwEffectDelayID != -1 )
		{
			m_pPlayStage->DestroyMapEffect( m_dwEffectDelayID );
			m_dwEffectDelayID = -1;
		}

		if( m_dwEffectStartID != -1 )
		{
			m_pPlayStage->DestroyMapEffect( m_dwEffectStartID );
			m_dwEffectStartID = -1;
		}

		if( m_dwEffectResultID != -1 )
		{
			m_pPlayStage->DestroyMapEffect( m_dwEffectResultID );
			m_dwEffectResultID = -1;
		}

		if( g_GUIMgr.IsShow( ROULETTE_WND ) )
			g_GUIMgr.HideWnd( ROULETTE_WND );
	}
}

void ioRouletteManager::ResetEntity( int iSubIndex /* = 0 */ )
{
	int iSize = m_vRouletteInfoList.size();
	for( int i=0; i<iSize; ++i )
	{
		EraseRouletteEntity( RT_WHEEL, i );
		EraseRouletteEntity( RT_PRIZE, i );
	}

	ioPlayMode *pMode = m_pPlayStage->GetPlayMode();
	if( pMode )
	{
		ioRoulette *pWheel = CreateRouletteEntity( pMode, RT_WHEEL, iSubIndex );
		if( pWheel )
		{
			m_pWheel = pWheel;
		}

		CreateRouletteEntity( pMode, RT_PRIZE, iSubIndex );
	}
}

void ioRouletteManager::EraseRouletteEntity( int iType, int iSubIndex )
{
	RouletteList::iterator iter = m_vRouletteList.begin();
	while( iter != m_vRouletteList.end() )
	{
		ioRoulette *pRoulette = *iter;
		if( pRoulette && pRoulette->GetRouletteType() == iType && pRoulette->GetSubIndex() == iSubIndex )
		{
			pRoulette->SetReserveDestroy();
			m_vRouletteList.erase( iter );
		}
		else
			++iter;
	}
}

int ioRouletteManager::GetCurResultGroupArray()
{
	int iPlayers = m_vJoinUser.size();
	int iSize = m_vResultGroupList.size();
	for( int i=0; i<iSize; ++i )
	{
		if( COMPARE( iPlayers, m_vResultGroupList[i].m_iMinPlayers, m_vResultGroupList[i].m_iMaxPlayers + 1 ) )
			return i;
	}
	return 0;
}

void ioRouletteManager::GetResultGroupInfoByArray( IN int iArray, OUT int &iMax, OUT int &iMin, OUT int &iRate )
{
	int iSize = m_vResultGroupList.size();
	if( iSize <= iArray )
		return;

	iMax  = m_vResultGroupList[iArray].m_iMaxPlayers;
	iMin  = m_vResultGroupList[iArray].m_iMinPlayers;

	if( m_vResultGroupList[iArray].m_vRouletteResultList.empty() )
		return;

	iRate = m_vResultGroupList[iArray].m_vRouletteResultList[0].m_iRate;
}

ioRoulette *ioRouletteManager::GetRouletteEntity( int iType, int iSubIndex /* = 0 */ )
{
	RouletteList::iterator iter = m_vRouletteList.begin();
	for( ; iter != m_vRouletteList.end(); ++iter )
	{
		ioRoulette *pRoulette = *iter;
		if( pRoulette )
		{
			if( pRoulette->GetRouletteType() == iType && pRoulette->GetSubIndex() == iSubIndex &&
				!pRoulette->IsReserveDestroy() )
				return pRoulette;
		}
	}

	return NULL;
}

bool ioRouletteManager::IsExistRouletteEntity()
{
	if( !m_pPlayStage )
		return false;

	const GameEntityList &rkObjectList = m_pPlayStage->GetGameEntityList();
	GameEntityList::const_iterator iter_b;
	for( iter_b = rkObjectList.begin() ; iter_b!=rkObjectList.end() ; ++iter_b )
	{
		ioRoulette *pRoulette = ToRoulette( *iter_b );
		if( pRoulette )
			return true;
	}

	return false;
}

void ioRouletteManager::DestroyRouletteList()
{
	if( !m_pPlayStage )
		return;

	m_vRouletteList.clear();
	m_pPlayStage->DestroyAllRouletteEntity();
}

int ioRouletteManager::GetRewardRank()
{
	if( m_RouletteState != RS_END )
		return 0;

	int iArray = GetCurResultGroupArray();
	int iSize = m_vResultGroupList[iArray].m_vRouletteResultList.size();
	for( int i=0; i<iSize; ++i )
	{
		ResultData kData = m_vResultGroupList[iArray].m_vRouletteResultList[i];
		if( COMPARE( m_fResultAngle, kData.m_fAngleMin, kData.m_fAngleMax ) )
			return i+1;
	}

	return 0;
}

const D3DXQUATERNION &ioRouletteManager::GetRouletteOrientation()
{
	if( !m_pWheel )
		return m_qtOriginal;

	return m_pWheel->GetWorldOrientation();
}