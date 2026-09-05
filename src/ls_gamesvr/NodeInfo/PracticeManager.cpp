#include <stdafx.h>

#include "PracticeManager.h"

static PracticeManager *s_pInstance = NULL;

PracticeManager::PracticeManager()
{
	m_pPracticeMgr = NULL;
}

PracticeManager::~PracticeManager()
{
	if( m_pPracticeMgr )
	{
		m_pPracticeMgr->Release();
		SAFEDELETE( m_pPracticeMgr );
	}
}

PracticeManager& PracticeManager::GetInstance()
{
	if( !s_pInstance )
		s_pInstance = new PracticeManager;
		return *s_pInstance;
}

void PracticeManager::ReleaseInstance()
{
	SAFEDELETE( s_pInstance );
}

void PracticeManager::Init()
{
	if( m_pPracticeMgr )
	{
		m_pPracticeMgr->Release();
		SAFEDELETE( m_pPracticeMgr );
	}

	m_pPracticeMgr = new LSC_Practice_Manager;
	m_pPracticeMgr->LoadData( PRACTICE_TABLE );

}

int PracticeManager::GetRegularSoldierCloak( int iIndex )
{
	if( m_pPracticeMgr )
	{
		LSC_Practice *pData = m_pPracticeMgr->GetData( iIndex );

		if( pData ) 
			return pData->CloakIndex;
	}

	return -1;
}

int PracticeManager::GetRegularSoldierHelmet( int iIndex )
{
	if( m_pPracticeMgr )
	{
		LSC_Practice *pData = m_pPracticeMgr->GetData( iIndex );

		if( pData ) 
			return pData->HelmetIndex;
	}

	return -1;
}

int PracticeManager::GetRegularSoldierArmor( int iIndex )
{
	if( m_pPracticeMgr )
	{
		LSC_Practice *pData = m_pPracticeMgr->GetData( iIndex );

		if( pData ) 
			return pData->ArmorIndex;
	}

	return -1;
}

int PracticeManager::GetRegularSoldierWeapon( int iIndex )
{
	if( m_pPracticeMgr )
	{
		LSC_Practice *pData = m_pPracticeMgr->GetData( iIndex );

		if( pData ) 
			return pData->WeaponIndex;
	}

	return -1;
}

int PracticeManager::GetRegularSoldierClass( int iIndex )
{
	if( m_pPracticeMgr )
	{
		LSC_Practice *pData = m_pPracticeMgr->GetData( iIndex );

		if( pData ) 
			return pData->UseClass;
	}
	return -1;
}

BYTE PracticeManager::GetRegularSoldierGender( int iIndex )
{
	if( m_pPracticeMgr )
	{
		LSC_Practice *pData = m_pPracticeMgr->GetData( iIndex );

		if( pData ) 
			return pData->Gender;
	}
	return 0;
}

int PracticeManager::GetRegularSoldierUnderwear( int iIndex )
{
	if( m_pPracticeMgr )
	{
		LSC_Practice *pData = m_pPracticeMgr->GetData( iIndex );

		if( pData ) 
			return pData->Underwear;
	}
	return -1;
}

int PracticeManager::GetRegularSoldierHair( int iIndex )
{
	if( m_pPracticeMgr )
	{
		LSC_Practice *pData = m_pPracticeMgr->GetData( iIndex );

		if( pData ) 
			return pData->Hair;
	}
	return -1;
}

int PracticeManager::GetRegularSoldierHairColor( int iIndex )
{
	if( m_pPracticeMgr )
	{
		LSC_Practice *pData = m_pPracticeMgr->GetData( iIndex );

		if( pData ) 
			return pData->HairColor;
	}
	return -1;
}

int PracticeManager::GetRegularSoldierFace( int iIndex )
{
	if( m_pPracticeMgr )
	{
		LSC_Practice *pData = m_pPracticeMgr->GetData( iIndex );

		if( pData ) 
			return pData->Face;
	}
	return -1;
}

int PracticeManager::GetRegularSoldierSkinColor( int iIndex )
{
	if( m_pPracticeMgr )
	{
		LSC_Practice *pData = m_pPracticeMgr->GetData( iIndex );

		if( pData ) 
			return pData->SkinColor;
	}
	return -1;
}

