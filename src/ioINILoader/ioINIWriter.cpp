#include "StdAfx.h"
#include "ioINIWriter.h"
#include "ioINIControl.h"

ioINIWriter::ioINIWriter(void) : m_runState(false)
{
}

ioINIWriter::~ioINIWriter(void)
{
	Destroy();
}

void ioINIWriter::Init()
{
	iocpQueue::Startup();

	m_ININodePool.CreatePool(0, 100000, TRUE);

	for(int i = 0; i < 100; i++)
	{
		ioININode* node = new ioININode;

		if(node)
			m_ININodePool.Push(node);
	}
}

void ioINIWriter::Destroy()
{
	SetRunState(false);

	m_ININodePool.DestroyPool();
}

void ioINIWriter::Run()
{
	DWORD size = 0;

	while(GetRunState())
	{
		ioININode* node = (ioININode*)iocpQueue::Dequeue(size);

		if(node)
		{
			SaveData( node );
		}
	}
}

void ioINIWriter::SaveData( ioININode* node )
{
	switch(node->GetSaveType())
	{
	case 1:
		{
			TCHAR szBuf[MAX_PATH]= _T( "" );
			StringCbPrintf( szBuf, sizeof( szBuf ), _T( "%d" ), static_cast<int32>(node->GetValue()) );
			WritePrivateProfileString( node->GetSection(), node->Getkey(), szBuf, node->GetFileName() );
		}
		break; 
	case 2:
		{
			if( node->GetValue() ? true : false )
			{
				WritePrivateProfileString( node->GetSection(), node->Getkey(), _T( "1" ), node->GetFileName() );
			}
			else
			{
				WritePrivateProfileString( node->GetSection(), node->Getkey(), _T( "0" ), node->GetFileName() );
			}
		}
		break;
	case 3:
		{
			TCHAR szBuf[MAX_PATH]= _T( "" );
			StringCbPrintf( szBuf, sizeof( szBuf ), _T( "%f" ), static_cast<float>(node->GetValue()) );
			WritePrivateProfileString( node->GetSection(), node->Getkey(), szBuf, node->GetFileName() );
		}
		break;
	case 4:
		{
			TCHAR szBuf[MAX_PATH]= _T( "" );
			StringCbPrintf( szBuf, sizeof( szBuf ), _T( "%.2f" ), static_cast<float>(node->GetValue()) );
			WritePrivateProfileString( node->GetSection(), node->Getkey(), szBuf, node->GetFileName() );
		}
		break;
	}
	
	m_ININodePool.Push(node);
}

void ioINIWriter::PushNode( ioININode* node )
{
	DWORD key = (DWORD)node;
	iocpQueue::Enqueue(key, sizeof(node));
}

ioININode* ioINIWriter::GetDataNode()
{
	return m_ININodePool.Pop();
}