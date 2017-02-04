#pragma once
#include<wx/wx.h>
#include<string>
inline wxString MakeFromUTF8String(const std::string & str)
{
	return wxString::FromUTF8(str.c_str());
}
inline std::string MakeFromWxString(const wxString & str)
{
	return std::string(str.ToUTF8().data());
}