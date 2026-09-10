#pragma once

class PCRoomLocalBenefitWnd : public ioWnd
{
public:
	enum
	{
		ID_EXIT  = 1,
		ID_OK = 2,
	};

	enum
	{
		MAX_TEXT_LINE = 12,
	};

protected:
	ioUIRenderImage *m_pMarkGold;
	ioUIRenderImage *m_pMarkSilver;
	ioHashStringVec  m_vText;

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void iwm_show();
	virtual bool iwm_esc();

public:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void ParseExtraInfo( ioXMLElement &xElement );

protected:
	virtual void OnRender();

public:
	PCRoomLocalBenefitWnd(void);
	virtual ~PCRoomLocalBenefitWnd(void);
};
