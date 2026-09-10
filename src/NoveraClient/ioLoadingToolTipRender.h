#pragma once
#include "StdAfx.h"

bool TipStatgeAnalysis( ioINILoader_e& kTipLoader, ioComplexStringPrinter& rTipPrinter, IORandom& rRandom );
bool TipAnalysis( const std::string& stagename, ioComplexStringPrinter& rTipPrinter, IORandom& rRandom );