
//----------------------------------------------------------------------------------
/*
Filename	: wxioPropertyGrid.h
Author		: megamegy
Date		: 2013.10.22
Desc		: 
*/
//----------------------------------------------------------------------------------

#ifndef _IOPROPERTYGRID_H_
#define _IOPROPERTYGRID_H_

#include "wx/propgrid/propgrid.h"
#include "wx/propgrid/advprops.h"
#include <set>

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
wxioPropertyGrid
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
class wxioPropertyGrid : public wxPropertyGrid
{
	typedef std::set< std::string > StringSet;

public:

	//----------------------------------------------------------------------------------
	/*
	»ý¼º / ¼Ò¸ê
	*/
	wxioPropertyGrid( wxWindow *parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
					const wxSize& size = wxDefaultSize, long style = wxPG_DEFAULT_STYLE, const wxString& name = wxPropertyGridNameStr );
	virtual ~wxioPropertyGrid();

	//----------------------------------------------------------------------------------
	/*
	SetValue
	*/
	template < typename T >
	void		SetValue( const char* key, T v )
	{
		if ( !key ) {
			return;
		}

		StringSet::iterator iter = m_ignoreset.find( key );
		if ( iter != m_ignoreset.end() ) {
			return;
		}

		wxPGProperty* pg = GetProperty( key );
		if ( pg ) {
			pg->SetValue( v );
		}
	}
	void		SetColorValue( const char* key, DWORD v );

	//----------------------------------------------------------------------------------
	/*
	Set Ignore
	*/
	void		AddIgnoreSet( const char* ignore );
	void		ClearIgnoreSet();

protected:

	StringSet	m_ignoreset;
};


#endif // _IOPROPERTYGRID_H_