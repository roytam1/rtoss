#pragma once


// CSkillComboBox

class CSkillComboBox : public CComboBox
{
	DECLARE_DYNAMIC(CSkillComboBox)

public:
	CSkillComboBox();
	virtual ~CSkillComboBox();

	// owner draw implementations
	void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	int CompareItem(LPCOMPAREITEMSTRUCT lpCompareItemStruct);

protected:
	DECLARE_MESSAGE_MAP()
};


