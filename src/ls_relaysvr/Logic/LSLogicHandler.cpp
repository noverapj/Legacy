#include "StdAfx.h"
#include "LSLogicHandler.h"


LSLogicHandler::~LSLogicHandler(void)
{
}

void LSLogicHandler::Init()
{
	m_voperations.push_back(new TestOperation(m_queue)); // 0
	m_voperations.push_back(new OnAccept(m_queue));
	m_voperations.push_back(new ReConnect(m_queue));
	m_voperations.push_back(new SchedulerOperation(m_queue));
	m_voperations.push_back(new ChangeTickTime(m_queue));
}