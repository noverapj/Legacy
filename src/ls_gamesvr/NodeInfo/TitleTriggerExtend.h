#pragma once

#include "TitleTriggerBase.h"

//class TitleTriggerConsumeAccumulateGold : public TitleTriggerBase
//{
//public:
//	virtual BOOL DoTrigger(UserTitleInfo* pData, const DWORD dwValue);
//};


class TitleTriggerAccumulateEvent : public TitleTriggerBase
{
public:
	virtual BOOL DoTrigger(UserTitleInfo* pData, const __int64  iValue);
};

class TitleTriggerNoAction : public TitleTriggerBase
{
public:
	virtual BOOL DoTrigger(UserTitleInfo* pData, const __int64  iValue);
};

//class TitleTriggerHaveGold : public TitleTriggerBase
//{
//public:
//	virtual BOOL DoTrigger(UserTitleInfo* pData, const DWORD dwValue);
//};
//
//class TitleTriggerConsumePresentGold : public TitleTriggerBase
//{
//public:
//	virtual BOOL DoTrigger(UserTitleInfo* pData, const DWORD dwValue);
//};
//
//class TitleTriggerConsumeAccumulatePeso : public TitleTriggerBase
//{
//public:
//	virtual BOOL DoTrigger(UserTitleInfo* pData, const DWORD dwValue);
//};
//
//class TitleTriggerHavePeso : public TitleTriggerBase
//{
//public:
//	virtual BOOL DoTrigger(UserTitleInfo* pData, const DWORD dwValue);
//};
