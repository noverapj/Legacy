//-------------------------------------------------------------------
//-------------------------------------------------------------------
// 작성자	: cha
// 날짜		: 2013.10.24
// 이름		: HeaderFileGenerator
//-------------------------------------------------------------------
//-------------------------------------------------------------------

#include "stdafx.h"
#include "HeaderFileGenerator.h"
#include <fstream>
#include "ioWndEXEventType.h"
#include <set>
#include "ProjectManager.h"

//-------------------------------------------------------------
/*
생성
*/
HeaderFileGenerator::HeaderFileGenerator()
	: m_buffer( wxEmptyString )
	, m_filename( wxEmptyString )
	, m_date( wxEmptyString )
	, m_define( wxEmptyString )
	, m_classname( wxEmptyString )
	, m_baseclassname( wxEmptyString )
{
	m_includes.clear();
}

//-------------------------------------------------------------
/*
파괴
*/
HeaderFileGenerator::~HeaderFileGenerator()
{
}

//-------------------------------------------------------------
/*
Load / Save
*/
bool	HeaderFileGenerator::LoadFrom( const char* filename, wxString& buff )
{
	if ( !filename ) {
		return false;
	}

	std::string tmp;
	std::string buffer;
	std::ifstream stream( filename );
	
	while ( stream && getline( stream, tmp ) )
	{
		buffer += tmp;
		buffer += "\n";
	}
	stream.close();

	if ( buffer.empty() ) {
		return false;
	}

	buff = buffer.c_str();
	return true;
}

bool	HeaderFileGenerator::Load( const char* filename )
{
	return LoadFrom( filename, m_buffer );
}

void	ReplaceBuffer( wxString& buffer, const wxString& line, const wxString& pattern, const wxString& def = wxEmptyString )
{
	if ( line != wxEmptyString ) {
		buffer.Replace( pattern, line );
	} else {
		buffer.Replace( pattern, "" );
	}
}

void	ReplaceBuffer( wxString& buffer, std::set< wxString >& lines, const wxString& pattern )
{
	wxString eventbuffer( wxEmptyString );

	std::set< wxString >::iterator iter = lines.begin();
	std::set< wxString >::iterator eiter = lines.end();
	for ( ; iter != eiter; ++iter ) {
		eventbuffer += *iter;
	}

	if ( eventbuffer != wxEmptyString ) {
		buffer.Replace( pattern, eventbuffer );
	} else {
		buffer.Replace( pattern, "" );
	}
}

