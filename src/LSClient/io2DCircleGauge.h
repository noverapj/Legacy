#pragma once

class ioMeshData;
class ioTexture;
class ioRenderSystem;

class io2DCircleGauge
{
public:
	enum StartType
	{
		ST_6OCLOCK  = 0,
		ST_12OCLOCK = 1,
	};
protected:
	ioMeshData *m_pGaugeData;
	ioTexture  *m_pGaugeTex;

	int m_iRenderX;
	int m_iRenderY;

	int m_iMaxCount;
	int m_iFrontSkip;
	int m_iBackSkip;
	int m_iCurRenderCount;

	StartType m_eStartType;

public:
	// 게이지가 완전원형이 아닌 아래쪽이 뚤린형태인데 이 뚤린곳의 범위를 iFrontSkip ~ iBackSkip만큼 제외함
	void InitResource( const char *szTexFileName,	// 텍스쳐파일이름
					   int iDivideCount,			// 최대 몇등분
					   int iFrontSkip = 0,			// 앞에서 몇개를 제외할건지
					   int iBackSkip = 0,           // 뒤에서 몇개를 제외할건지
					   StartType eType = ST_6OCLOCK );			

public:
	bool IsNeedRender() const;
	void UpdateGauge( int iXPos, int iYPos, float fRate );	// 출력 xPos, yPos, 
	void Render( ioRenderSystem *pSystem );

public:
	io2DCircleGauge();
	virtual ~io2DCircleGauge();
};

