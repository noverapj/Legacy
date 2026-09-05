
//----------------------------------------------------------------------------------
/*
Filename	: wxioPropertyGrid.cpp
Author		: megamegy
Date		: 2013.10.22
Desc		: 
*/
//----------------------------------------------------------------------------------

#include "stdafx.h"
#include "wxioPropertyGrid.h"
#include "../Utils/GlobalRef.h"

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
wxioPropertyGrid
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
/*
»ý¼º / ¼Ò¸ê
*/
wxioPropertyGrid::wxioPropertyGrid( wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name )
	: wxPropertyGrid( parent, id, pos, size, style, name )
{
}

wxioPropertyGrid::~wxioPropertyGrid()
{
}

//----------------------------------------------------------------------------------
/*
SetValue
*/
void	wxioPropertyGrid::SetColorValue( const char* key, DWORD v )
{
	if ( !key ) {
		return;
	}

	StringSet::iterator iter = m_ignoreset.find( key );
	if ( iter != m_ignoreset.end() ) {
		return;
	}

	wxColourProperty* pg = dynamic_cast<wxColourProperty*>( GetProperty( key ) );
	if ( pg ) 
	{
		wxColour colour( v );
		wxVariant variant;
		variant << colour;
		pg->SetValue( variant );
	}
}

//----------------------------------------------------------------------------------
/*
Set Ignore
*/
void	wxioPropertyGrid::AddIgnoreSet( const char* ignore )
{
	if ( ignore ) {
		m_ignoreset.insert( std::string( ignore ) );
	}
}

void	wxioPropertyGrid::ClearIgnoreSet()
{
	m_ignoreset.clear();
}