// cWorkerThread.cpp: implementation of the cWorkerThread class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "cIocpContext.h"
#include "cPacket.h"
#include "cSystem.h"
#include "cWorkerThread.h"
#include "../include/cSingleton.h"


#ifndef CONTAINING_RECORD 
#define CONTAINING_RECORD(address, type, field) \ 
((type *)((PCHAR)(address) - (ULONG_PTR)(&((type *)0)->field))) 
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

cWorkerThread::cWorkerThread() : m_packet(NULL)
{
	Init();
}

cWorkerThread::~cWorkerThread()
{
	Destroy();
}

void cWorkerThread::Init()
{
	uint32 maxPacket = cSingleton<cSystem>::GetInstance()->GetPacket();
	uint32 encoding = cSingleton<cSystem>::GetInstance()->GetEncoding();

	m_packet = new cPacket( encoding );
	m_storage.Create( maxPacket * 4 );
}

void cWorkerThread::Destroy()
{
	if(m_packet)
	{
		delete m_packet;
		m_packet = NULL;
	}
}

//////////////////////////////////////////////////////////////////////
// operation
//////////////////////////////////////////////////////////////////////

void cWorkerThread::Process()
{
	BOOL success;
	DWORD bytesTransferred;

	cSystem* system = cSingleton<cSystem>::GetInstance();

	cOverlapped*	iocpOv			= NULL;
	cIocpContext*	iocpContext		= NULL;
	LPWSAOVERLAPPED	lpOverlapped	= NULL;
	
	cIOCP* IOCP			= system->GetIOCP();
	cNetwork* network	= system->GetNetwork();

	Trace( _T("WorkerThread :: %lu\r\n"), GetCurrentThreadId() );

	while( !IsTerminated() )
	{
		iocpContext	= NULL;
		lpOverlapped	= NULL;

		success = IOCP->GetIocpStatus(	&bytesTransferred, 
											reinterpret_cast<PULONG>(&iocpContext), 
											&lpOverlapped );	
		if( !lpOverlapped )	
		{
			Trace( _T("GQCS timeout\r\n") );
			continue;
		}

		// 이벤트 검사
		iocpOv = reinterpret_cast<cOverlapped*>(lpOverlapped);
		switch( iocpOv->GetOperation() )
		{
		case E_IO_ACCEPT : // 여기서 iocpContext는 cServerSocket객체임
			{
				iocpContext = CONTAINING_RECORD( lpOverlapped, cIocpContext, m_olAccept );

				// Accept 요청완료
				network->AcceptCompleted( iocpContext->GetPortID() );

				if( !success )
				{
					// 접속 대기 중 오류 - backlog된 소켓이 종료됐을때
					Debug( _T("WorkerThread :: E_IO_ACCEPT failed\r\n") );

					network->Finish( iocpContext );
					continue;
				}
				
				if( !iocpContext->IsAssociated() )
				{
					// 미등록된 Context를 IOCP에 등록
					if( !IOCP->AssocInstance(	reinterpret_cast<HANDLE>(iocpContext->GetSockHandle()),
												reinterpret_cast<DWORD>(iocpContext) ) )
					{
						// 접속 대기 중 오류 - 이미 등록되어 있거나 기타 오류시
						Debug( _T("WorkerThread :: AssocInstance failed\r\n") );
						network->Finish( iocpContext );
						continue;
					}
					iocpContext->Associate();
				}

				// 접속성공
				iocpContext->Reset();

				// Receive 요청
				if( !iocpContext->ReceiveRequest() )
				{
					iocpContext->Disconnect();
				}

				OnConnect( iocpContext );

				// 새로운 Accept 요청
				network->AcceptReqeust( iocpContext->GetPortID() );
			}
			break;

		case E_IO_RECEIVE :
			{
				if( !success || !bytesTransferred )	
				{
					OnDisconnect( iocpContext );
					continue;
				}

				// 패킷 처리
				if( OnReceive( iocpContext, bytesTransferred ) )
				{
					// Receive 요청
					if( !iocpContext->ReceiveRequest() )
					{
						OnDisconnect( iocpContext );
					}
				}
			}
			break;

		case E_IO_SEND :
			{
				cIocpOv* sendOvl = reinterpret_cast<cIocpOv*>(lpOverlapped);
				if( iocpContext )
				{
					// 전송버퍼 push
					iocpContext->SendCompleted( sendOvl );
					//Debug( _T("WorkerThread :: Sent(%lu)\r\n"), bytesTransferred );
				}
				if( !success )
				{
					Debug( _T("WorkerThread :: E_IO_SEND failed\r\n") );
				}
			}
			break;

		case E_IO_DISCONNECT :
			{
				if( success && iocpContext )
				{
					// Context를 반환하고 새로운 접속을 요청
					network->Finish( iocpContext );
				}
				else
				{
					Debug( _T("WorkerThread :: E_IO_DISCONNECT failed\r\n") );
				}
			}
			break;

		default :
			{
				Debug( _T("WorkerThread :: Bad operation\r\n") );
			}
			break;
		}
	}
}

