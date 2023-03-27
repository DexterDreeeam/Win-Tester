#include "loop.hpp"

void Dbg(shared_ptr<slim::element> root)
{
    logger l;

    slim::IterateElement(
        root,
        [&](auto e)
        {
            e->LoadProperty();
            string space = "";
            for (int i = 0; i < e->Depth(); ++i)
            {
                space += "  ";
            }
            l.w(space);
            l.w(e->Identifier());
            l.w("-[" + to_string(e->Area().left) + ", " + to_string(e->Area().right) + "]");
            l.w("-[" + to_string(e->Area().top) + ", " + to_string(e->Area().bottom) + "]");
            l.w_endl();
        });
}

bool SlimLoop(SlimDemoInfo& info)
{
    try
    {
        POINT pt;
        GetCursorPos(&pt);

        auto elementChain =  slim::platform::I()->ElementChainByPoint(slim::point(pt));
        if (elementChain)
        {
            info.test = elementChain->FriendlyIdentifier();
        }


        //auto root = slim::platform::I()->Diagram();

        //info.point.x = pt.x;
        //info.point.y = pt.y;

        //if (!root->Area().Inside(pt))
        //{
        //    return false;
        //}

        //if (DPressd())
        //{
        //    Dbg(root);
        //}

        //// e : e_parent_idx_in_cand
        //vector<pair<shared_ptr<slim::element>, int>> elms = { { root, -1 } };
        //vector<pair<shared_ptr<slim::element>, int>> cand = {};

        //while (elms.size())
        //{
        //    auto e = elms.back();
        //    elms.pop_back();
        //    cand.push_back(e);
        //    int parent_idx = (int)cand.size() - 1;
        //    for (int i = e.first->SubCount() - 1; i >= 0; --i)
        //    {
        //        auto s = e.first->Sub(i);
        //        elms.push_back({ s, parent_idx });
        //    }
        //}

        //double maxGrade = -1;
        //int maxElmIdx = -1;
        //for (int i = 0; i < cand.size(); ++i)
        //{
        //    double iG = cand[i].first->InteractGrade(pt);
        //    if (iG > maxGrade)
        //    {
        //        maxGrade = iG;
        //        maxElmIdx = i;
        //    }
        //}

        //if (maxElmIdx == -1)
        //{
        //    return false;
        //}

        //vector<shared_ptr<slim::element>> chain;
        //int parent = cand[maxElmIdx].second;
        //chain.push_back(cand[maxElmIdx].first);
        //while (parent >= 0)
        //{
        //    auto p = cand[parent];
        //    parent = p.second;
        //    chain.push_back(p.first);
        //}

        //info.chain.clear();
        //for (auto e : chain)
        //{
        //    e->LoadProperty();
        //    info.chain.push_back(e->FriendlyIdentifier());
        //}

        //info.test = (int)cand.size();
        return true;
    }
    catch (...)
    {
        return false;
    }
}
