#include "stdafx.h"
#include "ioAIBrain.h"
#include "ioAISymbolTable.h"
#include "ioAIDatabase.h"
#include "ioChampionAI.h"

#include <regex>

#include "../io3DEngine/ioXMLElement.h"
#include "stringext.h"

//#define _DEBUG_BvT_INOUT
//#define _DEBUG_BvT_EVAC
//#define _DEBUG_BvT_COMMENT

void PrintXMLLogForBvT( ioXMLElement* node, int depth )
{
	std::string ts;
	for( int i = 0; i < depth; i++ )
		ts += "\t";

	LOG.PrintTimeAndLog( 0, "%s> New Behavior Tree Node has been on depth %d: ", ts.c_str(), depth );
	LOG.PrintTimeAndLog( 0, "%s\ttag name = %s", ts.c_str(), node->GetTagName() );
	LOG.PrintTimeAndLog( 0, "%s\tname = %s", ts.c_str(), node->GetStringAttribute_e("name") );
	LOG.PrintTimeAndLog( 0, "%s\thalt = %s", ts.c_str(), node->GetStringAttribute_e("halt") );
	LOG.PrintTimeAndLog( 0, "%s\tuse = %s", ts.c_str(), node->GetStringAttribute("use") );
	LOG.PrintTimeAndLog( 0, "%s\tact = %s", ts.c_str(), node->GetStringAttribute("act") );
	LOG.PrintTimeAndLog( 0, "%s\tweight = %s", ts.c_str(), node->GetStringAttribute_e("weight") );
	LOG.PrintTimeAndLog( 0, "%s\tcomment = %s", ts.c_str(), node->GetStringAttribute_e("comment") );
}

ioAIBvTNode::ioAIBvTNode( ioXMLElement* node, int depth )
	: m_nMaxWeightSum( 0 ), m_pExec( (ioAIExecUnit*)0 ), m_bIgnore( false )
{
	if( node != (ioXMLElement*)0 && !node->IsEmpty() )
	{
		SetNode( node, depth );
	}
}

ioAIBvTNode::~ioAIBvTNode()
{
	for( std::vector<ioAIBvTNode*>::iterator 
				it = m_vChildren.begin();
				it != m_vChildren.end();
				it++ )
	{
		SAFEDELETE( *it );
	}
	SAFEDELETE( m_pExec );
}

void	ioAIBvTNode::SetNode( ioXMLElement* node, int depth )
{
	SetNodeName( node->GetStringAttribute_e( "name" ) );
	SetNodeType( node->GetTagName() );
	SetHaltType( node->GetStringAttribute_e("halt") );
	SetRoleType( node->GetStringAttribute_e("role") );
	SetExecUnit( node->GetStringAttribute("act") );

	SetIgnore( node->GetStringAttribute_e("ignore") );

	m_nWeight	= node->GetIntAttribute_e( "weight" );
	m_szComment = node->GetStringAttribute_e( "comment" );
	
	m_Timer.SetDuration( (unsigned int)node->GetIntAttribute_e( "checkfor" ) );

	//PrintXMLLogForBvT( node, depth );
}

void	ioAIBvTNode::SetNodeName( const std::string& name )
{
	m_szName = name;
}

void	ioAIBvTNode::SetNodeType( const std::string& nodeType )
{
	m_eNodeType = g_AISymbolTable.GetBvTNodeType( nodeType );
}

void	ioAIBvTNode::SetHaltType( const std::string& haltType )
{
	if( haltType == "" )
	{
		switch( m_eNodeType )
		{
		case AI_BvT_NT_SEQUENCE:	m_eHaltType = AI_BvT_HT_ON_FALSE;	break;
		case AI_BvT_NT_SELECT:		m_eHaltType = AI_BvT_HT_ON_TRUE;	break;
		case AI_BvT_NT_RANDOM:		m_eHaltType = AI_BvT_HT_ON_TRUE;	break;
		case AI_BvT_NT_ACTION:		m_eHaltType = AI_BvT_HT_NONE;		break;
		case AI_BvT_NT_REPEAT:		m_eHaltType = AI_BvT_HT_ON_FALSE;	break;
		case AI_BvT_NT_DECIDE:		m_eHaltType = AI_BvT_HT_ON_TRUE;	break;
		case AI_BvT_NT_IMPORT:		m_eHaltType = AI_BvT_HT_NONE;		break;
		}
	}
	else m_eHaltType = g_AISymbolTable.GetBvTHaltType( haltType );
}

