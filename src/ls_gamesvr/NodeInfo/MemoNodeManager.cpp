#include "stdafx.h"
//#include "../Window.h"
#include "../MainProcess.h"

#include "User.h"
#include "UserNodeManager.h"
#include "MemoNodeManager.h"

MemoNode::MemoNode()
{
	m_dwMemoTime = 0;
}
MemoNode::~MemoNode(){}
void MemoNode::SetMemo( ioHashString ToID, ioHashString FromID, ioHashString Memo )
{
	m_ToID	= ToID;
	m_FromID= FromID;
	m_Memo	= Memo;	

	//등록 시간
	SYSTEMTIME st;
	GetLocalTime( &st );
	m_dwMemoTime = (st.wMonth * 1000000) + (st.wDay * 10000) + (st.wHour * 100) + st.wMinute;
}

MemoNodeManager* MemoNodeManager::sg_Instance = NULL;

MemoNodeManager::MemoNodeManager()
{
}

MemoNodeManager::~MemoNodeManager()
{
	m_vMemoNode.clear();
}

MemoNodeManager &MemoNodeManager::GetInstance()
{
	if( sg_Instance == NULL )
		sg_Instance = new MemoNodeManager;
	return *sg_Instance;
}

void MemoNodeManager::ReleaseInstance()
{
	SAFEDELETE( sg_Instance );
}

void MemoNodeManager::AddMemo( MemoNode &rkMemo )
{
	m_vMemoNode.push_back( rkMemo );
}

void MemoNodeManager::SendMemo( UserParent *pUser, int iSendCount )
{
	CRASH_GUARD();
	static vMemoNode vMyMemoList;
	vMyMemoList.clear();

	LOOP_GUARD();
	vMemoNode_iter iter = m_vMemoNode.begin();
	while( iter != m_vMemoNode.end() )
	{
		MemoNode &rkMemo = *iter;		
		if( rkMemo.GetToID() == pUser->GetPublicID() )
		{
			vMyMemoList.push_back( rkMemo );
			iter = m_vMemoNode.erase( iter );
			
			// 전송 갯수 제한
			if( (int)vMyMemoList.size() >= iSendCount )
				break;				
		}
		else
			++iter;			
	}
	LOOP_GUARD_CLEAR();

	int iCurSize = vMyMemoList.size();
	SP2Packet kPacket( STPK_OFFLINE_MEMO_MSG );
	kPacket << iCurSize;
	if( iCurSize == 0 )
	{
		pUser->RelayPacket( kPacket );
		return;
	}

	LOOP_GUARD();
	vMemoNode_riter rIter, rIEnd;
	rIter = vMyMemoList.rbegin();
	rIEnd = vMyMemoList.rend();
	while( rIter != rIEnd )
	{
		MemoNode &rkMemo = *rIter++;
		bool bOnline = false;
		if( g_UserNodeManager.GetGlobalUserNode( rkMemo.GetFromID() ) )
			bOnline = true;
		kPacket << rkMemo.GetFromID() << rkMemo.GetMemo() << rkMemo.GetTime() << bOnline;		
	}
	LOOP_GUARD_CLEAR();
	vMyMemoList.clear();
	pUser->RelayPacket( kPacket );
}
