#pragma once

class ioUI3DRender;
class ioCreateChar;

class ioLSMouse : public ioMouse
{
protected:
	ioUIRenderImage *m_pNormalCursor;
	ioUIRenderImage *m_pToolTipCursor;
	ioUIRenderImage *m_pHousingCameraCursor;
	ioUIRenderImage *m_pDragImg;
	ioUIRenderImage *m_pSubImg;

	ioUIRenderFrame * m_pDragRenderFrame;

	ioUI3DRender    *m_pUI3DRender;
	ioCreateChar    *m_pUIChar;

	ioHashString m_szToolTipID;
	int          m_iToolTipType; // TT_ROOM ~ class Type
	DWORD        m_dwToolTipWndID;
	
	ioUIRenderImage *m_pCursor;

	bool	m_bMouseShowing;
	bool	m_bTraining;

public:
	void InitResource();
	void ReleaseResource();

public:
	void Render();
	void ClearMouseShowingState();

protected:
	void RenderDragImage();
	void RenderDragCharacter();
	void RenderDragUIFrame();

public:
	bool IsDragDropping() const;
	bool IsMouseShowing() const { return m_bMouseShowing; }

	bool IsMouseBtnAction() const;

public:
	virtual void SetDragDropStart( ioDragItem *pDragItem, const POINT &ptOffset );

	virtual void SetPacketID( DWORD dwPacketID );
public:
	void SetToolTipID( const ioHashString &szToolTipID, int iType );
	void SetToolTipWndID(DWORD dwToolTipWndID) { m_dwToolTipWndID = dwToolTipWndID; }

	void GetToolTipID( OUT ioHashString &rsToolTipID, OUT int &riType );
	DWORD GetToolTipWndID() const { return m_dwToolTipWndID; }

	void SetToolTipCursor();
	void SetHousingCameraCursor();
	void RestoreCursor() { m_pCursor = m_pNormalCursor; }
	void SetTraining( bool b )	{ m_bTraining = b; }
	bool GetTraining()			{ return m_bTraining; }

public:
	ioLSMouse();
	virtual ~ioLSMouse();
};