void	ioAIBvTNode::SetRoleType( const std::string& roleType )
{
	m_eRoleType = g_AISymbolTable.GetBvTRoleType( roleType );
}

void	ioAIBvTNode::SetIgnore( const std::string& ignore )
{
	std::string		strval = ignore;

	m_bIgnore = (tolower(strval) == "yes");
}

void	ioAIBvTNode::ResetNode()
{
	if( m_pExec != (ioAIExecUnit*)0 )
		m_pExec->SetHolding( false );

	for( std::vector<ioAIBvTNode*>::iterator it = m_vChildren.begin(); it != m_vChildren.end(); it++ )
	{
		(*it)->ResetNode();
	}
}

void	ioAIBvTNode::AddChild( ioAIBvTNode* child )
{
	m_vChildren.push_back( child );
}

void	ioAIBvTNode::SetExecUnit( const std::string& actstr )
{
	std::regex	reg( 	g_AISymbolTable.GetSymbolString("opname")		+
						g_AISymbolTable.GetSymbolString("varname")		+
						g_AISymbolTable.GetSymbolString("methodname")	+
						g_AISymbolTable.GetSymbolString("args")			);
	std::smatch sm;

	if( std::regex_match( actstr, sm, reg ) )
	{
		m_pExec = new ioAIExecUnit(
							g_AISymbolTable.GetExecOperatorType( sm[1] ),
							g_AISymbolTable.GetExecVariableType( sm[2] ),
							g_AISymbolTable.GetExecMethodType( sm[3] ),
							sm[4] );

		//LOG.PrintTimeAndLog( 0, "ExecUnit set with act string : %s", actstr.c_str() );
	}
	else m_pExec = (ioAIExecUnit*)0;
}

bool	ioAIBvTNode::Execute(ioBaseChar*& pCurTarget, ioChampionAI* pCAI, bool& bEvac, bool& bRecover )
{
	if( !m_szComment.empty() && pCAI->IsDebugging( ioAIBrain::AIDEBUG_SHOW_COMMENT ) )
		LOG.PrintTimeAndLog( 0, "Comment : %s", m_szComment.c_str() );

	if( !m_szName.empty() && pCAI->IsDebugging( ioAIBrain::AIDEBUG_SHOW_NAME ) )
	{
		LOG.PrintTimeAndLog( 0, ">> Entering named BvT Node called : %s", m_szName.c_str() );
	}

	bool	bRet = false;
	switch( m_eNodeType )
	{
	case AI_BvT_NT_ACTION:	bRet = ProcessAction( pCurTarget, pCAI, bEvac, bRecover );		break;
	case AI_BvT_NT_DECIDE:	bRet = ProcessDecide( pCurTarget, pCAI, bEvac, bRecover );		break;
	case AI_BvT_NT_RANDOM:	bRet = ProcessRandom( pCurTarget, pCAI, bEvac, bRecover );		break;
	default:				bRet = ProcessFlowControl( pCurTarget, pCAI, bEvac, bRecover );	break;
	}

#ifdef _DEBUG_BvT_INOUT
	if( !m_szName.empty() )
	{
		LOG.PrintTimeAndLog( 0, "<< Exiting named BvT Node called : %s", m_szName.c_str() );
	}
#endif

	return bRet;
}

bool	ioAIBvTNode::ProcessAction( ioBaseChar*& pCurTarget, ioChampionAI* pCAI, bool& bEvac, bool& bRecover )
{
	if( m_pExec != (ioAIExecUnit*)0 )
	{
		if( m_pExec->IsHolding() )
		{
			//LOG.PrintTimeAndLog( 0, "-->> Node(recovering) : %s", m_szName.c_str() );
			m_pExec->SetHolding( false );
			bRecover = false;
			//LOG.PrintTimeAndLog( 0, "<<-- last result : %s %d", m_szName.c_str(), m_pExec->GetLastResult() );

			return m_pExec->GetLastResult();
		}
		else
		{
			//LOG.PrintTimeAndLog( 0, "-->> Node : %s", m_szName.c_str() );

			bool ret =  m_pExec->Execute( pCurTarget, pCAI );
			if( m_pExec->IsHolding() )	bEvac = true;	// evacuation
			//LOG.PrintTimeAndLog( 0, "<<-- Evac : %s %d", m_szName.c_str(), bEvac );

			return ret;
		}
	}
	else return false;
}

