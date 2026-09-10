#pragma once

#include "../io3DEngine/ioStringConverter.h"

#include "QuestVariety.h"

class ioUIRenderFrame;
class ioUIRenderImage;
class ioQuestGuide : public Singleton< ioQuestGuide >
{
protected:
	ioUIRenderFrame *m_pGuideFrm;
	ioUIRenderImage *m_pGuideFrmTail;
	ioUIRenderImage *m_pQuestIconBack;
	ioUIRenderImage *m_pQuestInfoBtn;
	ioUIRenderImage *m_pCostumeMark;

protected:
	struct GuideElement
	{
		StringVector m_Element;
	};
	typedef std::vector< GuideElement > vGuideElement;

	struct GuideData
	{
		QuestParent   *m_pQuestParent;
		vGuideElement  m_ElementList;
		ioWnd         *m_pGuideWnd;

		bool  m_bRender;
		DWORD m_dwWndID;
		float m_fViewXPos;
		float m_fViewYPos;
		float m_fArrivalXPos;
		float m_fArrivalYPos;
		DWORD m_dwCurrentTime;
		bool  m_bInfoScreen;
		GuideData()
		{
			m_pQuestParent = NULL;
			m_pGuideWnd    = NULL;
			m_bRender	= false;
			m_dwWndID   = 0;
			m_fViewXPos = -999.0f;
			m_fViewYPos = -999.0f;
			m_fArrivalXPos = 0;
			m_fArrivalYPos = 0;
			m_dwCurrentTime= 0;
			m_bInfoScreen  = false;
		}
	};
	typedef std::vector< GuideData > vGuideData;
	vGuideData m_GuideList;

protected:
	bool m_bMouseOverView;

protected:
	void ClearResource();

public:
	void LoadINIData();
	void CreateGuideWnd();

public:
	void RotateMouseView(){ m_bMouseOverView = !m_bMouseOverView; }

protected:
	double GetAngle( int iStartX, int iStartY, int iEndX, int iEndY );

protected:
	bool ParseGuide( const GuideElement &rkElement, float &rfXPos, float &rfYPos, DWORD &rWndID );
	bool ParseGuideFun( const GuideElement &rkElement, float &rfXPos, float &rfYPos, DWORD &rWndID );

protected:
	bool ParseManual( const GuideElement &rkElement, float &rfXPos, float &rfYPos, DWORD &rWndID );
	bool ParseShop( const GuideElement &rkElement, float &rfXPos, float &rfYPos, DWORD &rWndID );
	bool ParseMyItem( const GuideElement &rkElement, float &rfXPos, float &rfYPos, DWORD &rWndID );
	bool ParsePractice( const GuideElement &rkElement, float &rfXPos, float &rfYPos, DWORD &rWndID );
	bool ParseQuickStart( const GuideElement &rkElement, float &rfXPos, float &rfYPos, DWORD &rWndID );
	bool ParseBattleSelect( const GuideElement &rkElement, float &rfXPos, float &rfYPos, DWORD &rWndID );
	bool ParseBattleList( const GuideElement &rkElement, float &rfXPos, float &rfYPos, DWORD &rWndID );
	bool ParseBattleCreate( const GuideElement &rkElement, float &rfXPos, float &rfYPos, DWORD &rWndID );
	bool ParsePlazaCreate( const GuideElement &rkElement, float &rfXPos, float &rfYPos, DWORD &rWndID );
	bool ParseCampCreate( const GuideElement &rkElement, float &rfXPos, float &rfYPos, DWORD &rWndID );
	bool ParseBottomBar( const GuideElement &rkElement, float &rfXPos, float &rfYPos, DWORD &rWndID );

public:
	void ProcessGuideMove();
	void ProcessGuide();
	void ProcessGuideWnd();
	void RenderGuide( DWORD dwWndID );

protected:
	void RenderGuide( const GuideData &rkData );
	void RenderQuestTitle( int iXPos, int iYPos, QuestParent *pQuestParent, const char *szResult );
	void RenderUI();

public:
	static ioQuestGuide& GetSingleton();

public:   
	ioQuestGuide();
	virtual ~ioQuestGuide();
};
#define g_QuestGuide ioQuestGuide::GetSingleton()
