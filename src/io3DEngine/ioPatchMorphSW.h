

#ifndef _ioPatchMorphSW_h_
#define _ioPatchMorphSW_h_

class ioPatch;
class ioLandScape;

class __EX ioPatchMorphSW : public ioPatch
{
protected:
	D3DXVECTOR3	*m_pVertexLow;

	float	*m_pYMoveSelf;
	float	*m_pYMoveLeft, *m_pYMoveLeft2;
	float	*m_pYMoveRight, *m_pYMoveRight2;
	float	*m_pYMoveBottom, *m_pYMoveBottom2;
	float	*m_pYMoveTop, *m_pYMoveTop2;

	bool	m_bNewFollowsLeft, m_bFollowsLeft;
	bool	m_bNewFollowsRight, m_bFollowsRight;
	bool	m_bNewFollowsBottom, m_bFollowsBottom;
	bool	m_bNewFollowsTop, m_bFollowsTop;

	bool	m_bRecalcBorderLeft;
	bool	m_bRecalcBorderRight;
	bool	m_bRecalcBorderBottom;
	bool	m_bRecalcBorderTop;

	bool	m_bForceReTessellation;

public:
	virtual void Init();
	virtual void DeInit();
	virtual bool UpdateLevel();
	virtual bool UpdateLevelSecond();
	virtual bool UpdateLevelThird();

protected:
	void FillMorphVertex();	
	void CheckBorderLeft();
	void CheckBorderRight();
	void CheckBorderBottom();
	void CheckBorderTop();
	virtual int FillVertices( D3DXVECTOR3 *pBuf );

public:
	ioPatchMorphSW( ioLandScape *pLand, int nX, int nY );
	virtual ~ioPatchMorphSW();
};

#endif