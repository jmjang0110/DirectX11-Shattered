#pragma once
#include "ImGui/imgui.h"

#include "ImGuizmo/ImSequencer.h"



struct AnimItem
{
    string mName;
    int mIdx;
    int mFrameStart;
    int mFrameEnd;

    //bool mExpanded;

};

class Anim3DSequencer
    : public ImSequencer::SequenceInterface
{
private:
    class Animator3DUI* m_pOwner;
    class CAnimator3D* m_pCurAnimator3D;

private:
    vector<string>      m_vecAnimName;  // 애니메이션 이름 
    vector<AnimItem>    m_vecAnimItem;  // 애니메이션 

    int                 m_FrameMin;
    int                 m_FrameMax;

    int                 m_iEditIdx;


public:
    void Reset();
    void SetOwner(class Animator3DUI* _pAnimator3D);

    Animator3DUI* GetOwner() { return m_pOwner; }
    int GetEidtIdx() { return m_iEditIdx; }


public:
    // interface with sequencer
    void AddAnimName(string Name) { m_vecAnimName.push_back(Name); }
    void ClearAnimName() { m_vecAnimName.clear(); }
    void ClearAnimItem() { m_vecAnimItem.clear(); }
    void SubAnimName(string Name);
    void SubAnimItem(string Name);
    int GetAnimNameIndex(string _name);


    virtual int GetFrameMin() const { return m_FrameMin; }
    virtual int GetFrameMax() const { return m_FrameMax; }
    virtual int GetItemCount() const { return (int)m_vecAnimItem.size(); }
    virtual int GetItemTypeCount() const { return (int)m_vecAnimName.size(); }

    virtual const char* GetItemTypeName(int typeIndex) const { return m_vecAnimName[typeIndex].c_str(); }
    virtual const char* GetItemLabel(int index) const;
    virtual void Get(int index, int** start, int** end, int* type, unsigned int* color);

    virtual void Add(int type) { m_vecAnimItem.push_back(AnimItem{ NULL, type, 0, 10 }); };
    virtual void Del(int index) { m_vecAnimItem.erase(m_vecAnimItem.begin() + index); }
    virtual void Duplicate(int index) { m_vecAnimItem.push_back(m_vecAnimItem[index]); }

    //virtual size_t GetCustomHeight(int index) { return m_vecAnimItem[index].mExpanded ? 300 : 0; }


    virtual void DoubleClick(int index);     // 더블 클릭 되었을 때 들어오는 함수 

    virtual void BeginEdit(int _index);
    virtual void EndEdit();


    template <typename T>
    T Clamp(T val, T min, T max);

public:
    Anim3DSequencer();
    ~Anim3DSequencer();
    friend class Animator3DUI;
};


template <typename T>
T Anim3DSequencer::Clamp(T val, T min, T max)
{
    if (val <= min)
    {
        val = min;
    }
    if (val >= max)
    {
        val = max;
    }
    return val;
}

