#pragma once

class ioWnd;

#define CHANGE_WND_ICON_SCALE 0.75f
class FirstIDChangeWnd : public ioWnd
{
public:
	enum
	{
		ID_NEWID_EDIT = 1,
		ID_CHANGE_BTN = 2,

		ID_EXIT_BTN   = 101,
	};
protected:
	ioUIRenderImage *m_pIcon;
	ioWnd        *m_pPreEdit;
	ioHashString  m_szNewID;
	ioHashString  m_szStartEditText;
	ioHashString  m_szErrorEditText;
	DWORD         m_dwSendTime;

public:
	virtual void iwm_command( ioWnd *pWnd , int cmd, DWORD param );
	virtual void iwm_show();	
	virtual void iwm_hide();

protected:
	virtual void OnRender();
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

protected:
	void CheckNewID();
	void SendNewID();
	void ChangeWnd();
	void MakeNewSaveFile();

	void RenderDefault( int iXPos, int iYPos );
	void RenderChanged( int iXPos, int iYPos );

	void SetExit();

	bool IsRightFirstID( const char *szID );

public:
	void ErrorEdit( const char *szError );

public:
	FirstIDChangeWnd(void);
	virtual ~FirstIDChangeWnd(void);
};

