#pragma once

class ioUIGuide : public Singleton< ioUIGuide >
{
private:
	bool m_bShow;
	ioUIRenderFrame *m_pGuideLine;

public:
	void LoadINIData();
	void Render();

private:
	void RenderClassName( ioWnd *pCurrWnd, int iXPos, int iYPos );
	//void RenderXMLName( ioWnd *pCurrWnd, int iXPos, int iYPos );

public:
	void UIGuideSwitch() { m_bShow = !m_bShow; }

public:
	ioUIGuide();
	virtual ~ioUIGuide();

public:
	static ioUIGuide& GetSingleton();
};
#define g_UIGuide ioUIGuide::GetSingleton()