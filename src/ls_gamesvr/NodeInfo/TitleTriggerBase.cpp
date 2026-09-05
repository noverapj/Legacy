#include "stdafx.h"
#include "TitleTriggerBase.h"

TitleTriggerBase::TitleTriggerBase()
{
	Init();
}

TitleTriggerBase::~TitleTriggerBase()
{
	Destroy();
}

void TitleTriggerBase::Init()
{
	m_iValue	= 0;
}

void TitleTriggerBase::Destroy()
{
}

__int64 TitleTriggerBase::GetValue()
{
	return m_iValue;
}

void TitleTriggerBase::Create(const __int64 iValue)
{
	m_iValue	= iValue;
}

BOOL TitleTriggerBase::IsComplete(const __int64 iValue)
{
	if( iValue >= GetValue() )
		return TRUE;

	return FALSE;
}