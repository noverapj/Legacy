#include "stdafx.h"
#include "BlockNode.h"

BlockNode::BlockNode(void) : m_bBlockFlag(false)
{
	Init();
}

BlockNode::~BlockNode(void)
{
	Destroy();
}

void BlockNode::Init()
{
}

void BlockNode::Destroy()
{
}

void BlockNode::Reset()
{
	m_bBlockFlag = false;
}

void BlockNode::SetBlockFlag(const bool bBlockFlag)
{
	m_bBlockFlag = bBlockFlag;
}

bool BlockNode::IsBlocked()
{
	return m_bBlockFlag;
}