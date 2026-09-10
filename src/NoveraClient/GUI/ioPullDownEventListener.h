#pragma once

/*
	풀다운 메뉴의 마우스 오버, 클릭 등의 마우스 및 키보드 등에 대한 이벤트 객체를 정의
*/

class ioPullDownList;
class PullDownEvent
{
private:
	friend class ioPullDownList;
	ioPullDownList* pEventCaller;

public:
	enum
	{		
		NONE_SELECT  = -1,
	};

	//이벤트를 발생시킨 풀 다운 리스트 리턴
	const ioPullDownList* GetEventCaller() const { return pEventCaller; }

	PullDownEvent( ioPullDownList* pCaller )
	{
		pEventCaller = pCaller;
	}
	virtual ~ PullDownEvent(){};
};

/*
	풀다운 메뉴 아이템 마우스 오버 이벤트 객체
*/

class OnOverPullDownEvent : public PullDownEvent
{
public:
	int iOverIdx;
	int iOverImageIdx;

	OnOverPullDownEvent()	: PullDownEvent( NULL )
	{
		iOverIdx		= NONE_SELECT;
		iOverImageIdx	= NONE_SELECT;
	}

	OnOverPullDownEvent( ioPullDownList* pCaller ) : PullDownEvent( pCaller )
	{
		iOverIdx		= NONE_SELECT;
		iOverImageIdx	= NONE_SELECT;
	}
	virtual ~ OnOverPullDownEvent(){};

	bool IsPulldownMouseOver() const
	{
		if( NONE_SELECT != iOverIdx && NONE_SELECT == iOverImageIdx )
			return true;

		return false;
	}

	bool IsImageMouseOver() const
	{
		if( NONE_SELECT != iOverIdx && NONE_SELECT != iOverImageIdx )
			return true;

		return false;
	}
};

/*
	풀다운 메뉴 아이템 마우스 좌클릭 이벤트 객체
*/

class ClickPullDownEvent : public PullDownEvent
{
public:
	int iClickIdx;
	int iClickImageIdx;
	int iSelectChar;

	ClickPullDownEvent()	: PullDownEvent( NULL )
	{
		iClickIdx		= NONE_SELECT;
		iClickImageIdx	= NONE_SELECT;
		iSelectChar		= -1;
	}

	ClickPullDownEvent( ioPullDownList* pCaller ) : PullDownEvent( pCaller )
	{
		iClickIdx		= NONE_SELECT;
		iClickImageIdx	= NONE_SELECT;
		iSelectChar		= -1;
	}
	virtual ~ ClickPullDownEvent(){};

	bool IsClickNone()
	{
		if( NONE_SELECT == iClickIdx && NONE_SELECT == iClickImageIdx )
			return true;

		return false;
	}

	bool IsPulldownClick() const
	{
		if( NONE_SELECT != iClickIdx && NONE_SELECT == iClickImageIdx )
			return true;

		return false;
	}

	bool IsImageClick() const
	{
		if( NONE_SELECT != iClickIdx && NONE_SELECT != iClickImageIdx )
			return true;

		return false;
	}
};

/*
	풀다운 메뉴 아이템 마우스 리브 이벤트 객체
*/

class OnLeavePullDownEvent : public PullDownEvent
{
public:
	int iLeaveIdx;
	int iLeaveImageIdx;

	OnLeavePullDownEvent()	: PullDownEvent( NULL )
	{
		iLeaveIdx		= NONE_SELECT;
		iLeaveImageIdx	= NONE_SELECT;
	}

	OnLeavePullDownEvent( ioPullDownList* pCaller ) : PullDownEvent( pCaller )
	{
		iLeaveIdx		= NONE_SELECT;
		iLeaveImageIdx	= NONE_SELECT;
	}
	virtual ~ OnLeavePullDownEvent(){};

	bool IsPulldownMouseLeave() const
	{
		if( NONE_SELECT != iLeaveIdx )
			return true;

		return false;
	}
	
	bool IsImageMouseLeave() const
	{
		if( NONE_SELECT != iLeaveImageIdx )
			return true;

		return false;
	}
};

/*
	풀다운 메뉴 아이템 마우스 이벤트 처리 인터페이스
*/

class PullDownEventListener
{
public:
	virtual void iwm_pulldown_event( const PullDownEvent& Event ) PURE;
};