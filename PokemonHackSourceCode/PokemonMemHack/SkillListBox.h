#pragma once

/////////////////////////////////////////////////
// Usage:
// //Make sure g_MemRom is ready!
// WORD wSkillId = ...;
// AddString((LPCTSTR *)(wSkillId));

/////////////////////////////////////////////////
// CSkillListBox
class CSkillListBox : public CListBox
{
	DECLARE_DYNAMIC(CSkillListBox)

public:
	CSkillListBox();
	virtual ~CSkillListBox();

	// owner draw implementations
	void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	int CompareItem(LPCOMPAREITEMSTRUCT lpCompareItemStruct);

protected:
	DECLARE_MESSAGE_MAP()
};


