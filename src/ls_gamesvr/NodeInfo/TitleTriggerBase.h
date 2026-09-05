#pragma once

class UserTitleInfo;

enum TitleTriggerClasses
{
	TITLE_NONE						= 0,
	TITLE_CONSUME_ACCUMULATE_GOLD	= 1,
	TITLE_HAVE_GOLD					= 2,
	TITLE_CONSUME_PRESENT_GOLD		= 3,
	TITLE_CONSUME_ACCUMULATE_PESO	= 4,
	TITLE_HAVE_PESO					= 5,

	//primium 
	TITLE_PRIMIUM_ACCUMULATE_TIME	= 6, 
};

class TitleTriggerBase
{
public:
	TitleTriggerBase();
	virtual ~TitleTriggerBase();

	void Init();
	void Destroy();

public:
	virtual BOOL DoTrigger(UserTitleInfo* pData, const __int64 iValue) = 0;
	
	virtual BOOL IsComplete(const __int64 iValue);
	virtual void Create(const __int64 iValue);

public:
	__int64 GetValue();

protected:
	__int64 m_iValue;
};