

#include "stdafx.h"

#include "ioBallManager.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

template<> ioBallManager* Singleton< ioBallManager >::ms_Singleton = 0;

ioBallManager::ioBallManager( ioPlayStage *pStage )
{
	m_iLocalIndex = 0;
	m_pPlayStage = pStage;
	m_vBallStructList.clear();
}

ioBallManager::~ioBallManager()
{
	DestroyBallStructList();
}

ioBallManager& ioBallManager::GetSingleton()
{
	return Singleton< ioBallManager >::GetSingleton();
}

ioBall* ioBallManager::CreateBallStruct( ioHashString szStructName, ioPlayMode *pMode )
{
	ioEntityGroup *pGrp = m_pPlayStage->CreateEntityGrpAndSceneNode( szStructName, true );
	if( !pGrp )
	{
		LOG.PrintTimeAndLog( 0, "ioBallManager::CreatePushStruct - %s Fail!", szStructName.c_str() );
		return NULL;
	}

	ioBall* pPush = new ioBall( pGrp, pMode );
	m_iLocalIndex++;
	m_vBallStructList.push_back( pPush );
	return pPush;
}

void ioBallManager::GetBallStructInfo( SP2Packet &rkPacket )
{
	int iBallCnt = m_vBallStructList.size();
	rkPacket << iBallCnt;
	for( int i=0 ; i<iBallCnt ; i++ )
	{
		m_vBallStructList[i]->GetBallStructInfo( rkPacket );
	}
}

void ioBallManager::ApplyBallStructInfo( SP2Packet &rkPacket )
{
	int iBallCnt;
	rkPacket >> iBallCnt;

	for( int i=0 ; i<iBallCnt ; i++ )
	{
		int iArray;
		rkPacket >> iArray;

		ioBall *pBall = FindBallStruct( iArray );
		if( pBall )
		{
			pBall->OnStructInfo( rkPacket );
		}
		else
		{
			// 예외처리
			int iType;
			rkPacket >> iType;

			int iState;
			rkPacket >> iState;

			switch( iType )
			{
			case BALL_KICK:
				{
					ioHashString szAttacker;
					rkPacket >> szAttacker;

					D3DXVECTOR3 vPos;
					rkPacket >> vPos;

					D3DXVECTOR3 vDir;
					float fPower, fSpeed;
					rkPacket >> vDir >> fPower >> fSpeed;

					float fDamage;
					rkPacket >> fDamage;
					int iActionStop, iShakeCamera;
					rkPacket >> iActionStop, iShakeCamera;
				}
				break;
			case BALL_INFO:
				D3DXVECTOR3 vPos;
				rkPacket >> vPos;

				D3DXVECTOR3 vDir;
				float fPower, fSpeed;
				rkPacket >> vDir >> fPower >> fSpeed;
				break;
			}

		}
	}
}

ioBall* ioBallManager::FindBallStruct( int iArray )
{
	int iBallCnt = m_vBallStructList.size();
	for( int i=0; i<iBallCnt; i++ )
	{
		if( m_vBallStructList[i]->GetArrayIndex() == iArray )
			return m_vBallStructList[i];
	}

	return NULL;
}

ioBall* ioBallManager::GetBallStruct( int iIndex )
{
	if( (int)m_vBallStructList.size() > iIndex )
		return m_vBallStructList[iIndex];

	return NULL;
}

void ioBallManager::DestroyBallStructList()
{
	m_vBallStructList.clear();
	m_pPlayStage->DestroyAllBallStruct();
}

void ioBallManager::OnBallStructReposition( SP2Packet &rkPacket )
{
	int iArray;
	rkPacket >> iArray;

	ioBall *pBall = FindBallStruct( iArray );
	if( !pBall ) return;

	pBall->OnStructReposition( rkPacket );
}