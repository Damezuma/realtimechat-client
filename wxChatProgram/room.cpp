#include "room.h"

Room::Room(const wxString & name, const std::vector<Member>& memberList)
{
	m_name = name;
	m_memberList = memberList;
}
