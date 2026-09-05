#include "stdafx.h"

#include "ioClassExpert.h"

ioClassExpert::ioClassExpert()
{
	Initialize();
}

ioClassExpert::~ioClassExpert()
{
	
}

void ioClassExpert::Initialize()
{
	m_vClassInfo.clear();
}

void ioClassExpert::ApplyClassInfo( SP2Packet &rkPacket )
{
	Initialize();

	int iSize;
	rkPacket >> iSize;
	for(int i = 0;i < iSize;i++)
	{
		CLASSSLOT kSlot;
		rkPacket >> kSlot.m_iType >> kSlot.m_iLevel >> kSlot.m_iExpert >> kSlot.m_ePowerUpCharGrade;
		if( kSlot.m_iType != 0 )
		{
			AddClassInfo( kSlot );
		}
	}
}

void ioClassExpert::AddClassInfo( CLASSSLOT &kSlot )
{
	CreateClass( kSlot.m_iType );

	vClassInfo::iterator iter, iEnd;
	iEnd = m_vClassInfo.end();
	for(iter = m_vClassInfo.begin();iter != iEnd;iter++)
	{
		ClassInfo &kClassInfo = *iter;
		for(int i = 0;i < MAX_SLOT;i++)
		{
			if( kClassInfo.m_Data[i].m_iType == kSlot.m_iType )
			{
				memcpy( &kClassInfo.m_Data[i], &kSlot, sizeof( kSlot ) );
				return;
			}
		} 
	}
}

void ioClassExpert::CreateClass( int iClassType )
{
	if( iClassType <= 0 ) return;

	vClassInfo::iterator iter, iEnd;
	iEnd = m_vClassInfo.end();
	for(iter = m_vClassInfo.begin();iter != iEnd;iter++)
	{
		ClassInfo &kClassInfo = *iter;
		for(int i = 0;i < MAX_SLOT;i++)
		{
			if( kClassInfo.m_Data[i].m_iType == iClassType )
				return;
		}		
	}

	// ½Å±Ô ¿ëº´ ±¸¸Å ºó ½½·Ô È®ÀÎ.
	for(iter = m_vClassInfo.begin();iter != iEnd;iter++)
	{
		ClassInfo &kClassInfo = *iter;
		for(int i = 0;i < MAX_SLOT;i++)
		{
			if( kClassInfo.m_Data[i].m_iType == 0 )
			{
				kClassInfo.m_Data[i].m_iType	= iClassType;
				kClassInfo.m_Data[i].m_iLevel	= 0;
				kClassInfo.m_Data[i].m_iExpert	= 0;
				return;
			}
		}		
	}

	// ºó ½½·Ô ¾ø´Ù. ½½·Ô Ãß°¡
	ClassInfo kClassInfo;
	kClassInfo.m_Data[0].m_iType	= iClassType;
	kClassInfo.m_Data[0].m_iLevel	= 0;
	kClassInfo.m_Data[0].m_iExpert	= 0;
	m_vClassInfo.push_back( kClassInfo );
}

int  ioClassExpert::GetClassLevel( int iClassType )
{
	vClassInfo::iterator iter, iEnd;
	iEnd = m_vClassInfo.end();
	for(iter = m_vClassInfo.begin();iter != iEnd;iter++)
	{
		ClassInfo &kClassInfo = *iter;
		for(int i = 0;i < MAX_SLOT;i++)
		{
			if( kClassInfo.m_Data[i].m_iType == iClassType )
				return kClassInfo.m_Data[i].m_iLevel;
		}		
	}
	return 0;
}

int ioClassExpert::GetClassExpert( int iClassType )
{
	vClassInfo::iterator iter, iEnd;
	iEnd = m_vClassInfo.end();
	for(iter = m_vClassInfo.begin(); iter != iEnd; iter++ )
	{
		ClassInfo &kClassInfo = *iter;
		for(int i = 0;i < MAX_SLOT;i++)
		{
			if( kClassInfo.m_Data[i].m_iType == iClassType )
				return kClassInfo.m_Data[i].m_iExpert;
		}
	}

	return 0;
}

