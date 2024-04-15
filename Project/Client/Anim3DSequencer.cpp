#include "pch.h"
#include "Anim3DSequencer.h"

#include <Engine/CAnimation3D.h>
#include <Engine/CAnimator3D.h>
#include "Animator3DUI.h"
#include <Engine/CAnimation3D.h>
#include <Engine/CAnimator3D.h>

Anim3DSequencer::Anim3DSequencer()
    : m_FrameMin(0)
    , m_FrameMax(0)
    , m_pOwner(nullptr)
    , m_pCurAnimator3D(nullptr)
{
}

Anim3DSequencer::~Anim3DSequencer()
{
}


void Anim3DSequencer::SubAnimName(string Name)
{
    if (std::find(m_vecAnimName.begin(), m_vecAnimName.end(), Name) != m_vecAnimName.end())
        m_vecAnimName.erase(std::find(m_vecAnimName.begin(), m_vecAnimName.end(), Name));

}

int Anim3DSequencer::GetAnimNameIndex(string _name)
{
    int idx = -1;
    for (int i = 0; i < m_vecAnimName.size(); ++i)
    {
        if (m_vecAnimName[i] == _name)
        {
            idx = i;
            break;
        }
    }
    return idx;
}

void Anim3DSequencer::SetOwner(Animator3DUI* _pAnimator3D)
{
    m_pOwner = _pAnimator3D;
    m_pCurAnimator3D = _pAnimator3D->GetCurAnimator3D();
}

void Anim3DSequencer::Reset()
{
    vector<string>      vecName;
    vector<AnimItem>    vecItem;
    if (m_pCurAnimator3D)
    {
        m_FrameMin = 0;
        m_FrameMax = m_pCurAnimator3D->GetAnimClip(0).iFrameLength - 1;

        map<wstring, CAnimation3D*> mapAnim = m_pCurAnimator3D->GetAllAnim();

        for (int i = 0; i < m_vecAnimItem.size(); ++i)
        {
            AnimItem item;
            item.mName = m_vecAnimItem[i].mName;
            item.mIdx = i;
            item.mFrameStart = m_vecAnimItem[i].mFrameStart;
            item.mFrameEnd = m_vecAnimItem[i].mFrameEnd;

            vecItem.push_back(item);
            vecName.push_back(item.mName);
        }

    }
    m_vecAnimName = vecName;
    m_vecAnimItem = vecItem;


    /* if (m_pCurAnimator3D)
     {
         m_FrameMin = 0;
         m_FrameMax = m_pCurAnimator3D->GetAnimClip(0).iFrameLength - 1;

         int idx = 0;
         map<wstring, CAnimation3D*> mapAnim = m_pCurAnimator3D->GetAllAnim();
         map<wstring, CAnimation3D*>::iterator iter = mapAnim.begin();
         for (; iter != mapAnim.end(); ++iter)
         {
             wstring wstrName = iter->first;
             string name = string(wstrName.begin(), wstrName.end());
             AnimItem item;
             item.mFrameStart    = iter->second->GetStartFrameIdx();
             item.mFrameEnd      = iter->second->GetEndFrameIdx();
             item.mIdx           = idx;
             item.mName          = name;

             m_vecAnimName.push_back(name);
             m_vecAnimItem.push_back(item);

             idx++;
         }
     }*/
}


void Anim3DSequencer::SubAnimItem(string Name)
{
    int delIdx = 0;
    vector<AnimItem>::iterator iter = m_vecAnimItem.begin();
    for (; iter != m_vecAnimItem.end(); ++iter)
    {
        if (iter->mName == Name)
        {
            m_vecAnimItem.erase(m_vecAnimItem.begin() + delIdx);
            break;
        }
        delIdx++;
    }

    if (m_iEditIdx == delIdx)
        m_iEditIdx = -1;

    return;
}



const char* Anim3DSequencer::GetItemLabel(int index) const
{
    static char tmps[512];
    _snprintf_s(tmps, 512, "[%02d] %s", index, m_vecAnimName[m_vecAnimItem[index].mIdx].c_str());
    return tmps;
}

void Anim3DSequencer::Get(int index, int** start, int** end, int* type, unsigned int* color)
{
    AnimItem& item = m_vecAnimItem[index];
    if (color)
        *color = 0xFFAA8080; // same color for everyone, return color based on type
    if (start)
        *start = &item.mFrameStart;
    if (end)
        *end = &item.mFrameEnd;
    if (type)
        *type = item.mIdx;
}

void Anim3DSequencer::DoubleClick(int index)
{
    int i = 0;

    //if (m_vecAnimItem[index].mExpanded)
    //{
    //    m_vecAnimItem[index].mExpanded = false;
    //    return;
    //}
    //for (auto& item : m_vecAnimItem)
    //    item.mExpanded = false;\
    //m_vecAnimItem[index].mExpanded = !m_vecAnimItem[index].mExpanded;
}

void Anim3DSequencer::BeginEdit(int _index)
{

    m_iEditIdx = _index;

}

void Anim3DSequencer::EndEdit()
{
    if (m_iEditIdx == -1)
        return;

    if (m_pCurAnimator3D != nullptr)
    {
        string AnimName = m_vecAnimName[m_iEditIdx];
        wstring wstrAnimName = wstring(AnimName.begin(), AnimName.end());

        if (m_pCurAnimator3D != nullptr)
        {
            CAnimation3D* pAnim = m_pCurAnimator3D->FindAnim(wstrAnimName);
            pAnim->SetStartFrameIdx(m_vecAnimItem[m_iEditIdx].mFrameStart);
            pAnim->SetEndFrameIdx(m_vecAnimItem[m_iEditIdx].mFrameEnd);
        }


    }

    m_iEditIdx = -1;
}
