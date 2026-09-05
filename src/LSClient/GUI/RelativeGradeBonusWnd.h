#pragma once

class SP2Packet;
class ioPlayStage;
class ioUIRenderImage;

class RelativeGradeBonusWnd : public ioWnd
{
public:
	enum
	{
		GRADE_LEVEL_0 = 0,	// 준장
		GRADE_LEVEL_1 = 1,	// 소장
		GRADE_LEVEL_2 = 2,	// 중장
		GRADE_LEVEL_3 = 3,	// 대장
		GRADE_LEVEL_4 = 4,	// 원수
		GRADE_LEVEL_MAX = 5,
	};

	enum
	{
		ID_EXIT  = 1,
		ID_CLOSE = 2,
	};

protected:
	ioUIRenderImage *m_pIconBack;
	ioUIRenderImage *m_pInfoBack;
	ioUIRenderImage *m_pRankingIcon;
	ioUIRenderImage *m_pGradeText[5];
	ioUIRenderImage *m_pGradeIcon[5];
	ioUIRenderImage *m_pRewardIcon[2];
	ioUIRenderImage *m_pSubIcon[2];

protected:
	int m_iGradeType;
	ioHashString m_szReward[2];
	ioHashString m_szLastExp;

public:
	virtual void iwm_show();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void OnRender();
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

public:
	void SetInfoToPacket( SP2Packet &rkPacket );

public:
	RelativeGradeBonusWnd();
	virtual ~RelativeGradeBonusWnd();
};
