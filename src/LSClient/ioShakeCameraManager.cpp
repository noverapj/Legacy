

#include "stdafx.h"

#include "ioPlayStage.h"
#include "ioShakeCameraManager.h"

template<> ioShakeCameraManager* Singleton< ioShakeCameraManager >::ms_Singleton = 0;

ioShakeCameraManager::ioShakeCameraManager( ioPlayStage *pStage )
{
	m_pPlayStage = pStage;
}

ioShakeCameraManager::~ioShakeCameraManager()
{
}

void ioShakeCameraManager::InitManager()
{
	m_ShakeCameraList.clear();

	ioINILoader_e kLoader( "config/sp2_shake_camera.ini" );

	int iNumItem = kLoader.LoadInt_e( "common_info", "shake_camera_count", 0 );
	m_ShakeCameraList.reserve( iNumItem );

	char szBuf[MAX_PATH];
	for( int i=0 ; i<iNumItem ; i++ )
	{
		wsprintf_e( szBuf, "shake%d", i+1 );
		kLoader.SetTitle( szBuf );

		ShakeCamera eShake;
		eShake.iShakeID = i+1;
		eShake.dwShakeChangeTime = kLoader.LoadInt_e( "direction_change_time", 0 );
		eShake.fShakeRate = kLoader.LoadFloat_e( "shake_rate", 0.0f );
		eShake.fShakeAmountX = kLoader.LoadFloat_e( "shake_amount_x", 0.0f );
		eShake.fShakeAmountY = kLoader.LoadFloat_e( "shake_amount_y", 0.0f );

		eShake.fEndAmount = kLoader.LoadFloat_e( "shake_end_amount", FLOAT1 );
		eShake.fEndAmount = max( FLOAT1, eShake.fEndAmount );

		eShake.dwRollChangeTime = kLoader.LoadInt_e( "roll_change_time", 0 );
		eShake.dwRollEndSlerp = kLoader.LoadInt_e( "roll_end_slerp", 0 );
		eShake.fRollAmount = kLoader.LoadFloat_e( "roll_amount", 0.0f );
		eShake.fRollEndAmount = kLoader.LoadFloat_e( "roll_end_amount", 0.0f );
		eShake.fRollRate = kLoader.LoadFloat_e( "roll_rate", 0.0f );

		m_ShakeCameraList.push_back( eShake );
	}
}

void ioShakeCameraManager::SetShakeCamera( int iShakeType, int iPriority )
{
	if( iShakeType <= 0)
		return;

	ShakeCameraList::iterator iter = m_ShakeCameraList.begin();
	for( ; iter!= m_ShakeCameraList.end() ; ++iter )
	{
		const ShakeCamera &eShake = *iter;
		if( eShake.iShakeID == iShakeType )
		{
			if( m_pPlayStage )
			{
				m_pPlayStage->ShakeCameraStart( eShake, iPriority );
				m_pPlayStage->RollCameraStart( eShake );
			}
			break;
		}
	}
}

ioShakeCameraManager& ioShakeCameraManager::GetSingleton()
{
	return Singleton< ioShakeCameraManager >::GetSingleton();
}

