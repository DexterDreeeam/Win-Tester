#include "common.hpp"

namespace slim
{

bool LaunchShellFilePath(const string& path)
{
    return (LONGLONG)ShellExecuteA(NULL, "open", path.c_str(), NULL, NULL, SW_SHOWNORMAL) > 32;
}

bool LaunchShellAppId(const string& id)
{
    return false;
}

}