int PracticeManager::GetPracticeMap( int iIndex )
{
	if( m_pPracticeMgr )
	{
		LSC_Practice *pData = m_pPracticeMgr->GetData( iIndex );

		if( pData ) 
			return pData->MapIndex;
	}
	return -1;
}

DWORD PracticeManager::GetLimitTime( int iIndex )
{
	if( m_pPracticeMgr )
	{
		LSC_Practice *pData = m_pPracticeMgr->GetData( iIndex );

		if( pData ) 
			return pData->TimeLimit;
	}
	return 0;
}

PracticeManager::PracticeType PracticeManager::GetPracticeType( int iIndex )
{
	if( m_pPracticeMgr )
	{
		LSC_Practice *pData = m_pPracticeMgr->GetData( iIndex );

		if( pData ) 
			return PracticeType(pData->Type);
	}
	return E_PRACTICE_NONE;
}

DWORD PracticeManager::GetPracticeGradeA( int iIndex )
{
	if( m_pPracticeMgr )
	{
		LSC_Practice *pData = m_pPracticeMgr->GetData( iIndex );

		if( pData ) 
			return pData->GradeA;
	}
	return 0;
}

DWORD PracticeManager::GetPracticeRewardA( int iIndex )
{
	if( m_pPracticeMgr )
	{
		LSC_Practice *pData = m_pPracticeMgr->GetData( iIndex );

		if( pData ) 
			return pData->RewardA;
	}
	return 0;
}


DWORD PracticeManager::GetPracticeGradeB( int iIndex )
{
	if( m_pPracticeMgr )
	{
		LSC_Practice *pData = m_pPracticeMgr->GetData( iIndex );

		if( pData ) 
			return pData->GradeB;
	}
	return 0;
}

DWORD PracticeManager::GetPracticeRewardB( int iIndex )
{
	if( m_pPracticeMgr )
	{
		LSC_Practice *pData = m_pPracticeMgr->GetData( iIndex );

		if( pData ) 
			return pData->RewardB;
	}
	return 0;
}

DWORD PracticeManager::GetPracticeGradeC( int iIndex )
{
	if( m_pPracticeMgr )
	{
		LSC_Practice *pData = m_pPracticeMgr->GetData( iIndex );

		if( pData ) 
			return pData->GradeC;
	}
	return 0;
}

DWORD PracticeManager::GetPracticeRewardC( int iIndex )
{
	if( m_pPracticeMgr )
	{
		LSC_Practice *pData = m_pPracticeMgr->GetData( iIndex );

		if( pData ) 
			return pData->RewardC;
	}
	return 0;
}

int PracticeManager::GetPracticeFreeAdmission( int iIndex )
{
	if( m_pPracticeMgr )
	{
		LSC_Practice *pData = m_pPracticeMgr->GetData( iIndex );

		if( pData ) 
			return pData->FreeAdmission;
	}
	return 0;
}


LSC_Practice* PracticeManager::GetLSCPractice( int iIndex )
{
	if( m_pPracticeMgr )
	{
		LSC_Practice *pData = m_pPracticeMgr->GetData( iIndex );
		return pData;
	}

	return NULL;
}



void PracticeManager::GetPracticeReward( int iIndex, int iGrade, DWORDVec& rvGrade, DWORDVec& rvReward )
{
	if( m_pPracticeMgr )
	{
		LSC_Practice *pData = m_pPracticeMgr->GetData( iIndex );

		if( pData )
		{
			if( E_GRADE_A == iGrade )
			{
				rvGrade.push_back( E_GRADE_A );
				rvGrade.push_back( E_GRADE_B );
				rvGrade.push_back( E_GRADE_C );
				rvReward.push_back( pData->RewardA );
				rvReward.push_back( pData->RewardB );
				rvReward.push_back( pData->RewardC );
			}
			else if( E_GRADE_B == iGrade )
			{
				rvGrade.push_back( E_GRADE_B );
				rvGrade.push_back( E_GRADE_C );
				rvReward.push_back( pData->RewardB );
				rvReward.push_back( pData->RewardC );
			}
			else if( E_GRADE_C == iGrade )
			{
				rvGrade.push_back( E_GRADE_C );
				rvReward.push_back( pData->RewardC );
			}
			else
				return;
		}
	}
	return;
}