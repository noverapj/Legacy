
#include "StdAfx.h"
#include "ioSprite.h"

//////////////////////////////////////////////////////////////////////////
ioSprite::ioSprite()
{
	Init();
	m_vSpriteList.clear();

	m_dwCheckTime = 0;
	m_dwAniGapTime = 0;
	m_dwDuration = 0;

	m_iStartAlpha = MAX_ALPHA_RATE;
	m_iEndAlpha = MAX_ALPHA_RATE;

	m_fStartScale = FLOAT1;
	m_fEndScale = FLOAT1;
}

ioSprite::~ioSprite()
{
	Init();
	m_vSpriteList.clear();

	m_iCurAlpha = m_iStartAlpha;
	m_fCurScale = m_fStartScale;
}

void ioSprite::Init()
{
	m_State = SS_READY;
	m_iCurSprite = 0;
	
	m_dwAniStartTime = 0;
	m_dwAniEndTIme = 0;
}

void ioSprite::AddSpriteList( ioUIRenderImageList vList )
{
	int iSize = vList.size();
	for( int i=0; i<iSize; ++i )
	{
		ioUIRenderImage *pImage = vList[i];
		if( pImage )
		{
			m_vSpriteList.push_back( pImage );
		}
	}
	m_iMaxSpriteCnt = m_vSpriteList.size();
}

void ioSprite::DestroySpriteList()
{
	m_vSpriteList.clear();
	m_iMaxSpriteCnt = 0;
}

void ioSprite::SetType( AniType eType )
{
	m_AniType = eType;
}

void ioSprite::SetAnimationGapTime( DWORD dwGapTime )
{
	m_dwCheckTime = FRAMEGETTIME() + dwGapTime;
	m_dwAniGapTime = dwGapTime;
}

void ioSprite::SetAnimationDuration( DWORD dwDuration )
{
	m_dwDuration = dwDuration;
}

void ioSprite::SetAlphaAnimation( int iStartAlpha /* = MAX_ALPHA_RATE */, int iEndAlpha /* = MAX_ALPHA_RATE */ )
{
	m_iCurAlpha = iStartAlpha;
	m_iStartAlpha = iStartAlpha;
	m_iEndAlpha = iEndAlpha;
}

void ioSprite::SetScaleAnimation( float fStartScale /* = FLOAT1 */, float fEndScale /* = FLOAT1 */ )
{
	m_fCurScale = fStartScale;
	m_fStartScale = fStartScale;
	m_fEndScale = fEndScale;
}

void ioSprite::SetPlay( DWORD dwDelayTime /* = 0 */ )
{
	m_State = SS_READY;

	m_iCurSprite = 0;

	m_dwCheckTime = FRAMEGETTIME() + m_dwAniGapTime;
	m_dwAniStartTime = m_dwAniEndTIme = FRAMEGETTIME() + dwDelayTime;
	m_dwAniEndTIme += m_dwDuration;

	m_iCurAlpha = m_iStartAlpha;
	m_fCurScale = m_fStartScale;
}

bool ioSprite::IsStop()
{
	if( m_State == SS_STOP )
		return true;

	return false;
}

void ioSprite::Process()
{
	if( m_State == SS_READY && m_dwAniStartTime > 0 && m_dwAniStartTime <= FRAMEGETTIME() )
		m_State = SS_PLAY;
		
	if( m_State != SS_PLAY )
		return;

	if( m_vSpriteList.empty() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_AniType )
	{
	case AT_PLAY_ONCE:
		{
			if( m_dwCheckTime < dwCurTime )
			{
				m_dwCheckTime = dwCurTime + m_dwAniGapTime;
				m_iCurSprite++;
				if( m_iCurSprite >= m_iMaxSpriteCnt - 1 )
				{
					SetStop();
					return;
				}
			}
		}
		break;
	case AT_LOOP:
		{
			if( m_dwCheckTime < dwCurTime )
			{
				m_dwCheckTime = dwCurTime + m_dwAniGapTime;
				m_iCurSprite++;
				if( m_iCurSprite >= m_iMaxSpriteCnt )
					m_iCurSprite = 0;
			}

			if( m_dwDuration > 0 && m_dwAniEndTIme < dwCurTime )
			{
				SetStop();
				return;
			}
			else if( m_dwDuration > 0 )
			{
				float fRate = static_cast<float>( m_dwAniEndTIme - dwCurTime ) / m_dwDuration;

				if( m_iStartAlpha > m_iEndAlpha )
					m_iCurAlpha = MAX_ALPHA_RATE * fRate;
				else if( m_iStartAlpha < m_iEndAlpha )
					m_iCurAlpha = MAX_ALPHA_RATE * ( FLOAT1 - fRate );
				else
					m_iCurAlpha = m_iEndAlpha;

				if( m_fStartScale > m_fEndScale )
				{
					float fScaleGap = m_fStartScale - m_fEndScale;
					m_fCurScale = m_fStartScale - fScaleGap * ( FLOAT1 - fRate );
				}
				else if( m_fStartScale < m_fEndScale )
				{
					float fScaleGap = m_fEndScale - m_fStartScale;
					m_fCurScale = m_fStartScale + fScaleGap * ( FLOAT1 - fRate );
				}
				else
					m_fCurScale = m_fEndScale;
			}
		}
		break;
	}
}

void ioSprite::Render(int iXpos, int iYPos, float fScale /* = FLOAT1 */, UIRenderType eRenderType /* = UI_RENDER_NORMAL */, TextureFilterOption eFilterOption /* = TFO_NEAREST */)
{
	if( m_State != SS_PLAY )
		return;

	if( m_vSpriteList.empty() )
		return;

	if( (int)m_vSpriteList.size() > m_iCurSprite && m_vSpriteList[m_iCurSprite] /*&& m_vSpriteList[m_iCurSprite]->GetImage()*/ )
	{
		m_vSpriteList[m_iCurSprite]->SetAlpha( m_iCurAlpha );
		m_vSpriteList[m_iCurSprite]->SetScale( m_fCurScale * fScale );
		m_vSpriteList[m_iCurSprite]->Render( iXpos, iYPos, eRenderType, eFilterOption );
	}
}