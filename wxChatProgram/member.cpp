#include "member.h"

Member::Member()
{
}

Member::Member(const std::string & name, const std::string & hashcode)
{
	m_name = name;
	m_hashcode = hashcode;
}

Member::Member(std::string && name, std::string && hashcode)
{
	m_name = std::move(name);
	m_hashcode = std::move(hashcode);
}

bool Member::HaveSameHashcode(const std::string & hashcode) const
{
	return m_hashcode == hashcode;
}
