
//----------------------------------------------------------------------------------
/*
Filename	: ioRenderElementAnimation.cpp
Author		: megamegy
Date		: 2013.10.31
Desc		: 
*/
//----------------------------------------------------------------------------------

#include "stdafx.h"

#include "ioRenderElementAnimation.h"
#include "ioRenderElementAnimationFrame.h"
//#include "../io3DEngine/ioWnd.h"

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
ioRenderElementAnimation
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
/*
»ý¼º / ¼Ò¸ê
*/
ioRenderElementAnimation::ioRenderElementAnimation()
	: m_curframe( -1 )
	, m_loop( false )
{
}

ioRenderElementAnimation::~ioRenderElementAnimation()
{
	ClearFrames();
}

//----------------------------------------------------------------------------------
/*
Update
*/
void	ioRenderElementAnimation::Update()
{
	int numframe = GetNumFrame();
	if ( m_curframe < 0 || m_curframe >= numframe ) {
		return;
	}

	if ( m_frames[ m_curframe ]->Update() ) 
	{
		m_frames[ m_curframe ]->OnEnd();

		++m_curframe;
		if ( m_loop ) {
			m_curframe %= numframe;
		}

		if ( m_curframe >= 0 && m_curframe < GetNumFrame() ) {
			m_frames[ m_curframe ]->OnBegin();
		}
	}
}

//----------------------------------------------------------------------------------
/*
Controls
*/
void	ioRenderElementAnimation::Play( bool loop )
{
	m_loop = loop;
	m_curframe = 0;

	if ( m_frames.empty() ) {
		return;
	}
	m_frames[ m_curframe ]->OnBegin();
}

void	ioRenderElementAnimation::Stop()
{
	if ( m_curframe >= 0 && m_curframe < GetNumFrame() ) {
		m_frames[ m_curframe ]->OnEnd();
	}
	m_curframe = -1;
}

bool	ioRenderElementAnimation::IsPlaying() const
{
	return ( m_curframe >= 0 && m_curframe < GetNumFrame() );
}

//----------------------------------------------------------------------------------
/*
Frame
*/
const ioRenderElementAnimationFrame*		ioRenderElementAnimation::GetFrame( int index ) const
{
	if ( index < 0 || index >= GetNumFrame() ) {
		return 0;
	}
	return m_frames[ index ];
}

void	ioRenderElementAnimation::AppendFrame( ioRenderElementAnimationFrame* frame )
{
	m_frames.push_back( frame );
}

void	ioRenderElementAnimation::ClearFrames()
{
	FrameVector::iterator iter = m_frames.begin();
	FrameVector::iterator eiter = m_frames.end();
	for ( ; iter != eiter; ++iter ) {
		delete *iter;
	}
	m_frames.clear();
}