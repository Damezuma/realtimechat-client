#include "channelpage.h"
#include "application.h"
#include "utility.hpp"
ChannelPage::ChannelPage(wxWindow * parent, std::shared_ptr<Room> room) : MyPanel1(parent)
{
	this->m_textCtrl1->Connect(ID_TXT_CHAT_MESSAGE_INPUT, wxEVT_CHAR, wxKeyEventHandler(ChannelPage::OnCharTxtCtrl), nullptr, this);
	this->m_room = room;
}
ChannelPage::~ChannelPage()
{
	auto * app = dynamic_cast<Application*>(wxApp::GetInstance());
	app->LeaveRoom(m_room->GetName());
}
wxString ChannelPage::GetMessage()
{
	wxString value = m_textCtrl1->GetValue();
	m_textCtrl1->Clear();
	return value;
}
void ChannelPage::UpdateMemberList()
{
	m_listBox1->Clear();
	auto list = m_room->GetMemberList();
	for (auto & it : list)
	{
		m_listBox1->Append(MakeFromUTF8String(it.GetName()));
	}
}
void ChannelPage::ShowSystemMessage(const wxString & msg)
{
	//this->m_listBox2->AppendString(msg);
	SystemMessageBalloon * message = new SystemMessageBalloon(m_scrolledWindow1, wxID_ANY, msg);
	m_scrolledWindow1->GetSizer()->Add(message, 0, wxALIGN_CENTER);

	m_scrolledWindow1->FitInside();
	m_scrolledWindow1->GetSizer()->Layout();
}
void ChannelPage::ShowMessage(MessageComeChat & message)
{
	auto * app = dynamic_cast<Application*>(wxApp::GetInstance());
	std::string hashId = app->GetUserHashId();
	if (message.TestEqualSenderHashId(hashId))
	{
		auto balloon = new MyMessageBalloon(m_scrolledWindow1, wxID_ANY, message.GetTime(), MakeFromUTF8String(message.GetText()));
		m_scrolledWindow1->GetSizer()->Add(balloon, 0, wxALIGN_RIGHT);
	}
	else
	{
		auto balloon = new MessageBalloon(m_scrolledWindow1, wxID_ANY, m_room->FindUserNameWithHashId(hashId), message.GetTime(), MakeFromUTF8String(message.GetText()));
		m_scrolledWindow1->GetSizer()->Add(balloon, 0, wxALIGN_LEFT);
	}
	m_scrolledWindow1->FitInside();
	m_scrolledWindow1->GetSizer()->Layout();
	m_scrolledWindow1->Scroll(0, m_scrolledWindow1->GetSizer()->GetSize().y);
}
void ChannelPage::ProcedureOnEnterNewUser(MessageAboutRoomEvent & message)
{
	m_room->SetMemberList(message.GetMemberList());
	UpdateMemberList();
	wxString name = MakeFromUTF8String(m_room->FindUserNameWithHashId(message.GetSender()));
	ShowSystemMessage(wxString::Format(wxT("%s님이 입장하였습니다."), name));
}
void ChannelPage::ProcedureOnLeaveUser(MessageAboutRoomEvent & message)
{
	
	wxString name = MakeFromUTF8String(m_room->FindUserNameWithHashId(message.GetSender()));
	ShowSystemMessage(wxString::Format(wxT("%s님이 나가셨습니다."), name));
	m_room->SetMemberList(message.GetMemberList());
	UpdateMemberList();
}
void ChannelPage::ProcedureOnExitOtherUser(MessageAboutRoomEvent & message)
{
	wxString name = MakeFromUTF8String(m_room->FindUserNameWithHashId(message.GetSender()));
	ShowSystemMessage(wxString::Format(wxT("%s님이 프로그램을 종료하였습니다."), name));
	m_room->SetMemberList(message.GetMemberList());
	UpdateMemberList();
}
void ChannelPage::ProcedureOnOtherUserDisconnectServer(MessageAboutRoomEvent & message)
{
	wxString name = MakeFromUTF8String(m_room->FindUserNameWithHashId(message.GetSender()));
	ShowSystemMessage(wxString::Format(wxT("%s님에게 문제가 발생하여 서버와 연결이 끊겼습니다."), name));
	m_room->SetMemberList(message.GetMemberList());
	UpdateMemberList();
}
void ChannelPage::EventProcedure(Message & message)
{
	switch (message.GetMessageType())
	{
	case MessageType::ChatMessage:
		ShowMessage(dynamic_cast<MessageComeChat&>(message));
		break;
	case MessageType::ComeNewMemberInRoom:
		ProcedureOnEnterNewUser(dynamic_cast<MessageAboutRoomEvent&>(message));
		break;
	case MessageType::LeaveMemberFromRoom:
		ProcedureOnLeaveUser(dynamic_cast<MessageAboutRoomEvent&>(message));
		break;
	case MessageType::ExitServer:
		ProcedureOnExitOtherUser(dynamic_cast<MessageAboutRoomEvent&>(message));
		break;
	case MessageType::DisconnectedServer:
		ProcedureOnOtherUserDisconnectServer(dynamic_cast<MessageAboutRoomEvent&>(message));
		break;
	}
}
void ChannelPage::OnCharTxtCtrl(wxKeyEvent& event)
{
	bool needSkip = false;
	needSkip = (event.GetKeyCode() != wxKeyCode::WXK_RETURN) || (event.GetKeyCode() == wxKeyCode::WXK_RETURN && event.ShiftDown());
	
	if (needSkip)
	{
		event.Skip();
	}
	else
	{
		auto * app = dynamic_cast<Application*>(wxApp::GetInstance());
		app->ChatMessage(m_room->GetName(), MakeFromWxString(m_textCtrl1->GetValue()));
		m_textCtrl1->Clear();
	}
}