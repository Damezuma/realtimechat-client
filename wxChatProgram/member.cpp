#include "member.h"

Member::Member()
{
}

Member::Member(const wxString & name, const wxString & hashcode)
{
	m_name = name;
	m_hashcode = hashcode;
}

bool Member::HaveSameHashcode(const wxString & hashcode) const
{
	return m_hashcode == hashcode;
}