bool	HeaderFileGenerator::Save( const char* filename )
{
	if ( !filename ) {
		return false;
	}
	
	std::ofstream stream( filename );
	if ( !stream ) {
		return false;
	}
	
	ReplaceBuffer( m_buffer, m_filename, "%FILENAME%" );
	ReplaceBuffer( m_buffer, m_date, "%DATE%" );
	ReplaceBuffer( m_buffer, m_define, "%DEFINE%" );
	ReplaceBuffer( m_buffer, m_classname, "%CLASSSNAME%" );
	ReplaceBuffer( m_buffer, m_baseclassname, "%BASECLASSSNAME%" );

	{
		wxString idbuffer( wxEmptyString );	
		std::map< wxString, int >::iterator iter = m_ids.begin();
		std::map< wxString, int >::iterator eiter = m_ids.end();
		for ( ; iter != eiter; ++iter )
		{
			if ( iter->first == wxEmptyString || iter->first == "" ) {
				continue;
			}
			idbuffer += wxString::Format( "\t\t%s = %d,\n", iter->first.c_str(), iter->second );
		}
		ReplaceBuffer( m_buffer, idbuffer, "%ID%" );
	}

	{
		wxString includebuffer( wxEmptyString );
		std::vector< wxString >::iterator iter = m_includes.begin();
		std::vector< wxString >::iterator eiter = m_includes.end();
		for ( ; iter != eiter; ++iter ) {
			includebuffer += wxString::Format( "#include \"%s\"\n", (*iter).c_str() );
		}
		ReplaceBuffer( m_buffer, includebuffer, "%INCLUDE%" );
	}

	{
		std::set< wxString > functions;
		EventFuncInfoSet::iterator iter = m_eventfunctions.begin();
		EventFuncInfoSet::iterator eiter = m_eventfunctions.end();
		for ( ; iter != eiter; ++iter )
		{
			wxString func( wxEmptyString );
			switch ( iter->m_evttype )
			{
			case WEVT_LBUTTONDOWN:
			case WEVT_LBUTTONUP:
			case WEVT_LBUTTONDBCLICK:
			case WEVT_RBUTTONDOWN:
			case WEVT_RBUTTONUP:
			case WEVT_MOUSEOVER:
			case WEVT_MOUSEMOVE:
			case WEVT_MOUSELEAVE:
			case WEVT_WHEEL:
				functions.insert( wxString::Format( "\tvirtual void ""%s""( ioWndEX* wnd, ioWndEXMouseEvent& event ) {}\n", iter->m_functionname.c_str() ) );
				break;

			case WEVT_RADIOCHECK:
			case WEVT_TABCHECK:
				functions.insert( wxString::Format( "\tvirtual void ""%s""( ioWndEX* wnd, ioWndEXRadioEvent& event ) {}\n", iter->m_functionname.c_str() ) );
				break;

			case WEVT_SB_CLICK_DECREASE:
			case WEVT_SB_CLICK_INCREASE:
			case WEVT_SB_SCROLL:
				functions.insert( wxString::Format( "\tvirtual void ""%s""( ioWndEX* wnd, ioWndEXScrollBarEvent& event ) {}\n", iter->m_functionname.c_str() ) );
				break;

			case WEVT_CREATE:
			case WEVT_DESTROY:
			case WEVT_SHOW:
			case WEVT_HIDE:
			case WEVT_ESC:
			case WEVT_SPACEBAR:
			default:
				functions.insert( wxString::Format( "\tvirtual void ""%s""( ioWndEX* wnd, ioWndEXEvent& event ) {}\n", iter->m_functionname.c_str() ) );
				break;
			}
		}
		ReplaceBuffer( m_buffer, functions, "%EVENTFUNCTION%" );
	}

	{
		std::set< wxString > connect;
		std::set< wxString > disconnect;
		EventFuncConnectInfoSet::iterator iter = m_eventfunctionconnects.begin();
		EventFuncConnectInfoSet::iterator eiter = m_eventfunctionconnects.end();
		for ( ; iter != eiter; ++iter )
		{
			static const char* connectformat =
			{
				"\t\t{\n\
				ioWndEX* child = dynamic_cast<ioWndEX*>( FindChildWndEX(%d) );\n\
				if ( child ) {\n\
				\tchild->Connect( static_cast<WNDEX_EVENT_TYPE>(%d), %s );\n\
				}\n\t\t}\n"
			};
			static const char* disconnectformat =
			{
				"\t\t{\n\
				ioWndEX* child = dynamic_cast<ioWndEX*>( FindChildWndEX(%d) );\n\
				if ( child ) {\n\
				\tchild->Disconnect( static_cast<WNDEX_EVENT_TYPE>(%d), this );\n\
				}\n\t\t}\n"
			};
			wxString func( wxEmptyString );
			switch ( iter->m_evttype )
			{
			case WEVT_LBUTTONDOWN:
			case WEVT_LBUTTONUP:
			case WEVT_LBUTTONDBCLICK:
			case WEVT_RBUTTONDOWN:
			case WEVT_RBUTTONUP:
			case WEVT_MOUSEOVER:
			case WEVT_MOUSEMOVE:				
			case WEVT_MOUSELEAVE:
			case WEVT_WHEEL:
				connect.insert( wxString::Format( connectformat, iter->m_id, iter->m_evttype,
											wxString::Format( "new ioWndEXMouseEventHandlerEX< %s >( this, &%s::%s )",
															m_classname.c_str(), m_classname.c_str(), iter->m_functionname.c_str() ).c_str() )
								);
				break;

			case WEVT_RADIOCHECK:
			case WEVT_TABCHECK:
				connect.insert( wxString::Format( connectformat, iter->m_id, iter->m_evttype,
											wxString::Format( "new ioWndEXRadioEventHandlerEX< %s >( this, &%s::%s )",
															m_classname.c_str(), m_classname.c_str(), iter->m_functionname.c_str() ).c_str() )
								);
				break;

			case WEVT_SB_CLICK_DECREASE:
			case WEVT_SB_CLICK_INCREASE:
			case WEVT_SB_SCROLL:
				connect.insert( wxString::Format( connectformat, iter->m_id, iter->m_evttype,
											wxString::Format( "new ioWndEXScrollBarEventHandlerEX< %s >( this, &%s::%s )",
															m_classname.c_str(), m_classname.c_str(), iter->m_functionname.c_str() ).c_str() )
								);
				break;

			case WEVT_CREATE:
			case WEVT_DESTROY:
			case WEVT_SHOW:
			case WEVT_HIDE:
			case WEVT_ESC:
			case WEVT_SPACEBAR:
			default:
				connect.insert( wxString::Format( connectformat, iter->m_id, iter->m_evttype,
											wxString::Format( "new ioWndEXEventHandlerEX< %s, ioWndEXEvent >( this, &%s::%s )",
															m_classname.c_str(), m_classname.c_str(), iter->m_functionname.c_str() ).c_str() )
								);				
				break;
			}

			disconnect.insert( wxString::Format( disconnectformat, iter->m_id, iter->m_evttype ) );
		}
		ReplaceBuffer( m_buffer, connect, "%EVENTCONNECT%" );
		ReplaceBuffer( m_buffer, disconnect, "%EVENTDISCONNECT%" );
	}

	{
		struct ArrayMemberInfo 
		{
			ArrayMemberInfo( const wxString& cname, int num ) : m_classname( cname ), m_num( num ) {}
			wxString	m_classname;
			int			m_num;
		};
		std::map< wxString, ArrayMemberInfo > memberarray;
		std::set< wxString > memberinclude;
		std::set< wxString > memberdeclare;
		std::set< wxString > memberinit;
		std::set< wxString > memberconnect;
		MemeberInfoSet::iterator iter = m_memberinfos.begin();
		MemeberInfoSet::iterator eiter = m_memberinfos.end();
		for ( ; iter != eiter; ++iter )
		{
			if ( iter->m_membername == wxEmptyString ) {
				continue;
			}

			size_t arymarkpos = iter->m_membername.find( "[" );
			size_t memnamelen = iter->m_membername.size();
			if ( ( arymarkpos < memnamelen ) || ( iter->m_membername.find( "]" ) < memnamelen ) )
			{
				wxString memname = iter->m_membername.substr( 0, arymarkpos );
				int idx = atoi( iter->m_membername.substr( arymarkpos + 1, memnamelen - arymarkpos + 1 ).c_str() );

				std::map< wxString, ArrayMemberInfo >::iterator aryiter = memberarray.find( memname );
				if ( aryiter == memberarray.end() ) {
					memberarray.insert( std::map< wxString, ArrayMemberInfo >::value_type( memname, ArrayMemberInfo( iter->m_classname, idx ) ) );
				} else {
					aryiter->second.m_num = std::max<int>( aryiter->second.m_num, idx );
				}
			}
			else
			{
				memberdeclare.insert( wxString::Format( "\t%s* %s;\n", iter->m_classname.c_str(), iter->m_membername.c_str() ) );
			}

			memberinclude.insert( wxString::Format( "#include \"%s.h\"\n", GetFileNameOnly( iter->m_headerfilename.c_str() ).c_str() ) );
			memberinit.insert( wxString::Format( "\t\t%s = 0;\n", iter->m_membername.c_str() ) );
			memberconnect.insert( wxString::Format( "\t\t%s = dynamic_cast<%s*>(FindChildWndEX(%d));\n", iter->m_membername.c_str(), iter->m_classname.c_str(), iter->m_id ) );
		}

		std::map< wxString, ArrayMemberInfo >::iterator aiter = memberarray.begin();
		std::map< wxString, ArrayMemberInfo >::iterator eaiter = memberarray.end();
		for ( ; aiter != eaiter; ++aiter ) {
			memberdeclare.insert( wxString::Format( "\t%s* %s[%d];\n", aiter->second.m_classname.c_str(), aiter->first.c_str(), aiter->second.m_num + 1 ) );
		}

		ReplaceBuffer( m_buffer, memberinclude, "%MEMBERINCLUDE%" );
		ReplaceBuffer( m_buffer, memberdeclare, "%MEMBERDECLARE%" );
		ReplaceBuffer( m_buffer, memberinit, "%MEMBERINIT%" );
		ReplaceBuffer( m_buffer, memberconnect, "%MEMBERCONNECT%" );
	}

	stream << m_buffer.c_str() << std::endl;

	stream.close();
	return true;
}