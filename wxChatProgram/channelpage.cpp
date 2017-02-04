#include "channelpage.h"

ChannelPage::ChannelPage(wxWindow * parent) : MyPanel1(parent)
{
}
wxString ChannelPage::GetMessage()
{
	wxString value = m_textCtrl1->GetValue();
	m_textCtrl1->Clear();
	return value;
}
void ChannelPage::UpdateMemberList(const std::vector<wxString> & list)
{
	m_listBox1->Clear();
	wxArrayString arr;
	for (auto & it : list)
	{
		arr.push_back(it);
	}
	m_listBox1->InsertItems(arr, 0);
}
void ChannelPage::ShowSystemMessage(const wxString & msg)
{
	//this->m_listBox2->AppendString(msg);
	SystemMessageBalloon * message = new SystemMessageBalloon(m_scrolledWindow1, wxID_ANY, msg);
	m_scrolledWindow1->GetSizer()->Add(message, 0, wxALIGN_CENTER);

	m_scrolledWindow1->FitInside();
	m_scrolledWindow1->GetSizer()->Layout();
}
void ChannelPage::ShowMessage(const wxDateTime & time, const wxString & sender, const wxString & msg)
{
	MessageBalloon * message = new MessageBalloon(m_scrolledWindow1, wxID_ANY, sender, time, msg);
	m_scrolledWindow1->GetSizer()->Add(message, 0, wxALIGN_LEFT);

	m_scrolledWindow1->FitInside();
	m_scrolledWindow1->GetSizer()->Layout();
}
void ChannelPage::ShowMyMessage(const wxDateTime & time, const wxString & msg)
{
	MyMessageBalloon * myMessage = new MyMessageBalloon(m_scrolledWindow1, wxID_ANY, time, msg);
	m_scrolledWindow1->GetSizer()->Add(myMessage, 0, wxALIGN_RIGHT);

	m_scrolledWindow1->FitInside();
	m_scrolledWindow1->GetSizer()->Layout();
	m_scrolledWindow1->Scroll(0, m_scrolledWindow1->GetSizer()->GetSize().y);
}