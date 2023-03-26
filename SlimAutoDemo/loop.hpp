#include "common.hpp"
#include "platform/platform.hpp"

struct SlimDemoInfo
{
    int test;
    vector<pair<int, wstring>> chain; // child_idx of parent : name
};

bool SlimLoop(SlimDemoInfo& info);
