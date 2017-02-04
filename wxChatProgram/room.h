#pragma once
#include<wx/wx.h>
#include"member.h"
#include<vector>

class Room
{
public:
	Room() = default;
	Room(std::string && name, std::vector<Member> && memberList);
	std::string GetName() { return m_name; }
	std::vector<Member> GetMemberList() { return m_memberList; }
	std::string FindUserNameWithHashId(const std::string& hashId);
	void SetMemberList(const std::vector<Member> & memberList) { m_memberList = memberList; }
private:
	std::string m_name;
	std::vector<Member> m_memberList;
};