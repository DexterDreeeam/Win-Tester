#include "platform/platform.hpp"

int main()
{
    Sleep(2000);
    POINT pt;
    GetCursorPos(&pt);
    slim::platform::UpdateDesktopWnds();
    auto ws = slim::platform::GetWnds();
    slim::platform::Test(pt);
    slim::platform::GetElementChainInDesktop(pt);
    return 0;
}
