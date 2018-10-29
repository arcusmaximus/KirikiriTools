#pragma once

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <cassert>
#include <exception>
#include <functional>
#include <map>
#include <string>
#include <vector>

#include "../Detours/detours.h"

#include "Common.h"

#include "CodeBuffer.h"
#include "Debugger.h"

#include "Kirikiri/Kirikiri.h"
#include "CxdecHelper.h"
#include "PluginPatcher.h"
