
//----------------------------------------------------------------------------------
/*
Filename	: ioRenderElementAnimationFrame.h
Author		: megamegy
Date		: 2013.10.31
Desc		: 
*/
//----------------------------------------------------------------------------------
#ifndef _IORENDERELEMENTANIMATIONFRAME_H_
#define _IORENDERELEMENTANIMATIONFRAME_H_

class ioUIRenderElement;
typedef std::vector< ioUIRenderElement* > RenderElementVec;

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
ioUIAnimationFrame
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
class ioRenderElementAnimationFrame
{
public:

	//----------------------------------------------------------------------------------
	/*
	ElementInfo : 계산을 위해 필요한것들
	*/
	struct ElementInfo
	{
		ElementInfo( ioUIRenderElement* element ) 
			: m_element( element ), m_speed( 0.0f ) {}

		ioUIRenderElement*		m_element;
		float					m_speed;
	};
	typedef std::vector< ElementInfo > ElementInfos;

public:

	//----------------------------------------------------------------------------------
	/*
	생성 / 소멸
	*/
	ioRenderElementAnimationFrame( ioUIRenderElement* element, DWORD targettime );
	ioRenderElementAnimationFrame( const RenderElementVec& elements, DWORD targettime );
	virtual ~ioRenderElementAnimationFrame();
	
	//----------------------------------------------------------------------------------
	/*
	Update : 처리가 완료되었다면 true를 리턴한다.
	*/
	virtual bool				Update();

	//----------------------------------------------------------------------------------
	/*
	진입점과 끝나는 점
	*/
	virtual void				OnBegin();
	virtual void				OnEnd() {}
	
protected:

	//----------------------------------------------------------------------------------
	/*
	각각의 요소를 update하는 함수
	*/
	virtual bool				UpdateElement( DWORD dt, ElementInfo& info, int index ) = 0;

	//----------------------------------------------------------------------------------
	/*
	시간이 0일경우 target값으로 즉시 셋팅하는 함수
	*/
	virtual void				SetTargetValue() {}

protected:

	ElementInfos				m_elements;
	
	DWORD						m_lastupdatetime;
	DWORD						m_targettime;
};

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
ioRenderElementAnimationFrameEmpty
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
class ioRenderElementAnimationFrameEmpty : public ioRenderElementAnimationFrame
{
public:

	//----------------------------------------------------------------------------------
	/*
	생성 / 소멸
	*/
	ioRenderElementAnimationFrameEmpty( ioUIRenderElement* element, DWORD targettime );
	ioRenderElementAnimationFrameEmpty( const RenderElementVec& elements, DWORD targettime );
	virtual ~ioRenderElementAnimationFrameEmpty();
	
	//----------------------------------------------------------------------------------
	/*
	Update : 처리가 완료되었다면 true를 리턴한다.
	*/
	virtual bool	Update();

	//----------------------------------------------------------------------------------
	/*
	진입점과 끝나는 점
	*/
	virtual void	OnBegin();

protected:

	//----------------------------------------------------------------------------------
	/*
	각각의 요소를 update
	*/
	virtual bool	UpdateElement( DWORD dt, ElementInfo& info, int index ) { return false; }

protected:

	DWORD			m_acctime;
};


//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
ioRenderElementAnimationFrameScale
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
class ioRenderElementAnimationFrameScale : public ioRenderElementAnimationFrame
{
public:

	//----------------------------------------------------------------------------------
	/*
	생성 / 소멸
	*/
	ioRenderElementAnimationFrameScale( ioUIRenderElement* element, const D3DXVECTOR2& targetscale, DWORD targettime );
	ioRenderElementAnimationFrameScale( const RenderElementVec& elements, const D3DXVECTOR2& targetscale, DWORD targettime );
	virtual ~ioRenderElementAnimationFrameScale();
	
	//----------------------------------------------------------------------------------
	/*
	진입점과 끝나는 점
	*/
	virtual void	OnBegin();

protected:

	//----------------------------------------------------------------------------------
	/*
	각각의 요소를 update
	*/
	virtual bool	UpdateElement( DWORD dt, ElementInfo& info, int index );

	//----------------------------------------------------------------------------------
	/*
	시간이 0일경우 target값으로 즉시 셋팅하는 함수
	*/
	virtual void	SetTargetValue();

protected:

	D3DXVECTOR2		m_targetscale;
};


//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
ioRenderElementAnimationFrameAlpha
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
class ioRenderElementAnimationFrameAlpha : public ioRenderElementAnimationFrame
{
public:

	//----------------------------------------------------------------------------------
	/*
	생성 / 소멸
	*/
	ioRenderElementAnimationFrameAlpha( ioUIRenderElement* element, float targetalpha, DWORD targettime );
	ioRenderElementAnimationFrameAlpha( const RenderElementVec& elements, float targetalpha, DWORD targettime );
	virtual ~ioRenderElementAnimationFrameAlpha();
	
	//----------------------------------------------------------------------------------
	/*
	진입점과 끝나는 점
	*/
	virtual void	OnBegin();

protected:

	//----------------------------------------------------------------------------------
	/*
	각각의 요소를 update하는 함수
	*/
	virtual bool	UpdateElement( DWORD dt, ElementInfo& info, int index );

	//----------------------------------------------------------------------------------
	/*
	시간이 0일경우 target값으로 즉시 셋팅하는 함수
	*/
	virtual void	SetTargetValue();

protected:

	float			m_targetalpha;
};

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
ioRenderElementAnimationFrameSize
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
class ioRenderElementAnimationFrameSize : public ioRenderElementAnimationFrame
{
	// 이건 size는 int형으로 저장이 되서 따로 이렇게 기억해야한다...;;;
	typedef std::vector< D3DXVECTOR2 > ElementSubInfos;

public:

	//----------------------------------------------------------------------------------
	/*
	생성 / 소멸
	*/
	ioRenderElementAnimationFrameSize( ioUIRenderElement* element, const D3DXVECTOR2& targetsize, DWORD targettime );
	ioRenderElementAnimationFrameSize( const RenderElementVec& elements, const D3DXVECTOR2& targetsize, DWORD targettime );
	virtual ~ioRenderElementAnimationFrameSize();
	
	//----------------------------------------------------------------------------------
	/*
	진입점과 끝나는 점
	*/
	virtual void		OnBegin();

protected:

	//----------------------------------------------------------------------------------
	/*
	각각의 요소를 update하는 함수
	*/
	virtual bool		UpdateElement( DWORD dt, ElementInfo& info, int index );

	//----------------------------------------------------------------------------------
	/*
	시간이 0일경우 target값으로 즉시 셋팅하는 함수
	*/
	virtual void		SetTargetValue();

protected:
		
	D3DXVECTOR2			m_targetsize;

	ElementSubInfos		m_cursizes;
};

#endif // _IORENDERELEMENTANIMATIONFRAME_H_