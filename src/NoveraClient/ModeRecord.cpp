

#include "stdafx.h"

#include "ModeRecord.h"
#include "ioBaseChar.h"

ModeRecord::ModeRecord()
{
	pChar = NULL;

	InitValues();
}

void ModeRecord::InitValues()
{
	dwCurDieTime				= 0;
	dwRevivalGap				= 0;
	iRevivalCnt					= 0;

	iCurRank					= 0;
	iPreRank					= 0;
	
	bResultShow					= false;
	bResultLevelUP				= false;
	iResultLevel				= 0;
	iResultTotalExp				= 0;
	iResultTotalPeso			= 0;
	iUniqueTotalKill			= 0;
	iUniqueTotalDeath			= 0;
	iResultTotalLadderPoint		= 0;
	iResultTreasureCardCount	= 0;
	iPvEClassType				= 0;
	dwPvECharIndex				= 0;
	vResultClassType.clear();
	vResultClassGender.clear();
	iNoDeathKillCount = 0;
}

int ModeRecord::GetAllTotalKill()
{
	int iAllTotal = 0;
	KillDeathInfoMap::iterator iter = iKillInfoMap.begin();
	while( iter != iKillInfoMap.end() )
	{
		iAllTotal += iter->second;

		++iter;
	}

	return ( iAllTotal / 10 );
}

int ModeRecord::GetAllTotalDeath()
{
	int iAllTotal = 0;
	KillDeathInfoMap::iterator iter = iDeathInfoMap.begin();
	while( iter != iDeathInfoMap.end() )
	{
		iAllTotal += iter->second;

		++iter;
	}

	return ( iAllTotal / 10 );
}

int ModeRecord::GetUniqueTotalKill() const
{
	return iUniqueTotalKill;
}

int ModeRecord::GetUniqueTotalDeath() const
{
	return iUniqueTotalDeath;
}

const int& ModeRecord::GetNoDeathKillCount() const
{
	return iNoDeathKillCount;
}

void ModeRecord::AddKill( ModeType eMode, int iCount, bool bDropKill, bool bBestAttacker )
{
	if( !pChar )	return;

	int iClassType = pChar->GetCharacterInfo().m_class_type;
	KillDeathInfoMap::iterator iter = iKillInfoMap.find( iClassType );
	if( iter != iKillInfoMap.end() )
	{
		iter->second += iCount;

		if( pChar->IsOwnerChar() )
			g_MyInfo.AddKill( eMode, iCount, bDropKill, bBestAttacker );

		if( !bBestAttacker )
		{
			if( iCount >= 0 )
			{
				iUniqueTotalKill++;
				iNoDeathKillCount++;
			}
			else
				iUniqueTotalKill = max( 0, iUniqueTotalKill - 1 );
		}
	}
	else
	{
		iKillInfoMap.insert( KillDeathInfoMap::value_type( iClassType, iCount ));

		if( pChar->IsOwnerChar() )
			g_MyInfo.AddKill( eMode, iCount, bDropKill, bBestAttacker );

		if( !bBestAttacker )
		{
			if( iCount >= 0 )
			{
				iNoDeathKillCount++;
				iUniqueTotalKill++;
			}
			else
				iUniqueTotalKill = max( 0, iUniqueTotalKill - 1 );
		}
	}
}

void ModeRecord::AddDeath( ModeType eMode, int iCount )
{
	if( !pChar )	return;

	int iClassType = pChar->GetCharacterInfo().m_class_type;
	KillDeathInfoMap::iterator iter = iDeathInfoMap.find( iClassType );
	if( iter != iDeathInfoMap.end() )
	{
		iter->second += iCount;

		if( pChar->IsOwnerChar() )
			g_MyInfo.AddDeath( eMode, iCount );

		iUniqueTotalDeath++;
		iNoDeathKillCount = 0;
	}
	else
	{
		iDeathInfoMap.insert( KillDeathInfoMap::value_type( iClassType, iCount ));

		if( pChar->IsOwnerChar() )
			g_MyInfo.AddDeath( eMode, iCount );

		iUniqueTotalDeath++;
		iNoDeathKillCount = 0;
	}
}

