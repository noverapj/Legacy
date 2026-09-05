#include "stdafx.h"
#include "TitleTriggerBase.h"
#include "TitleTriggerExtend.h"
#include "UserTitleInfo.h"
#include "TitleData.h"

TitleData::TitleData()
{
	Init();
}

TitleData::~TitleData()
{
	Destroy();
}

void TitleData::Init()
{
	m_dwPrecedeCode		= 0;
	m_pTrigger			= NULL;
	m_eClass			= TITLE_NONE;
}

void TitleData::Destroy()
{
	if( m_pTrigger )
		delete m_pTrigger;

	/*if( m_pPrimiumTrigger )
		delete m_pPrimiumTrigger;*/
}

void TitleData::CreateTitleByDesignated(TitleTriggerClasses eClass, TitleTriggerClasses ePrimiumClass, const DWORD dwCode, const __int64 iValue, const int iMaxLevel, const DWORD dwPrecedeCode)
{
	DefaultTitleData::CreateTitleInfo(dwCode, iValue, iMaxLevel, FALSE, FALSE);
	SetClass(eClass);
	SetPrecedeCode(dwPrecedeCode);

	switch( eClass )
	{
	case TITLE_CONSUME_ACCUMULATE_GOLD:
	case TITLE_CONSUME_PRESENT_GOLD:
	case TITLE_CONSUME_ACCUMULATE_PESO:
		m_pTrigger	= new TitleTriggerAccumulateEvent;
		break;
	case TITLE_HAVE_GOLD:
	case TITLE_HAVE_PESO:
		m_pTrigger	= new TitleTriggerNoAction;
		break;
	}

	if( m_pTrigger )
		m_pTrigger->Create(iValue);
}

BOOL TitleData::DoTrigger(UserTitleInfo* pInfo, const __int64 iValue)
{
	if( !pInfo )
		return FALSE;

	if( m_pTrigger )
		return m_pTrigger->DoTrigger(pInfo, iValue);

	return FALSE;
}

//BOOL TitleData::DoTriggerPrimium(UserTitleInfo* pInfo, const __int64 iValue)
//{
//	if( !pInfo )
//		return FALSE;
//
//	if( m_pPrimiumTrigger )
//		return m_pPrimiumTrigger->DoTrigger(pInfo, iValue);
//
//	return FALSE;
//}

BOOL TitleData::IsComplete(const __int64 iValue)
{
	if( m_pTrigger )
		return m_pTrigger->IsComplete(iValue);

	return FALSE;
}