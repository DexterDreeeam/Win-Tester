#include "loop.hpp"

bool SlimLoop(SlimDemoInfo& info)
{
    try
    {
        auto root = slim::platform::I()->Diagram();

        POINT pt;
        GetCursorPos(&pt);

        if (!root->Area().Inside(pt))
        {
            return false;
        }

        if (DPressd())
        {
            logger l("dbg.txt");
            slim::IterateElement(
                root,
                [&](auto e)
                {
                    e->LoadProperty();
                    string line = "";
                    for (int i = 0; i < e->Depth(); ++i)
                    {
                        line += "  ";
                    }
                    line += "[" + to_string(e->SubIdx()) + "] ";
                    line += WtoA(e->Proerty("Name"));
                    line += " <" + to_string(e->Area().left) + ", " + to_string(e->Area().right) + ">";
                    line += " - ";
                    line += " <" + to_string(e->Area().top) + ", " + to_string(e->Area().bottom) + ">";
                    l.write(line);
                });
        }

        // e : e_parent_idx_in_cand
        vector<pair<shared_ptr<slim::element>, int>> elms = { { root, -1 } };
        vector<pair<shared_ptr<slim::element>, int>> cand = {};

        while (elms.size())
        {
            auto e = elms.back();
            e.first->LoadProperty();

            elms.pop_back();
            cand.push_back(e);
            int parent_idx = (int)cand.size() - 1;
            for (int i = e.first->SubCount() - 1; i >= 0; --i)
            {
                auto s = e.first->Sub(i);
                if (s->Area().Inside(pt))
                {
                    elms.push_back({ s, parent_idx });
                }
            }
        }

        auto cand2 = cand;

        sort(
            cand2.begin(), cand2.end(),
            [=](auto x, auto y)
            {
                int xd = x.first->Depth();
                int yd = y.first->Depth();
                float xc = x.first->Area().center(pt);
                float yc = y.first->Area().center(pt);
                return tie(xd, xc) > tie(yd, yc);
            });

        vector<shared_ptr<slim::element>> chain;
        int parent = cand2.front().second;
        chain.push_back(cand2.front().first);
        while (parent >= 0)
        {
            auto p = cand[parent];
            parent = p.second;
            chain.push_back(p.first);
        }

        info.chain.clear();
        for (auto e : chain)
        {
            e->LoadProperty();
            info.chain.push_back({ e->SubIdx(), e->Proerty("Name") });
        }

        info.test = (int)cand.size();
        return true;
    }
    catch (...)
    {
        return false;
    }
}
