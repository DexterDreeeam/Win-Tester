
inline bool DPressd()
{
    static bool done = false;
    if (done)
    {
        return false;
    }
    if (GetKeyState(0x44) != 0)
    {
        done = true;
        return true;
    }
    return false;
}
