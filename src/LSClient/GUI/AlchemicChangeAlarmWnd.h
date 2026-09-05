#pragma once

class AlchemicChangeAlarmWnd : public ioWnd
{
private:
	ioPlayStage *m_pPlayStage;
	ioHashString m_szTopText;
	ioHashString m_szBottomText[2];
	int m_iPeso;
	int m_iPieceCount;

public:
	enum
	{
		ID_EXIT = 1,
		ID_PRESENT = 2,
		ID_EXIT2 = 3,
	};		

public:
	virtual void ParseExtraInfo( ioXMLElement &xElement );
	virtual void OnRender();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void iwm_hide();
	virtual bool iwm_spacebar();

	void SetInfo( int iPeso, int iCount );
	void SetPlayStage( ioPlayStage *pPlayStage );

public:
	AlchemicChangeAlarmWnd();
	virtual ~AlchemicChangeAlarmWnd();
};