#include "StdAfx.h"
#include "ioVector.h"
#include "Sharedvector.h"



ioVector::ioVector(void)
{
	m_list = NULL;
}
ioVector::~ioVector(void)
{
}

bool ioVector::Set( TCHAR* name,unsigned int count,int size,bool processMutexState /*= false*/ )
{
	int iii=0;
	try
	{	 
		m_list = new SharedVector();
		if(m_list->Set(name,count,size,processMutexState) == false)
		{
			return false;
		}
		if(m_list->size() >= count)
		{
			m_list->SetVectorAddress();
			return true;
		}
		/************************************************************************/
		/* 비정상적인 상황 or 처음 인 상황                                                                      */
		/************************************************************************/
		if(!m_list->empty())
		{
			m_list->clear();
			m_list->flush();
		}

		for(unsigned int i=0; i<count; ++i)
		{		 
			iii = i;
			int pos = m_list->GetBuf(size,i);
			m_list->push_back(pos);
			SharedParent_* tmpelement = reinterpret_cast<SharedParent_*>(pos);
			tmpelement->index = i;
			tmpelement->state = -1;	
		}
	}
	catch(std::exception& e)
	{
		m_lastError = e.what();
		return false;
	}
	return true;
}
int ioVector::GetFreeIndex()
{
	auto pos = m_list->begin();
	while(pos != m_list->end())
	{
		SharedParent_* element = reinterpret_cast<SharedParent_*>(*pos);
		if(element->state != -1)
		{
			return element->index;
		}
		pos++;
	}
	return -1;
}


void* ioVector::GetAt( unsigned int index )
{
	if(m_list == NULL)
		return NULL;
	if(index >= m_list->size())
		return NULL;
	SharedParent_* element = reinterpret_cast<SharedParent_*>(m_list->GetAt(index));
	if(element)
	{
		element->state = 1;
		return element;
	}
	return NULL;
}

bool ioVector::RemoveAt( unsigned int index )
{
	if(m_list == NULL)
		return false;
	if(index >= m_list->size())
		return false;
	SharedParent_* element = reinterpret_cast<SharedParent_*>(m_list->GetAt(index));
	if(element)
	{
		element->state = -1;
		return true;
	}
	return false;
}

void ioVector::RemoveVector()
{
	m_list->Remove();
}

int ioVector::Size()
{
	return m_list->size();
}

int ioVector::GetFreeMemorySize()
{
	return m_list->GetFreeMemorySize();
}

