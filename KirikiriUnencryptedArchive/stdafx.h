#pragma once

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <imagehlp.h>
#include <strsafe.h>
#include <algorithm>
#include <cassert>
#include <exception>
#include <functional>
#include <map>
#include <ranges>
#include <regex>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include "../Detours/detours.h"

#include "Proxy.h"
#include "Common.h"

#include "PE/PE.h"

#include "CompilerSpecific/Enumerations.h"
#include "CompilerSpecific/CompilerSpecificVector.h"
#include "CompilerSpecific/Rtti/BorlandTypeDescriptor.h"
#include "CompilerSpecific/Rtti/MsvcRttiCompleteObjectLocator.h"
#include "CompilerSpecific/CallingConvention/BorlandRegToCdeclAdapter.h"
#include "CompilerSpecific/CallingConvention/CdeclToBorlandRegAdapter.h"
#include "CompilerSpecific/CallingConvention/CdeclToThiscallAdapter.h"
#include "CompilerSpecific/CallingConvention/ThiscallToBorlandRegAdapter.h"
#include "CompilerSpecific/CallingConvention/ThiscallToCdeclAdapter.h"
#include "CompilerSpecific/CompilerHelper.h"

#include "Debugger.h"
#include "ImportHooker.h"
#include "Kirikiri/Kirikiri.h"
#include "CustomTVPXP3ArchiveStream.h"
#include "CxdecHelper.h"
#include "Patcher.h"
