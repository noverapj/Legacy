
//----------------------------------------------------------------------------------
/*
Filename	: wxRichLabelEditor.h
Author		: megamegy
Date		: 2013.10.14
Desc		: 
*/
//----------------------------------------------------------------------------------

#ifndef _WXRICHLABELEDITOR_H_
#define _WXRICHLABELEDITOR_H_

#include "../wxMainFrame.h"
#include <vector>

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
wxRichLabelEditor
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
class wxRichLabelEditor : public RichLabelEditor
{
public:

	//----------------------------------------------------------------------------------
	/*
	생성 / 소멸
	*/
	wxRichLabelEditor( wxWindow* parent );
	virtual ~wxRichLabelEditor();

	//----------------------------------------------------------------------------------
	/*
	RichLabelEditor override
	*/
	virtual void	OnClickBlock( wxCommandEvent& event );
	virtual void	OnAdd( wxCommandEvent& event );
	virtual void	OnRemove( wxCommandEvent& event );

	//----------------------------------------------------------------------------------
	/*
	Callback
	*/
	void			OnPropertyGridChanged( wxPropertyGridEvent& event );

	//----------------------------------------------------------------------------------
	/*
	Control
	*/
	void			SetupGrid();
	void			SyncTextBlockList();
	void			SyncTextBlock();

protected:

	//----------------------------------------------------------------------------------
	/*
	기타
	*/
	wxString		GetNewText();
};

#endif // _WXRICHLABELEDITOR_H_