bool ioClassExpert::AddClassExp( int iClassType, int iExp, int &rRemainExp )
{
	vClassInfo::iterator iter, iEnd;
	iEnd = m_vClassInfo.end();
	for(iter = m_vClassInfo.begin();iter != iEnd;iter++)
	{
		ClassInfo &kClassInfo = *iter;
		for(int i = 0;i < MAX_SLOT;i++)
		{
			if( kClassInfo.m_Data[i].m_iType == iClassType )
			{
				int iNextExp = g_LevelMgr.GetNextLevelupExp( kClassInfo.m_Data[i].m_iLevel );
				kClassInfo.m_Data[i].m_iExpert += iExp;
				if( iNextExp <= kClassInfo.m_Data[i].m_iExpert )
				{
					kClassInfo.m_Data[i].m_iLevel++;
					rRemainExp = kClassInfo.m_Data[i].m_iExpert - iNextExp;
					kClassInfo.m_Data[i].m_iExpert = 0;
					return true;
				}				
				return false;
			}
		}		
	}
	CreateClass( iClassType );
	return AddClassExp( iClassType, iExp, rRemainExp );
}

bool ioClassExpert::AddPowerUpGrade( int iClassType, byte ePowerUpCharGrade )
{
	vClassInfo::iterator iter, iEnd;
	iEnd = m_vClassInfo.end();
	for( iter = m_vClassInfo.begin(); iter != iEnd; iter++ )
	{
		ClassInfo &kClassInfo = *iter;
		for( int i = 0; i < MAX_SLOT; i++ )
		{
			if( kClassInfo.m_Data[i].m_iType == iClassType )
			{
				kClassInfo.m_Data[i].m_ePowerUpCharGrade = ePowerUpCharGrade;
				return true;
			}
		}
	}

	CreateClass( iClassType );
	return AddPowerUpGrade( iClassType, ePowerUpCharGrade );
}

bool ioClassExpert::SetClassExp( int iClassType, int iExp )
{
	vClassInfo::iterator iter, iEnd;
	iEnd = m_vClassInfo.end();
	for(iter = m_vClassInfo.begin();iter != iEnd;iter++)
	{
		ClassInfo &kClassInfo = *iter;
		for(int i = 0;i < MAX_SLOT;i++)
		{
			if( kClassInfo.m_Data[i].m_iType == iClassType )
			{
				kClassInfo.m_Data[i].m_iExpert = iExp;
				return true;
			}
		}
	}

	return false;
}

bool ioClassExpert::SetClassLevel( int iClassType, int iLevel )
{
	vClassInfo::iterator iter, iEnd;
	iEnd = m_vClassInfo.end();
	for(iter = m_vClassInfo.begin();iter != iEnd;iter++)
	{
		ClassInfo &kClassInfo = *iter;
		for(int i = 0;i < MAX_SLOT;i++)
		{
			if( kClassInfo.m_Data[i].m_iType == iClassType )
			{
				kClassInfo.m_Data[i].m_iLevel = iLevel;
				return true;
			}
		}
	}

	return false;
}

int  ioClassExpert::GetTopLevelClassType()
{
	int iClassType   = 1; // default ¾ÆÀÌ¾ð
	int iClassLevel  = 0;
	__int64 iClassExpert = 0;

	vClassInfo::iterator iter, iEnd;
	iEnd = m_vClassInfo.end();
	for(iter = m_vClassInfo.begin();iter != iEnd;iter++)
	{
		ClassInfo &kClassInfo = *iter;
		for(int i = 0;i < MAX_SLOT;i++)
		{
			if( kClassInfo.m_Data[i].m_iType == 0 ) continue;

			if( kClassInfo.m_Data[i].m_iLevel > iClassLevel )
			{
				iClassType	 = kClassInfo.m_Data[i].m_iType;
				iClassLevel	 = kClassInfo.m_Data[i].m_iLevel;
				iClassExpert = kClassInfo.m_Data[i].m_iExpert;
			}
			else if( kClassInfo.m_Data[i].m_iLevel == iClassLevel )
			{
				if( kClassInfo.m_Data[i].m_iExpert > iClassExpert )
				{
					iClassType	 = kClassInfo.m_Data[i].m_iType;
					iClassLevel	 = kClassInfo.m_Data[i].m_iLevel;
					iClassExpert = kClassInfo.m_Data[i].m_iExpert;
				}
			}
		}		
	}
	return iClassType;
}

const ioClassExpert::CLASSSLOT* ioClassExpert::GetClassExpertSlot( int iClassType )
{
	vClassInfo::iterator iter, iEnd;
	iEnd = m_vClassInfo.end();

	static CLASSSLOT emptySlot;
	for( iter = m_vClassInfo.begin(); iter != iEnd; iter++ )
	{
		ClassInfo &kClassInfo = *iter;
		for( int i = 0; i < MAX_SLOT; i++ )
		{
			if( kClassInfo.m_Data[i].m_iType == iClassType )
				return &kClassInfo.m_Data[i];
		}
	}

	return NULL;
}