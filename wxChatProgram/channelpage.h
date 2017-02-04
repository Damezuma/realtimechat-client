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
	ChannelPage(wxWindow* parent);
	wxString GetMessage();
	void UpdateMemberList(const std::vector<wxString> & list);
	void ShowSystemMessage(const wxString & msg);
	void ShowMessage(const wxDateTime & time, const wxString & sender, const wxString & msg);
	void ShowMyMessage(const wxDateTime & time, const wxString & msg);
	void EventProcedure(Message & message);
private:

};