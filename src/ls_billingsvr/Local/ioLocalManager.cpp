#include "../stdafx.h"
#include "iolocalkorea.h"
#include "iolocalus.h"
#include "iolocaltaiwan.h"
#include "iolocalIndonesia.h"
#include "./iolocalmanager.h"
#include "ioLocalThailand.h"
#include "ioLocalChina.h"
#include "ioLocalJapan.h"
#include "ioLocalLatin.h"
#include "ioLocalEU.h"

extern CLog LOG;

template<> ioLocalManager* Singleton< ioLocalManager >::ms_Singleton = 0;
ioLocalManager::LocalType ioLocalManager::m_eLocalType = ioLocalManager::LCT_KOREA;


ioLocalManager::ioLocalManager(void)
{
	m_vLocalVector.reserve( 10 );
}

ioLocalManager::~ioLocalManager(void)
{
	for(vLocalVector::iterator iter = m_vLocalVector.begin(); iter != m_vLocalVector.end(); ++iter)
	{
		delete *iter;
	}
	m_vLocalVector.clear();
}


ioLocalParent * ioLocalManager::CreateLocal( LocalType eLocalType )
{
	ioLocalParent *pLocal = NULL;
	if( eLocalType == LCT_KOREA )
		pLocal = new ioLocalKorea;
	else if( eLocalType == LCT_US )
		pLocal = new ioLocalUS;
	else if( eLocalType == LCT_TAIWAN )
		pLocal = new ioLocalTaiwan;
	else if( eLocalType == LCT_INDONESIA )
		pLocal = new ioLocalIndonesia;
	else if( eLocalType == LCT_THAILAND )
		pLocal = new ioLocalThailand;
	else if( eLocalType == LCT_CHINA )
		pLocal = new ioLocalChina;
	else if( eLocalType == LCT_JAPAN )
		pLocal = new ioLocalJapan;
	// 새로운 국가 추가
	else if( eLocalType == LCT_LATIN )
		pLocal = new ioLocalLatin;
	else if( eLocalType == LCT_EU )
		pLocal = new ioLocalEU;
	
	else
		LOG.PrintTimeAndLog( 0, "%s Type is wrong. %s", __FUNCTION__, GetLocalName( eLocalType ) );

	return pLocal;
}

void ioLocalManager::AddLocal( ioLocalParent *pLocal )
{
	if( !pLocal )
	{
		LOG.PrintTimeAndLog( 0, "%s pLocal == NULL." , __FUNCTION__ );
		return;
	}

	if( GetLocal( pLocal->GetType() ) )
	{
		LOG.PrintTimeAndLog( 0, "%s Type is existing.%s" , __FUNCTION__ , GetLocalName( pLocal->GetType() ) );
		return;
	}

	m_vLocalVector.push_back( pLocal );
}

void ioLocalManager::Init()
{
	AddLocal( CreateLocal( LCT_KOREA ) );
	AddLocal( CreateLocal( LCT_US ) );
	AddLocal( CreateLocal( LCT_TAIWAN ) );
	AddLocal( CreateLocal( LCT_INDONESIA ) );
	AddLocal( CreateLocal( LCT_THAILAND ) );
	AddLocal( CreateLocal( LCT_CHINA ) );
	AddLocal( CreateLocal( LCT_JAPAN ) );
	// 새로운 국가 추가
	AddLocal( CreateLocal( LCT_LATIN ) );
	AddLocal( CreateLocal( LCT_EU ) );
	
}

ioLocalParent * ioLocalManager::GetLocal( LocalType eLocalType )
{
	for(vLocalVector::iterator iter = m_vLocalVector.begin(); iter != m_vLocalVector.end(); ++iter)
	{
		ioLocalParent *pLocal = *iter;
		if( !pLocal )
			continue;
		if( pLocal->GetType() == eLocalType )
			return pLocal;
	}

	return NULL;
}

ioLocalManager &ioLocalManager::GetSingleton()
{
	return Singleton< ioLocalManager >::GetSingleton();
}

const char * ioLocalManager::GetLocalName( LocalType eLocalType )
{
	if(  m_eLocalType == LCT_KOREA )
		return "KOREA";
	else if( m_eLocalType == LCT_US )
		return "US";
	else if( m_eLocalType == LCT_TAIWAN )
		return "TAIWAN";
	else if( m_eLocalType == LCT_INDONESIA )
		return "INDONESIA";
	else if( m_eLocalType == LCT_THAILAND )
		return "THAILAND";
	else if( m_eLocalType == LCT_CHINA )
		return "CHINA";
	else if( m_eLocalType == LCT_JAPAN )
		return "JAPAN";
	else if( m_eLocalType == LCT_LATIN )
		return "LATIN";
	else if( m_eLocalType == LCT_EU )
		return "EU";
	else
		return "UNKNOWN";
}
