#pragma once
#include<vector>
#include<wx/wx.h>
class Member
{
public:
	Member();
	Member(const wxString & name, const wxString & hashcode);
	bool HaveSameHashcode(const wxString & hashcode) const;
	wxString GetHashcode() const
	{
		return m_hashcode;
	}
	wxString GetName() const 
	{
		return m_name;
	}
private:
	wxString m_hashcode;
	wxString m_name;
};