ioAIBvTNode::ChildrenLink::iterator	ioAIBvTNode::FindDecisionElementByResult( bool bResult )
{
	ChildrenLink::iterator	it;

	if( bResult )	it = FindDecisionElementByRole( AI_BvT_RT_DEC_ON_TRUE );
	else			it = FindDecisionElementByRole( AI_BvT_RT_DEC_ON_FALSE );

	return it;
}

bool	ioAIBvTNode::ProcessDecide( ioBaseChar*& pCurTarget, ioChampionAI* pCAI, bool& bEvac, bool& bRecover )
{
	bool					bRet = false;
	ChildrenLink::iterator	qit, it;

	// decide node is built-in node that combination of select and sequence node
	// first node is condition node and mendatory (return false if there are no children).
	// second node is onTrue and mendatory (return false if there is only one child - condition).
	// third node is onFalse and can be omitted.
	if( bRecover && pCAI->GetCallStackCount() > 0 )
	{
		qit = pCAI->PopCallStack();
		//LOG.PrintTimeAndLog( 0, "---> Recovering %s : %s", m_szComment.c_str(), (*qit)->GetNodeName().c_str() );
	}
	else
	{
		//LOG.PrintTimeAndLog( 0, "---> Get new query %s", m_szComment.c_str() );
		if( pCAI->GetCallStackCount() == 0) 	bRecover = false;
		qit = FindDecisionElementByRole( AI_BvT_RT_DEC_QUERY );
	}

	if( qit != m_vChildren.end() )
	{
		//LOG.PrintTimeAndLog( 0, "-------------> Searching %s", m_szComment.c_str() );
		bRet = (*qit)->Execute( pCurTarget, pCAI, bEvac, bRecover );
		//LOG.PrintTimeAndLog( 0, "-------------> found result %d", bRet );

		if( bEvac )	pCAI->PushCallStack( qit );
		else
		{
			if( (*qit)->GetRoleType() == AI_BvT_RT_DEC_QUERY )
			{
				if( bRet )	
					it = FindDecisionElementByRole( AI_BvT_RT_DEC_ON_TRUE );
				else		
					it = FindDecisionElementByRole( AI_BvT_RT_DEC_ON_FALSE );

				if( it != m_vChildren.end() )
				{
					bRet = (*it)->Execute( pCurTarget, pCAI, bEvac, bRecover );
					if( bEvac )	pCAI->PushCallStack( it );
				}
				else bRet = false;
			}
		}
	}

	return bRet;
}

ioAIBvTNode::ChildrenLink::iterator	ioAIBvTNode::FindDecisionElementByRole( AIBvTRoleType rt )
{
	ChildrenLink::iterator it;
	for( it = m_vChildren.begin(); it != m_vChildren.end(); it++ )
	{
		if( (*it)->GetRoleType() == rt )
			return it;
	}

	return it;
}

std::vector<ioAIBvTNode*>::iterator	ioAIBvTNode::GetProperIterator( ioChampionAI* pCAI, bool& bRecover )
{
	ChildrenLink::iterator	it;
	if( bRecover )
	{
		it = pCAI->PopCallStack();
		if( pCAI->IsCallStackEmpty() )
		{
			// end of call stack : stop recovery process
			bRecover = false;
		}
	}
	else it = m_vChildren.begin();

	return it;
}

bool	ioAIBvTNode::ProcessFlowControl(ioBaseChar*& pCurTarget, ioChampionAI* pCAI, bool& bEvac, bool& bRecover )
{
	bool	bHalt = false, bRet = false, bTimeout = false;
	std::vector<ioAIBvTNode*>::iterator it = GetProperIterator( pCAI, bRecover );

	m_Timer.StartTimerOnce();
	bTimeout = m_Timer.HasTimeoutOccured();
	for( ; it != m_vChildren.end() && !bHalt && !bTimeout; it++ )
	{
		bRet = (*it)->Execute( pCurTarget, pCAI, bEvac, bRecover );

		// process evacuation if it has been detected below
		if( bEvac )
		{
			pCAI->PushCallStack( it );
			return bRet;
		}

		if( !(bHalt = ShallWeHaltNow( bRet ) ) )
		{
			if( m_eNodeType == AI_BvT_NT_REPEAT && ( it + 1 ) == m_vChildren.end() )
			{
				bEvac = true;
				pCAI->PushCallStack( m_vChildren.begin() );
				return bRet;
			}
		}
		bTimeout = m_Timer.HasTimeoutOccured();
	}

	if( bTimeout )
	{
		// release recovery situation and return false
		m_Timer.StopTimer();
		bRet = bEvac = bRecover = false;
		pCAI->ResetCallStack();
		LOG.PrintTimeAndLog( 0, "Timeout occured : %s, start = %u, duration = %u, ends = %u", 
			m_szName, m_Timer.GetDuration(), m_Timer.GetStartTime(), FRAMEGETTIME() );
	}

	return bRet;
}

