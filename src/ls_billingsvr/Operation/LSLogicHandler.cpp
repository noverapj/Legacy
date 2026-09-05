#include "../stdAfx.h"
#include "LSLogicHandler.h"
#include "LSLogicOperations.h"


LSLogicHandler::~LSLogicHandler(void)
{
}

void LSLogicHandler::Init()
{
	m_operations.push_back(new TestOperation(m_queue)); // 0
	m_operations.push_back(new OnConnect(m_queue));
	m_operations.push_back(new WriteProcessLog(m_queue));
// 	m_operations.push_back(new OnAccept(m_queue));
// 	m_operations.push_back(new ReConnect(m_queue));
// 	m_operations.push_back(new SchedulerOperation(m_queue));
// 	m_operations.push_back(new ChangeTickTime(m_queue));
	m_operations.push_back(new TestCloseOperation(m_queue));
}