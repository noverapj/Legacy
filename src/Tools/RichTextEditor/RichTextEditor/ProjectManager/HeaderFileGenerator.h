//-------------------------------------------------------------------
//-------------------------------------------------------------------
// 작성자	: cha
// 날짜		: 2013.10.24
// 이름		: HeaderFileGenerator
//-------------------------------------------------------------------
//-------------------------------------------------------------------

#ifndef __HEADERFILEGENERATOR_H__
#define __HEADERFILEGENERATOR_H__

#include "wx/wx.h"
#include <vector>
#include <map>
#include <set>

//-------------------------------------------------------------
/*
HeaderFileGenerator
*/
#define COMPARE_STRING_EQUAL( member ) ( strcmp( lhs.member.c_str(), rhs.member.c_str() ) == 0 )
#define COMPARE_STRING_LESS( member ) ( strcmp( lhs.member.c_str(), rhs.member.c_str() ) < 0 )

class HeaderFileGenerator
{
public:
	
	// 이벤트 함수 정보
	struct EventFuncInfo {
		EventFuncInfo( const wxString& fn, int evttype ) : m_functionname( fn ), m_evttype( evttype ) {}
		wxString m_functionname;
		int	m_evttype;
	};
	struct EventFuncInfoSetOperator {
		bool operator()( const EventFuncInfo& lhs, const EventFuncInfo& rhs ) const {
			if ( COMPARE_STRING_LESS( m_functionname ) ) return true;
			return ( COMPARE_STRING_EQUAL( m_functionname ) && ( lhs.m_evttype < rhs.m_evttype ) );
		}
	};
	typedef std::set< EventFuncInfo, EventFuncInfoSetOperator >	EventFuncInfoSet;

	// 이벤트 함수 연결 정보
	struct EventFuncConnectInfo : public EventFuncInfo {
		EventFuncConnectInfo( const wxString& fn, int evttype, int id ) : EventFuncInfo( fn, evttype ), m_id( id ) {}
		int	m_id;
	};
	struct EventFuncConnectInfoSetOperator {
		bool operator()( const EventFuncConnectInfo& lhs, const EventFuncConnectInfo& rhs ) const {
			if ( COMPARE_STRING_LESS( m_functionname ) ) return true;
			if ( COMPARE_STRING_EQUAL( m_functionname ) && ( lhs.m_evttype < rhs.m_evttype ) ) return true;
			return ( COMPARE_STRING_EQUAL( m_functionname ) && ( lhs.m_evttype == rhs.m_evttype ) && ( lhs.m_id < rhs.m_id ) );
		}
	};
	typedef std::set< EventFuncConnectInfo, EventFuncConnectInfoSetOperator > EventFuncConnectInfoSet;

	// 멤버 대입 정보
	struct MemeberInfo {
		MemeberInfo( int id, const wxString& cn, const wxString& mn, const wxString& hf ) : m_id( id ), m_classname( cn ), m_membername( mn ), m_headerfilename( hf ) {}
		int m_id;
		wxString m_classname;
		wxString m_membername;
		wxString m_headerfilename;
	};
	struct MemeberInfoSetOperator {
		bool operator()( const MemeberInfo& lhs, const MemeberInfo& rhs ) const {
			if ( lhs.m_id < rhs.m_id ) return true;
			if ( ( lhs.m_id == rhs.m_id ) && COMPARE_STRING_LESS( m_classname ) ) return true;
			if ( ( lhs.m_id == rhs.m_id ) && COMPARE_STRING_EQUAL( m_classname ) && COMPARE_STRING_LESS( m_membername ) ) return true;
			return ( ( lhs.m_id == rhs.m_id ) && COMPARE_STRING_EQUAL( m_classname ) && COMPARE_STRING_EQUAL( m_membername ) && COMPARE_STRING_LESS( m_headerfilename ) );
		}
	};
	typedef std::set< MemeberInfo, MemeberInfoSetOperator > MemeberInfoSet;

public:

	//-------------------------------------------------------------
	/*
	생성,파괴
	*/
	HeaderFileGenerator();
	~HeaderFileGenerator();

	//-------------------------------------------------------------
	/*
	Load / Save
	*/
	static bool						LoadFrom( const char* filename, wxString& buff );
	bool							Load( const char* filename );
	bool							Save( const char* filename );

	//-------------------------------------------------------------
	/*
	Control
	*/
	void							SetFilename( const char* filename ) { if ( filename ) m_filename = filename; }
	void							SetDate( const char* date ) { if ( date ) m_date = date; }
	void							SetDefine( const char* define ) { if ( define ) m_define = define; }
	void							SetClassName( const char* classname ) { if ( classname ) m_classname = classname; }
	void							SetBaseClassName( const char* classname ) { if ( classname ) m_baseclassname = classname; }	
	void							AddIncludeFile( const char* filename ) { if ( filename ) m_includes.push_back( filename ); }
	void							AddWndID( const char* strid, int nid ) { if ( strid ) m_ids.insert( std::map< wxString, int >::value_type( strid, nid ) ); }
	void							SetWndIDInfo( const std::map< wxString, int >& ids ) { m_ids = ids; }
	void							SetEventFunctionInfo( const EventFuncInfoSet& info ) { m_eventfunctions = info; }
	void							SetEventFunctionConnectInfo( const EventFuncConnectInfoSet& info ) { m_eventfunctionconnects = info; }
	void							SetMemberInfo( const MemeberInfoSet& info ) { m_memberinfos = info; }

protected:

	wxString						m_buffer;
	wxString						m_filename;
	wxString						m_date;
	wxString						m_define;
	wxString						m_classname;
	wxString						m_baseclassname;
	std::vector< wxString >			m_includes;
	std::map< wxString, int >		m_ids;
	EventFuncInfoSet				m_eventfunctions;
	EventFuncConnectInfoSet			m_eventfunctionconnects;
	MemeberInfoSet					m_memberinfos;
};

#endif //__HEADERFILEGENERATOR_H__