bool	ioAIBvTNode::ProcessRandom( ioBaseChar*& pCurTarget, ioChampionAI* pCAI, bool& bEvac, bool& bRecover )
{
	if( bRecover )
	{
		bool	bRet = false;
		std::vector<ioAIBvTNode*>::iterator	it = pCAI->PopCallStack();
		bRet = (*it)->Execute( pCurTarget, pCAI, bEvac, bRecover );
		if( bEvac )
		{
			pCAI->PushCallStack( it );
		}

		return bRet;
	}
	else
	{
		int curAccWeight = 0;
		int lot = rand() % (m_nMaxWeightSum==0?m_vChildren.size():m_nMaxWeightSum);
		//LOG.PrintTimeAndLog( 0, "Picked the lot %d within %d", lot, m_nMaxWeightSum );
		for( ChildrenLink::iterator	it = m_vChildren.begin(); it != m_vChildren.end(); it++ )
		{
			curAccWeight += (((*it)->GetWeight()==0)?1:(*it)->GetWeight());
			//LOG.PrintTimeAndLog( 0, "\tchecking... curAccWeight = %d, curWeight = %d", curAccWeight, (*it)->GetWeight() );
			if( lot < curAccWeight )
			{
				bool ret = (*it)->Execute( pCurTarget, pCAI, bEvac, bRecover );
				if( bEvac )
				{
					pCAI->PushCallStack( it );
				}
				return ret;
			}
		}
	}
	return false;
}

bool	ioAIBvTNode::ShallWeHaltNow( bool curRet )
{
	switch( m_eHaltType )
	{
	default:
	case AI_BvT_HT_ERROR:		return true;
	case AI_BvT_HT_NONE:		return false;
	case AI_BvT_HT_ON_TRUE:		if(curRet)return true; else return false;
	case AI_BvT_HT_ON_FALSE:	if(curRet)return false; else return true;
	}
}

void	ioAIBvTNode::VerifyBvTNode( int depth )
{
	std::string	ts, nt, ht, rt, exec;

	for( int i = 0; i < depth; i++ )
		ts += "\t";

	switch( m_eNodeType )
	{
	case AI_BvT_NT_ERROR:		nt = "ERROR";		break;
	case AI_BvT_NT_SEQUENCE:	nt = "SEQUENCE";	break;
	case AI_BvT_NT_SELECT:		nt = "SELECT";		break;
	case AI_BvT_NT_ACTION:		nt = "ACTION";		break;
	case AI_BvT_NT_REPEAT:		nt = "REPEAT";		break;
	case AI_BvT_NT_RANDOM:		nt = "RANDOM";		break;
	case AI_BvT_NT_DECIDE:		nt = "DECIDE";		break;
	}

	switch( m_eHaltType )
	{
	case AI_BvT_HT_ERROR:		ht = "ERROR";	break;
	case AI_BvT_HT_NONE:		ht = "none";	break;
	case AI_BvT_HT_ON_TRUE:		ht = "onTrue";	break;
	case AI_BvT_HT_ON_FALSE:	ht = "onFalse";	break;
	}

	switch( m_eRoleType )
	{
	case AI_BvT_RT_ERROR:		rt = "ERROR";			break;
	case AI_BvT_RT_DEFAULT:		rt = "DEFAULT";			break;
	case AI_BvT_RT_DEC_QUERY:	rt = "DEC_QUERY";		break;
	case AI_BvT_RT_DEC_ON_TRUE:	rt = "DEC_ON_TRUE";		break;
	case AI_BvT_RT_DEC_ON_FALSE:rt = "DEC_ON_FALSE";	break;
	}

	if( m_pExec != (ioAIExecUnit*)0 )
		exec = m_pExec->GetDebugString();

	LOG.PrintTimeAndLog( 0, "%s[%s halt=\"%s\" role=\"%s\" exec=\"<%x:%s>\"]", 
		ts.c_str(), nt.c_str(), ht.c_str(), rt.c_str(), m_pExec, exec.c_str() );

	for( std::vector<ioAIBvTNode*>::iterator 
			it = m_vChildren.begin(); 
			it != m_vChildren.end(); 
			it++ )
	{
		(*it)->VerifyBvTNode( depth + 1 );
	}
}

