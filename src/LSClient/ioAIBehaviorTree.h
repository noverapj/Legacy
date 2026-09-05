#pragma once

#include "../io3DEngine/ioXMLDocument.h"
#include "../io3DEngine/ioXMLElement.h"

#include <map>
#include <vector>

#include "ioAIDefinitions.h"
#include "ioAIExecUnit.h"
#include "ioAITimer.h"

void	PrintXMLLogForBvT( ioXMLElement* node, int depth );

class	ioAIBvTNodeLink;
class	ioAIBvTNode
{
public:
	ioAIBvTNode( ioXMLElement* node, int depth = -1 );
	ioAIBvTNode( ioXMLElement* node, const std::string& act, int depth = -1 );
	~ioAIBvTNode();

	typedef std::vector<ioAIBvTNode*>	ChildrenLink;

	void	SetNode( ioXMLElement* node, int depth = -1 );
	void	SetNodeName( const std::string& name );
	void	SetNodeType( const std::string& nodeType );
	void	SetHaltType( const std::string& haltType );
	void	SetRoleType( const std::string& roleType );
	void	SetIgnore( const std::string& ignore );
	void	ResetNode();
	

	void	AddChild( ioAIBvTNode* child );
	void	SetExecUnit( const std::string& actstr );
	
	std::vector<ioAIBvTNode*>::iterator	
			GetProperIterator( ioChampionAI* pCAI, bool& bRecover );
	std::string	
			GetNodeName()	{	return m_szName;	}

	bool	Execute( ioBaseChar*& pCurTarget, ioChampionAI* pCAI, bool& bEvac, bool& bRecover );
	bool	ProcessAction( ioBaseChar*& pCurTarget, ioChampionAI* pCAI, bool& bEvac, bool& bRecover );
	bool	ProcessDecide( ioBaseChar*& pCurTarget, ioChampionAI* pCAI, bool& bEvac, bool& bRecover );
	bool	ProcessFlowControl(ioBaseChar*& pCurTarget, ioChampionAI* pCAI, bool& bEvac, bool& bRecover );
	bool	ProcessRandom( ioBaseChar*& pCurTarget, ioChampionAI* pCAI, bool& bEvac, bool& bRecover );
	bool	ShallWeHaltNow( bool curRet );
	
	ChildrenLink::iterator	FindDecisionElementByRole( AIBvTRoleType rt );
	ChildrenLink::iterator	FindDecisionElementByResult( bool bResult );

	void	SetMaxChildrenWeightSum( int maxWeightSum )	{	m_nMaxWeightSum = maxWeightSum;	}
	int		GetMaxChildrenWeightSum()					{	return m_nMaxWeightSum;	}
	
	void	SetWeight( int weight )	{ m_nWeight = weight;	}
	int		GetWeight()				{ return m_nWeight;		}

	AIBvTNodeType	GetNodeType()	{	return m_eNodeType;	}
	AIBvTHaltType	GetHaltType()	{	return m_eHaltType;	}
	AIBvTRoleType	GetRoleType()	{	return m_eRoleType;	}

	void	VerifyBvTNode( int depth );

protected:
	std::string		m_szName;
	AIBvTNodeType	m_eNodeType;
	AIBvTHaltType	m_eHaltType;
	AIBvTRoleType	m_eRoleType;
	int				m_nMaxWeightSum;
	ioAIExecUnit*	m_pExec;
	std::string		m_szComment;

	int				m_nWeight;
	ChildrenLink	m_vChildren;
	
	ioAITimer		m_Timer;

	bool			m_bIgnore;
};

void PrintLogBvTNode( ioAIBvTNode* node );

class	ioAIBvT
{
public:
	ioAIBvT( ioXMLElement* node, int depth = -1 );
	~ioAIBvT();

	ioAIBvTNode*	GetRootNode()	{ return m_pRoot; }
	ioAIBvTNode*	BuildBehaviorTree( ioXMLElement* node, int depth = -1 );
	ioAIBvTNode*	FindBehaviorTreeRoot( const std::string& cmd, int depth = -1 );
	ioAIBvTNode*	GetFlowControlNode( ioXMLElement* node, int depth = -1 );

	ioAIBvTNode*	CutOffRootNode()	
	{	
		ioAIBvTNode* t = m_pRoot;
		m_pRoot = (ioAIBvTNode*)0;

		return t;
	}

	void	VerifyBvT();
	bool	ProcessBehavior( ioBaseChar*& pCurTarget, ioChampionAI* pCAI );

	void	Reset();
	
protected:
	ioAIBvTNode*	m_pRoot;
	ioAIBvTNode*	m_pCurNode;
	bool			m_bEvac;
};
