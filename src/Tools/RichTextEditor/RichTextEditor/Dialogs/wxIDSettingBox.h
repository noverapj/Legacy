
//----------------------------------------------------------------------------------
/*
Filename	: wxIDSettingBox.h
Author		: megamegy
Date		: 2013.10.14
Desc		: 
*/
//----------------------------------------------------------------------------------

#ifndef _WXIDSETTINGBOX_H_
#define _WXIDSETTINGBOX_H_

#include "../wxMainFrame.h"
#include <vector>

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
wxIDSettingBox
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
class wxIDSettingBox : public IDSettingBox
{
public:

	typedef std::vector< DWORD > WndIDVec;

public:

	//----------------------------------------------------------------------------------
	/*
	생성 / 소멸
	*/
	wxIDSettingBox( wxWindow* parent, const WndIDVec& ids );
	virtual ~wxIDSettingBox();

	//----------------------------------------------------------------------------------
	/*
	IDSettingBox override
	*/
	virtual void	OnClickUp( wxCommandEvent& event );
	virtual void	OnClickDown( wxCommandEvent& event );
	virtual void	OnClickSubmit( wxCommandEvent& event );
	virtual void	OnClickCancel( wxCommandEvent& event );
	virtual void	OnSelectWnd( wxCommandEvent& event );

protected:

	//----------------------------------------------------------------------------------
	/*
	기타
	*/
	void			RefreshList();

protected:

	WndIDVec		m_settingdata;
};

#endif // _WXIDSETTINGBOX_H_