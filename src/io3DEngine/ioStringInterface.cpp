

#include "stdafx.h"

#include "ioStringInterface.h"

ParamDictionaryMap ioStringInterface::m_ParamDictionary;

bool ioStringInterface::SetParameter( const ioHashString &kName,
									  const std::string &kValue )
{
	ioParamDictionary *pDict = GetParamDictionary();

	if( pDict )
	{
		ioParamCommand *pCmd = pDict->GetParamCommand( kName );
		if( pCmd )
		{
			pCmd->Set( this, kValue );
			return true;
		}
	}

	return false;
}
