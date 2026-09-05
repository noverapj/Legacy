#include "stdafx.h"

#include "ioItemExpert.h"


ioItemExpert::ioItemExpert()
{
}

ioItemExpert::~ioItemExpert()
{
	m_ExpertInfoList.clear();
}

void ioItemExpert::LoadExpertInfo( ioINILoader &rkLoader )
{
	m_ExpertInfoList.clear();
	
	char szKeyName[MAX_PATH];
	char szBuf[MAX_PATH*2];
	
	int iMaxLevel = rkLoader.LoadInt_e( "max_level", 0 );
	for(int i = 0; i < iMaxLevel; i++)
	{
		ExpertInfo temp;
		ioHashString szValueType;

		wsprintf_e(szKeyName, "level%d_reach_expert", i+1);
		temp.iReachExpert = rkLoader.LoadInt( szKeyName, 0 );

		memset(szKeyName, 0, sizeof(szKeyName));
		wsprintf_e(szKeyName, "level%d_target", i+1);
		rkLoader.LoadString( szKeyName, "" , szBuf, MAX_PATH*2);
		temp.szTargetKeyName = szBuf;

		memset(szKeyName, 0, sizeof(szKeyName));
		wsprintf_e(szKeyName, "level%d_value_type", i+1);
		memset(szBuf, 0, sizeof(szBuf));
		rkLoader.LoadString( szKeyName, "", szBuf, MAX_PATH*2);
		szValueType = szBuf;

		memset(szKeyName, 0, sizeof(szKeyName));
		wsprintf_e(szKeyName, "level%d_value", i+1);
		memset(szBuf, 0, sizeof(szBuf));
		if(strcmp(szValueType.c_str(), "int") == 0)
		{
			temp.iValue = rkLoader.LoadInt( szKeyName, 0);
		}
		else if(szValueType == "float")
		{
			temp.fValue = rkLoader.LoadFloat( szKeyName, 0.0f);
		}
		else if(szValueType == "string")
		{
			rkLoader.LoadString( szKeyName, "" , szBuf, MAX_PATH*2 );
			temp.szValue = szBuf;
		}
		else
		{
			LOG.PrintTimeAndLog( 0, "ioItem::LoadLevelExpert - Error [%s]", szValueType.c_str());
			continue;
		}
		
		memset(szKeyName, 0, sizeof(szKeyName));
		wsprintf_e(szKeyName, "level%d_desc", i+1);

		memset(szBuf, 0, sizeof(szBuf));
		rkLoader.LoadString( szKeyName, "", szBuf, MAX_PATH*2);
		temp.szDesc = szBuf;

		memset(szKeyName, 0, sizeof(szKeyName));
		wsprintf_e(szKeyName, "level%d_peso", i+1);
		temp.iPeso = rkLoader.LoadInt( szKeyName, 0);
		
		m_ExpertInfoList.push_back(temp);
	}
}

const ioItemExpert::ExpertInfo* ioItemExpert::GetExpertInfo(int iLevel)
{
	if(m_ExpertInfoList.empty()) return NULL;
	
	int iSize = m_ExpertInfoList.size();
	int iLevelInList = iLevel - 1; 
	if( !COMPARE( iLevelInList , 0, iSize) ) return NULL;

	return &(m_ExpertInfoList[iLevelInList]);
}

const ioHashString* ioItemExpert::GetExpertDesc(int iLevel) 
{
	const ExpertInfo *pExpertInfo = GetExpertInfo(iLevel);
	if(!pExpertInfo) return NULL;

	return &pExpertInfo->szDesc;
}

int ioItemExpert::GetReachExpert(int iLevel)
{
	const ExpertInfo *pExpertInfo = GetExpertInfo(iLevel);
	if(!pExpertInfo) return -1;

	return pExpertInfo->iReachExpert;
}

const ioHashString* ioItemExpert::GetExpertKeyName(int iLevel)
{
	const ExpertInfo *pExpertInfo = GetExpertInfo(iLevel);
	if(!pExpertInfo) return NULL;

	return &pExpertInfo->szTargetKeyName;
}

void ioItemExpert::AddExpertValue(int iLevel, int &iDestValue)
{
	const ExpertInfo *pExpertInfo = GetExpertInfo(iLevel);
	if(!pExpertInfo) return;

	iDestValue += pExpertInfo->iValue;
}

void ioItemExpert::AddExpertValue(int iLevel, DWORD &dwDestValue)
{
	const ExpertInfo *pExpertInfo = GetExpertInfo(iLevel);
	if(!pExpertInfo) return;

	dwDestValue += (DWORD)pExpertInfo->iValue;
}

void ioItemExpert::AddExpertValue(int iLevel, float &fDestValue)
{
	const ExpertInfo *pExpertInfo = GetExpertInfo(iLevel);
	if(!pExpertInfo) return;

	fDestValue += pExpertInfo->fValue;
}

void ioItemExpert::AddExpertValue(int iLevel, ioHashString &szDestValue)
{
	const ExpertInfo *pExpertInfo = GetExpertInfo(iLevel);
	if(!pExpertInfo) return;

	szDestValue = pExpertInfo->szValue;
}

int ioItemExpert::GetPeso(int iLevel)
{
	const ExpertInfo *pExpertInfo = GetExpertInfo(iLevel);
	if(!pExpertInfo) return -1;

	return pExpertInfo->iPeso;
}