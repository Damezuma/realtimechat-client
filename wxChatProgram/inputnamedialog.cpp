#include "inputnamedialog.h"

NameInputDialog::NameInputDialog() : MyDialog1(nullptr,wxID_ANY,wxT("이름 입력창"))
{

}

void NameInputDialog::OnClickCloseButton(wxCommandEvent & event)
{
	if (this->IsModal())
	{
		this->EndModal(wxID_CLOSE);
	}
	else
	{
		this->EndDialog(wxID_CLOSE);
	}
}

void NameInputDialog::OnClickOKButton(wxCommandEvent & event)
{
	m_name = m_textCtrl2->GetValue();
	if (this->IsModal())
	{
		this->EndModal(wxID_OK);
	}
	else
	{
		this->EndDialog(wxID_OK);
	}
}
