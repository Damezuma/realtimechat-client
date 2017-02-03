#pragma once
#include "guidesign.h"
class NameInputDialog : public MyDialog1
{
public:
	NameInputDialog();
	wxString GetName() { return m_name; }
protected:
	virtual void OnClickCloseButton(wxCommandEvent& event)override;
	virtual void OnClickOKButton(wxCommandEvent& event)override;
	wxString m_name;
};