#pragma once

#include <windows.h>
#include <UIAutomation.h>
#include <UIAutomationClient.h>

#define Tell(e)   { if (FAILED(hr)) { throw e; } }
#define Rels(ptr)  { if (ptr) { ptr->Release(); ptr = nullptr; } }

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <memory>
#include <mutex>
#include <thread>

using namespace std;
