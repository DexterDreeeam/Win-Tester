#include "automation_msaa.hpp"

MsaaElement::MsaaElement(IAccessible* acc, LONG child) :
    acc(acc),
    child_id(child),
    child_idx(-1),
    rect(),
    props()
{
    HRESULT hr;
    BSTR bstr;

    hr = acc->get_accName(this->ChildVar(), &bstr);
    this->SetProperty({ "Name", ToString(bstr) }, false);

    hr = acc->get_accValue(this->ChildVar(), &bstr);
    this->SetProperty({ "Value", ToString(bstr) });

    hr = acc->get_accDescription(this->ChildVar(), &bstr);
    this->SetProperty({ "Description", ToString(bstr) });

    if (child_id == CHILDID_SELF && this->GetProperty("Name").value.size() == 0)
    {
        this->RepresentByParent();
    }

    this->LoadRect();
}

MsaaElement::MsaaElement(std::shared_ptr<MsaaElement> rhs) :
    acc(rhs->acc),
    child_id(rhs->child_id),
    child_idx(rhs->child_idx),
    rect(rhs->rect),
    props(rhs->props)
{
    this->acc->AddRef();
}

MsaaElement::~MsaaElement()
{
    acc->Release();
}

ImplementType MsaaElement::WinImplementType()
{
    return Msaa;
}

int MsaaElement::CountProperty()
{
    return (int)props.size();
}

ElementProperty MsaaElement::GetProperty(const std::string& name)
{
    if (props.count(name) == 0)
    {
        return {};
    }
    return props[name];
}

ElementProperty MsaaElement::GetProperty(int index)
{
    if (index < 0 || index >= CountProperty())
    {
        return {};
    }

    auto itr = props.begin();
    int i = 0;
    while (i++ < index)
    {
        ++itr;
    }
    return itr->second;
}

void MsaaElement::SetProperty(const ElementProperty& prop, bool discardEmpty)
{
    if (discardEmpty && prop.name.length() == 0)
    {
        return;
    }
    props[prop.name] = prop;
}

std::shared_ptr<Element> MsaaElement::Parent()
{
    if (child_id != CHILDID_SELF)
    {
        return std::make_shared<MsaaElement>(acc);
    }

    IDispatch* pDispatch = nullptr;
    IAccessible* pAcc = nullptr;

    if (FAILED(acc->get_accParent(&pDispatch)) || !pDispatch)
    {
        return nullptr;
    }

    if (FAILED(pDispatch->QueryInterface(&pAcc)) || !pAcc)
    {
        return nullptr;
    }

    return std::make_shared<MsaaElement>(pAcc);
}

std::wstring MsaaElement::Path()
{
    auto p = this->Parent();
    auto pPath = p ? p->Path() : std::wstring(L"");
    return pPath + L"---" + this->PathSection();
}

ElementRect MsaaElement::Rect()
{
    return rect;
}

VARIANT MsaaElement::ChildVar()
{
    VARIANT s;
    s.vt = VT_I4;
    s.lVal = this->child_id;
    return s;
}

void MsaaElement::RepresentByParent()
{
    IDispatch* pDispatch = nullptr;
    IAccessible* parentAcc = nullptr;

    if (FAILED(this->acc->get_accParent(&pDispatch)) || !pDispatch)
    {
        return;
    }

    if (FAILED(pDispatch->QueryInterface(&parentAcc)) || !parentAcc)
    {
        return;
    }

    LONG obtain = 0;
    long cnt = 0;
    if (FAILED(parentAcc->get_accChildCount(&cnt)) || cnt == 0)
    {
        return;
    }

    std::vector<VARIANT> vars;
    vars.resize(cnt);
    if (FAILED(AccessibleChildren(parentAcc, 0L, cnt, vars.data(), &obtain)) || obtain <= 0)
    {
        return;
    }

    for (int i = 0; i < obtain; ++i)
    {
        IDispatch* pDisp = nullptr;
        IAccessible* pChild = nullptr;
        VARIANT var;
        var.vt = VT_I4;
        var.lVal = i + 1;

        if (FAILED(parentAcc->get_accChild(var, &pDisp)))
        {
            continue;
        }

        if (FAILED(pDisp->QueryInterface(&pChild)))
        {
            continue;
        }

        if (pChild == this->acc)
        {
            this->acc = parentAcc;
            this->child_id = var.lVal;
            this->child_idx = i;
            return;
        }
        pDisp->Release();
    }

    //for (int i = 0; i < obtain; ++i)
    //{
    //    VARIANT vtChild = vars[i];
    //    if (vtChild.vt == VT_DISPATCH)
    //    {
    //        IDispatch* pDisp = vtChild.pdispVal;
    //        IAccessible* pChild = NULL;
    //        if (FAILED(pDisp->QueryInterface(&pChild)))
    //        {
    //            continue;
    //        }

    //        if (pChild == this->acc)
    //        {
    //            this->acc = parentAcc;
    //            this->child_id = vtChild.lVal;
    //            this->child_idx = i;
    //            return;
    //        }
    //        pDisp->Release();
    //    }
    //}
}

std::wstring MsaaElement::PathSection()
{
    if (this->GetProperty("Name").value.size() > 0)
    {
        return this->GetProperty("Name").value;
    }
    else
    {
        std::wstring str = L"";
        str += '[';
        str += std::to_wstring(this->child_id);
        str += ']';
        return str;
    }
}

bool MsaaElement::LoadRect()
{
    HRESULT hr;
    long l, t, w, h;
    hr = acc->accLocation(&l, &t, &w, &h, ChildVar());
    if (FAILED(hr))
    {
        return false;
    }

    this->rect = ElementRect({ l, t, l + w, t + h });
    return true;
}

MsaaAutomation::~MsaaAutomation()
{
}

std::shared_ptr<Element> MsaaAutomation::PointElement(int x, int y)
{
    return this->PointElement({ x, y });
}

std::shared_ptr<Element> MsaaAutomation::CursorElement()
{
    POINT pt;
    if (!GetCursorPos(&pt))
    {
        return {};
    }

    return this->PointElement(pt);
}

std::shared_ptr<Element> MsaaAutomation::PointElement(POINT pt)
{
    HRESULT hr;
    IAccessible* acc;
    VARIANT var;

    hr = AccessibleObjectFromPoint(pt, &acc, &var);
    if (FAILED(hr))
    {
        return {};
    }

    return std::make_shared<MsaaElement>(acc, var.lVal);
}
