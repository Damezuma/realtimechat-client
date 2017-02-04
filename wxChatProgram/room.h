#pragma once
#include<wx/wx.h>
#include"member.h"
#include<vector>
class Room
{
public:
	Room() = default;
	Room(const wxString & name, const std::vector<Member> & memberList);
private:
	wxString m_name;
	std::vector<Member> m_memberList;
};