#include "common.hpp"

void UiAutomationTest(EleProperties& props);
void MsaaTest(EleProperties& props);

int main()
{
    ShowWindow(GetConsoleWindow(), SW_HIDE);
    if (FAILED(CoInitialize(NULL)))
    {
        return 1;
    }

    while (1)
    {
        Sleep(2000);

        EleProperties props_1;
        UiAutomationTest(props_1);

        EleProperties props_2;
        MsaaTest(props_2);

        printf("");
    }

    system("pause");
    return 0;
}
