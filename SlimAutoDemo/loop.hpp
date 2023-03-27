#include "common.hpp"
#include "platform/platform.hpp"

struct SlimDemoInfo
{
    string test;
    slim::point point;
    vector<string> chain;
};

bool SlimLoop(SlimDemoInfo& info);