BOOL cWorkerThread::OnConnect( cIocpContext* iocpContext )
{
	if( iocpContext->IsActive() )	
	{
		PushCommand( EVENT_CONNECT, iocpContext );
		return TRUE;
	}
	return FALSE;
}

BOOL cWorkerThread::OnDisconnect( cIocpContext* iocpContext )
{
	if( iocpContext->IsActive() )	
	{
		PushCommand( EVENT_CLOSE, iocpContext );
		return TRUE;
	}
	return FALSE;
}

BOOL cWorkerThread::OnReceive( cIocpContext* iocpContext, DWORD length )
{
	if( iocpContext->IsActive() )	
	{
		//Trace( _T("[%lu]OnReceive : %lu\r\n"), GetCurrentThreadId(), length );

		// 임시버퍼에 보관된 데이타와 현재받은 데이타를 복사한다
		if( iocpContext->GetStorageLength() > 0 )
		{
			m_storage.Erase();
			if( !m_storage.Copy( iocpContext->GetStorageBuffer(), iocpContext->GetStorageLength() ) )	
			{
				OnDisconnect( iocpContext );
				return FALSE;
			}
			if( !m_storage.Append( iocpContext->GetWsaBuffer(), length ) ) 
			{
				OnDisconnect( iocpContext );
				return FALSE;
			}
		}
		else
		{
			m_storage.Erase();
			if( !m_storage.Copy( iocpContext->GetWsaBuffer(), length ) ) 
			{
				OnDisconnect( iocpContext );
				return FALSE;
			}
		}

		// 저장된 버퍼를 초기화
		iocpContext->CleanStorage();

		// 임시버퍼에 저장된 패킷을 검사하고 프로토콜 별로 나누어서 command queue에 push
		return CheckPacket(	iocpContext, 
							m_storage.GetBuffer(), 
							m_storage.GetLength() );
	}

	return FALSE;
}

