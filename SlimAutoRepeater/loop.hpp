#include "common.hpp"
#include "platform/platform.hpp"

struct GlobalInfo
{
    static GlobalInfo* I()
    {
        static GlobalInfo* _ins = nullptr;
        return _ins ? _ins : (_ins = new GlobalInfo());
    }

    GlobalInfo() :
        recording(false),
        point(),
        chain()
    {
        chain = make_shared<slim::element_chain>();
    }

    bool recording = false;
    slim::point point;
    shared_ptr<slim::element_chain> chain;
};

namespace slim
{

bool SlimLoop();

}