//////////////////////////////////////////////////////////////////////
ioAIBvT::ioAIBvT( ioXMLElement* node, int depth )
	: m_bEvac( false )
{
	m_pRoot = BuildBehaviorTree( node, depth );
	m_pCurNode = m_pRoot;
}

ioAIBvT::~ioAIBvT()
{
	SAFEDELETE( m_pRoot );
}

ioAIBvTNode* ioAIBvT::GetFlowControlNode( ioXMLElement* node, int depth )
{
	int weight = node->GetIntAttribute_e( "weight" );
	int	maxWeight = 0;

	// build child if there are local definition : ignore use or act clause
	ioAIBvTNode* curBvTNode = new ioAIBvTNode( node, depth );

	ioXMLElement*	child = &(node->FirstChild());
	while( !child->IsEmpty() )
	{
		ioAIBvTNode* childBvTNode = BuildBehaviorTree( child , depth + 1);
		if( childBvTNode )
		{
			curBvTNode->AddChild( childBvTNode );
			maxWeight += childBvTNode->GetWeight();
		}
		child = &(child->NextSibling());
	}
	curBvTNode->SetMaxChildrenWeightSum( maxWeight );

	return curBvTNode;
}

ioAIBvTNode* ioAIBvT::BuildBehaviorTree( ioXMLElement* node, int depth )
{
	if( node != (ioXMLElement*)0 && !node->IsEmpty() )
	{
		switch( g_AISymbolTable.GetBvTNodeType( node->GetTagName() ) )
		{
		case AI_BvT_NT_ACTION:
		{
			std::string		act = node->GetStringAttribute( "act" );

			if( node->GetChildCount() > 0 )
			{
				LOG.PrintTimeAndLog( 0, 
					"BvT : ACTION node cannot have child, but it has %d children.", 
						node->GetChildCount() );
			}
			ioAIBvTNode*	curBvTNode = new ioAIBvTNode( node, depth );

			return curBvTNode;
		}
		case AI_BvT_NT_IMPORT:
		{
			std::string	use = node->GetStringAttribute( "use" );

			return FindBehaviorTreeRoot( use, depth );
		}
		default:
		//case AI_BvT_NT_SELECT:	case AI_BvT_NT_SEQUENCE:
		//case AI_BvT_NT_REPEAT:	case AI_BvT_NT_RANDOM:	case AI_BvT_NT_DECIDE:
			return GetFlowControlNode( node, depth );
		}
	}

	return ((ioAIBvTNode*)0);
}

ioAIBvTNode* ioAIBvT::FindBehaviorTreeRoot( const std::string& cmd, int depth )
{
	ioAIBvT* bvt = g_AIDB.GenerateBehaviorTree( cmd, depth );
	
	if( bvt != (ioAIBvT*)0 )
	{
		ioAIBvTNode* node = bvt->GetRootNode();
			
		bvt->CutOffRootNode();	// to delete BvT object without losing tree itself
		SAFEDELETE( bvt );

		return node;
	}
	else return (ioAIBvTNode*)0;
}

bool ioAIBvT::ProcessBehavior(	ioBaseChar*& pCurTarget, ioChampionAI* pCAI )
{
	if( m_pRoot != (ioAIBvTNode*)0 )
	{
		bool bRecover = m_bEvac;

		m_bEvac = false;

#ifdef _DEBUG_BvT_EVAC
		LOG.PrintTimeAndLog( 0, 
			"> Entering Behavior tree : m_bEvac = %d, bRecover = %d", m_bEvac, bRecover );
#endif

		bool bTraverseResult = m_pRoot->Execute( pCurTarget, pCAI, m_bEvac, bRecover );

		if( !m_bEvac )
		{
#ifdef _DEBUG_BvT_EVAC
			LOG.PrintTimeAndLog( 0, "> Exiting Behavior tree : No evacuation condition." );
#endif
			m_pRoot->ResetNode();
		}
		else
		{
#ifdef _DEBUG_BvT_EVAC
			LOG.PrintTimeAndLog( 0, "> Exiting Behavior tree : Evacuation condition found." );
#endif
		}

		return bTraverseResult;
	}
	else return false;
}

void	ioAIBvT::VerifyBvT()
{
	if( m_pRoot )
		m_pRoot->VerifyBvTNode( 0 );
	else LOG.PrintTimeAndLog( 0, "No root of behavior tree found" );
}

void	ioAIBvT::Reset()
{
	m_bEvac = false;
	if( m_pRoot )
		m_pRoot->ResetNode();
	else LOG.PrintTimeAndLog( 0, "No root of behavior tree found" );
}