BOOL cWorkerThread::CheckPacket( cIocpContext* iocpContext, uint8* buffer, uint32 length )
{
	//Trace( _T("[%lu]CheckPacket : %lu\r\n"), GetCurrentThreadId(), length );

	// 패킷의 헤더길이 검사
	if( length < sizeof(cHeader) )	
	{
		// 헤더보다 작은 패킷이 들어왔을 경우 저장하고 끝난다
		iocpContext->StoreBuffer( buffer, length );
		//Trace( _T("[%lu]very short header : %lu, total : %lu\r\n"), GetCurrentThreadId(), length, iocpContext->GetBufferLength() );
		return TRUE;
	}

	// 헤더로 페이로드 길이 확인
	cHeader* header = reinterpret_cast<cHeader*>(buffer);
	Trace(_T("header : %lu, context : %lu\r\n"), header->GetSequence(), iocpContext->GetSequence());
	if( !header->IsPerfect(iocpContext->GetSequence()) )
	{
		// 잘못된 헤더가 들어왔을때 패킷을 drop한다
		OnDisconnect( iocpContext );
		Debug( _T("[%lu]invalid packet : %lu\r\n"), GetCurrentThreadId() );
		return FALSE;
	}
	
	if( header->GetPayload() > (m_storage.GetMaxLength()-m_storage.GetLength()) )
	{
		Debug( _T("[%lu]too large payload : %lu\r\n"), GetCurrentThreadId(), header->GetPayload() );
		OnDisconnect( iocpContext );
		return FALSE;
	}

	// 패킷 헤더에 따른 길이 검사
	uint32 packetLength = sizeof(cHeader) + header->GetPayload();
	if( length < packetLength ) // 패킷길이가 잘려서 들어왔다
	{
		//Trace( _T("[%lu]short than payload : %lu\r\n"), GetCurrentThreadId(), packetLength );
		iocpContext->StoreBuffer( buffer, length );
		Trace( _T("[%lu]very short payload : %lu, store : %lu, total : %lu\r\n"), GetCurrentThreadId(), header->GetPayload(), length, iocpContext->GetStorageLength() );
	}
	else if( length == packetLength ) // 패킷길이가 정확하게 들어왔다
	{
		Trace( _T("[%lu]good payload : %lu\r\n"), GetCurrentThreadId(), header->GetPayload() );

		// decryption 후 command queue에 push
		if( !PushCommand(	EVENT_RECEIVE,
							iocpContext,	
							buffer, 
							packetLength ) )
		{
			OnDisconnect( iocpContext );
			return FALSE;
		}
	}
	else if( length > packetLength )	// 패킷길이가 붙어서 들어왔다
	{
		Trace( _T("[%lu]too large payload : %lu\r\n"), GetCurrentThreadId(), header->GetPayload() );

		// decryption 후 command queue에 push
		if( !PushCommand(	EVENT_RECEIVE,
							iocpContext, 
							buffer, 
							packetLength ) )
		{
			OnDisconnect( iocpContext );
			return FALSE;
		}
		// 다시 한번 패킷 검사
		return CheckPacket( iocpContext, buffer+packetLength, length-packetLength );
	}
	return TRUE;
}

BOOL cWorkerThread::PushCommand( EventID eventId, cIocpContext* iocpContext, uint8* buffer, uint32 length )
{
	cSystem* system = cSingleton<cSystem>::GetInstance();

	cCommandQueue* commandQueue = system->GetCommandQueue();
	if( !commandQueue ) return TRUE;

	switch( eventId )
	{
	case EVENT_CONNECT :
		{
			commandQueue->PushCommand( iocpContext, system->GetConnectEvent() );
		}
		break;

	case EVENT_CLOSE :
		{
			commandQueue->PushCommand( iocpContext, system->GetDisconnectEvent() );
		}
		break;

	case EVENT_RECEIVE :
		{
			if( length == 0 )
			{
				// 디코딩 실패
				OnDisconnect( iocpContext );
				return FALSE;
			}

			// 시퀀스 추가
			iocpContext->PopSequence();

			//// 패킷의 유효성 검사
			cHeader* header = reinterpret_cast<cHeader*>(buffer);
			//if( !header->IsPerfect(iocpContext->GetSequence()) )
			//{
			//	OnDisconnect( iocpContext );
			//	return FALSE;
			//}

			uint8* packet	= buffer + sizeof(cHeader);
			uint32 payload	= length - sizeof(cHeader);

			// 패킷 디코딩
			uint32 result = 0;
			if(TRUE == m_packet->Decode(
				packet,
				length - sizeof(cHeader),
				header->GetSequence(),
				result))
			{
				commandQueue->PushCommand(		
					iocpContext,
					header->GetCommand(),
					m_packet->GetOutput(),
					result );
			}
			else
			{
				commandQueue->PushCommand(		
					iocpContext,
					header->GetCommand(),
					packet,
					static_cast<uint16>(payload) );
			}
		}
		break;
	}
	return TRUE;
}
