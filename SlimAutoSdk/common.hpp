#pragma once

#include <windows.h>
#include <UIAutomation.h>
#include <UIAutomationClient.h>

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <memory>
#include <mutex>
#include <thread>
#include <algorithm>
#include <fstream>
#include <locale>
#include <math.h>

using namespace std;

#include "utils/dbg.hpp"
#include "utils/log.hpp"
#include "utils/str.hpp"

#define Tell(e)   { if (FAILED(hr)) { throw e; } }
#define Rels(ptr)  { if (ptr) { ptr->Release(); ptr = nullptr; } }
