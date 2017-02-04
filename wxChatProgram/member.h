#pragma once
#include<vector>
#include<wx/wx.h>
#include<string>
#include<utility>
class Member
{
public:
	Member();
	Member(const std::string & name, const std::string & hashcode);
	Member(std::string && name, std::string && hashcode);
	Member(Member&& moveObj) = default;
	Member(const Member & obj);
	bool TestSameHashId(const std::string & hashcode) const;
	Member & operator = (const Member & obj)
	{
		this->m_hashcode = obj.m_hashcode;
		this->m_name = obj.m_name;
		return *this;
	}
	std::string GetHashcode() const
	{
		return m_hashcode;
	}
	std::string GetName() const 
	{
		return m_name;
	}
private:
	std::string m_hashcode;
	std::string m_name;
};