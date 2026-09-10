
//----------------------------------------------------------------------------------
/*
Filename	: ioRenderElementAnimation.h
Author		: megamegy
Date		: 2013.10.31
Desc		: 
*/
//----------------------------------------------------------------------------------
#ifndef _IORENDERELEMENTANIMATION_H_
#define _IORENDERELEMENTANIMATION_H_

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
ioRenderElementAnimation
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
class ioRenderElementAnimationFrame;
class ioRenderElementAnimation
{
	typedef std::vector< ioRenderElementAnimationFrame* > FrameVector;

public:

	//----------------------------------------------------------------------------------
	/*
	»ý¼º / ¼Ò¸ê
	*/
	ioRenderElementAnimation();
	virtual ~ioRenderElementAnimation();

	//----------------------------------------------------------------------------------
	/*
	Update
	*/
	virtual void							Update();

	//----------------------------------------------------------------------------------
	/*
	Controls
	*/
	virtual void							Play( bool loop );
	virtual void							Stop();
	virtual bool							IsPlaying() const;

	//----------------------------------------------------------------------------------
	/*
	Frame
	*/
	int										GetNumFrame() const { return static_cast<int>( m_frames.size() ); }
	const ioRenderElementAnimationFrame*	GetFrame( int index ) const;
	void									AppendFrame( ioRenderElementAnimationFrame* frame );
	void									ClearFrames();

protected:

	int										m_curframe;
	bool									m_loop;
	FrameVector								m_frames;
};

#endif // _IORENDERELEMENTANIMATION_H_