void TrainingRecord::AddKill( ModeType eMode, int iCount, bool bDropKill, bool bBestAttacker )
{
	if( !pChar )	return;

	int iClassType = pChar->GetCharacterInfo().m_class_type;
	KillDeathInfoMap::iterator iter = iKillInfoMap.find( iClassType );
	if( iter != iKillInfoMap.end() )
	{
		iter->second += iCount;

		if( !bBestAttacker )
		{
			if( iCount >= 0 )
				iUniqueTotalKill++;
			else
				iUniqueTotalKill = max( 0, iUniqueTotalKill - 1 );
		}
	}
	else
	{
		iKillInfoMap.insert( KillDeathInfoMap::value_type( iClassType, iCount ));

		if( !bBestAttacker )
		{
			if( iCount >= 0 )
				iUniqueTotalKill++;
			else
				iUniqueTotalKill = max( 0, iUniqueTotalKill - 1 );
		}
	}
}

void TrainingRecord::AddDeath( ModeType eMode, int iCount )
{
	if( !pChar )	return;

	int iClassType = pChar->GetCharacterInfo().m_class_type;
	KillDeathInfoMap::iterator iter = iDeathInfoMap.find( iClassType );
	if( iter != iDeathInfoMap.end() )
	{
		iter->second += iCount;
		iUniqueTotalDeath++;
	}
	else
	{
		iDeathInfoMap.insert( KillDeathInfoMap::value_type( iClassType, iCount ));
		iUniqueTotalDeath++;
	}
}

void HeadquartersRecord::AddKill( ModeType eMode, int iCount, bool bDropKill, bool bBestAttacker )
{
	if( !pChar )	return;

	int iClassType = pChar->GetCharacterInfo().m_class_type;
	KillDeathInfoMap::iterator iter = iKillInfoMap.find( iClassType );
	if( iter != iKillInfoMap.end() )
	{
		iter->second += iCount;

		if( !bBestAttacker )
		{
			if( iCount >= 0 )
			{
				iUniqueTotalKill++;
				iNoDeathKillCount++;
			}
			else
				iUniqueTotalKill = max( 0, iUniqueTotalKill - 1 );
		}
	}
	else
	{
		iKillInfoMap.insert( KillDeathInfoMap::value_type( iClassType, iCount ));

		if( !bBestAttacker )
		{
			if( iCount >= 0 )
			{
				iUniqueTotalKill++;
				iNoDeathKillCount++;
			}
			else
				iUniqueTotalKill = max( 0, iUniqueTotalKill - 1 );
		}
	}
}

void HeadquartersRecord::AddDeath( ModeType eMode, int iCount )
{
	if( !pChar )	return;

	int iClassType = pChar->GetCharacterInfo().m_class_type;
	KillDeathInfoMap::iterator iter = iDeathInfoMap.find( iClassType );
	if( iter != iDeathInfoMap.end() )
	{
		iter->second += iCount;
		iUniqueTotalDeath++;
		iNoDeathKillCount = 0;
	}
	else
	{
		iDeathInfoMap.insert( KillDeathInfoMap::value_type( iClassType, iCount ));
		iUniqueTotalDeath++;
		iNoDeathKillCount = 0;
	}
}



void HouseRecord::AddKill( ModeType eMode, int iCount, bool bDropKill, bool bBestAttacker )
{
	if( !pChar )	return;

	int iClassType = pChar->GetCharacterInfo().m_class_type;
	KillDeathInfoMap::iterator iter = iKillInfoMap.find( iClassType );
	if( iter != iKillInfoMap.end() )
	{
		iter->second += iCount;

		if( !bBestAttacker )
		{
			if( iCount >= 0 )
			{
				iUniqueTotalKill++;
				iNoDeathKillCount++;
			}
			else
				iUniqueTotalKill = max( 0, iUniqueTotalKill - 1 );
		}
	}
	else
	{
		iKillInfoMap.insert( KillDeathInfoMap::value_type( iClassType, iCount ));

		if( !bBestAttacker )
		{
			if( iCount >= 0 )
			{
				iUniqueTotalKill++;
				iNoDeathKillCount++;
			}
			else
				iUniqueTotalKill = max( 0, iUniqueTotalKill - 1 );
		}
	}
}

void HouseRecord::AddDeath( ModeType eMode, int iCount )
{
	if( !pChar )	return;

	int iClassType = pChar->GetCharacterInfo().m_class_type;
	KillDeathInfoMap::iterator iter = iDeathInfoMap.find( iClassType );
	if( iter != iDeathInfoMap.end() )
	{
		iter->second += iCount;
		iUniqueTotalDeath++;
		iNoDeathKillCount = 0;
	}
	else
	{
		iDeathInfoMap.insert( KillDeathInfoMap::value_type( iClassType, iCount ));
		iUniqueTotalDeath++;
		iNoDeathKillCount = 0;
	}
}
