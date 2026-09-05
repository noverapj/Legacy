
//----------------------------------------------------------------------------------
/*
Filename	: PropertyControllerProject.cpp
Author		: megamegy
Date		: 2013.10.22
Desc		: 
*/
//----------------------------------------------------------------------------------

#include "stdafx.h"
#include "PropertyControllerProject.h"
#include "../ProjectManager/ProjectManager.h"

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
PropertyControllerProject
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
/*
Setup
*/
void	PropertyControllerProject::Setup( wxioPropertyGrid* grid )
{
	if ( !grid ) {
		return;
	}

	wxPGProperty* categoryWnd = grid->Append( new wxPropertyCategory( "HeaderFile" ) );
	{
		grid->AppendIn( categoryWnd, new wxStringProperty( "FileName" ) );
		grid->AppendIn( categoryWnd, new wxStringProperty( "ClassName" ) );
		grid->AppendIn( categoryWnd, new wxStringProperty( "BaseClassName" ) );
	}
	grid->SetPropertyBackgroundColour( categoryWnd, PropertyControllerioWnd::st_uniquebackcolor );

	//categoryWnd = grid->Append( new wxPropertyCategory( "Register" ) );
	//{
	//	grid->AppendIn( categoryWnd, new wxStringProperty( "GUIRegisterFilePath" ) );
	//	grid->AppendIn( categoryWnd, new wxStringProperty( "RegIncludeHeader" ) );
	//	grid->AppendIn( categoryWnd, new wxStringProperty( "RegClassName" ) );
	//}
	//grid->SetPropertyBackgroundColour( categoryWnd, PropertyControllerioWnd::st_backdefcolor );
}

//----------------------------------------------------------------------------------
/*
Sync
*/
void	PropertyControllerProject::Sync( wxioPropertyGrid* grid, ioWnd* wnd )
{
	grid->SetValue<const char*>( "FileName", ProjectManager::GetSingleton().GetCodeFilename() );
	grid->SetValue<const char*>( "ClassName", ProjectManager::GetSingleton().GetClassName() );
	grid->SetValue<const char*>( "BaseClassName", ProjectManager::GetSingleton().GetBaseClassName() );

	//grid->SetValue<const char*>( "GUIRegisterFilePath", ProjectManager::GetSingleton().GetGUIMgrRegisterFileName() );
	//grid->SetValue<const char*>( "RegIncludeHeader", ProjectManager::GetSingleton().GetGUIMgrRegisterHeaderFilePath() );
	//grid->SetValue<const char*>( "RegClassName", ProjectManager::GetSingleton().GetGUIMgrRegisterClassName() );
}

//----------------------------------------------------------------------------------
/*
OnChange
*/
void	PropertyControllerProject::OnChange( wxioPropertyGrid* grid, ioWnd* wnd, wxPropertyGridEvent& event )
{
	event.Skip();

	wxPGProperty* property = event.GetProperty();
	if ( !property ) {
		return;
	}

	wxString keyname = property->GetName();
	wxAny value = property->GetValue();

	if ( keyname == "FileName" ) {
		ProjectManager::GetSingleton().SetCodeFilename( event.GetValue().GetString().c_str() );
	} else if ( keyname == "ClassName" ) {
		ProjectManager::GetSingleton().SetClassName( event.GetValue().GetString().c_str() );
	} else if ( keyname == "BaseClassName" ) {
		ProjectManager::GetSingleton().SetBaseClassName( event.GetValue().GetString().c_str() );
	} else if ( keyname == "GUIRegisterFilePath" ) {
		ProjectManager::GetSingleton().SetGUIMgrRegisterFileName( event.GetValue().GetString().c_str() );
	} else if ( keyname == "RegIncludeHeader" ) {
		ProjectManager::GetSingleton().SetGUIMgrRegisterHeaderFilePath( event.GetValue().GetString().c_str() );
	} else if ( keyname == "RegClassName" ) {
		ProjectManager::GetSingleton().SetGUIMgrRegisterClassName( event.GetValue().GetString().c_str() );
	}
}