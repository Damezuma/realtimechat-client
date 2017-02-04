#pragma once
#include<vector>
#include<wx/wx.h>
#include<string>
class Member
{
public:
	Member();
	Member(const std::string & name, const std::string & hashcode);
	Member(std::string && name, std::string && hashcode);
	Member(Member&& moveObj) = default;
	Member& operator = (Member && moveObj)
	{
		this->m_hashcode = std::move(moveObj.GetHashcode);
		this->m_name = std::move(moveObj.m_name);
		return *this;
	}
	bool HaveSameHashcode(const std::string & hashcode) const;
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