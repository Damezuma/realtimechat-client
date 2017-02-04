#pragma once
#include<wx/wx.h>
#include<memory>
#include"room.h"
#include"member.h"
#include"guidesign.h"
#include "messageballoon.h"
#include "message.h"
class ChannelPage : public MyPanel1
{
public:
	ChannelPage(wxWindow* parent, std::shared_ptr<Room> room);
	wxString GetMessage();
	void UpdateMemberList();
	void ShowSystemMessage(const wxString & msg);
	void ShowMessage(MessageComeChat & message);
	void ProcedureOnEnterNewUser(MessageAboutRoomEvent & message);
	void ProcedureOnLeaveUser(MessageAboutRoomEvent & message);
	void ProcedureOnExitOtherUser(MessageAboutRoomEvent & message);
	void ProcedureOnOtherUserDisconnectServer(MessageAboutRoomEvent & message);
	void EventProcedure(Message & message);
private:
	void OnCharTxtCtrl(wxKeyEvent& event);
	std::shared_ptr<Room> m_room;
};