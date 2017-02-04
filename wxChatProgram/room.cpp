#include "room.h"

Room::Room(wxString && name, std::vector<Member> && memberList)
{
	m_name =std::move(name);
	m_memberList = std::move(memberList);
}

std::string Room::FindUserNameWithHashId(const std::string & hashId)
{
	for (auto & member : m_memberList)
	{
		if (member.TestSameHashId(hashId))
		{
			return member.GetName();
		}
	}
	return std::string();
}
