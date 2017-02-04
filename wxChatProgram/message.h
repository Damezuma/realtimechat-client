#pragma once
#include<wx/wx.h>
#include"member.h"
#include<vector>
enum class MessageType
{
	InvalidMessage,
	ChatMessage,
	ComeNewMemberInRoom,
	LeaveMemberFromRoom
};
class Message
{
public:
	Message() = default;
	virtual ~Message() = default;
	Message(MessageType type, const wxString & room, const wxString& sender,const wxString & time)
	{
		m_sender = sender;
		m_room = room;
		m_type = type;
		m_time.ParseRfc822Date(time);
	}
	MessageType GetMessageType() { return m_type; }
	wxString GetSender() { return m_sender; }
	wxString GetRoom() { return m_room; }
	wxDateTime GetTime() { return m_time; }
protected:
	wxString m_sender;
	wxString m_room;
	wxDateTime m_time;
	MessageType m_type;
};
class IHasMemberList
{
public:
	virtual ~IHasMemberList() = default;
	virtual std::vector<Member> GetMemberList() = 0;
};

class MessageAboutRoomEvent : public IHasMemberList, public Message
{
public:
	MessageAboutRoomEvent(
		MessageType type,
		const wxString & room,
		const wxString& sender,
		const wxString & time,
		std::vector<Member> && memberList)
		:Message(type, room, sender, time),m_memberList(memberList)

	{
		
	}
	virtual std::vector<Member> GetMemberList() override
	{
		return m_memberList;
	}
private:
	std::vector<Member> m_memberList;
};
class MessageComeChat : public Message
{
public:
	MessageComeChat(
		const wxString & room,
		const wxString& sender,
		const wxString & text,
		const wxString & time)
		:Message(MessageType::ChatMessage, room, sender, time), m_text(text)
	{
	}
	wxString GetText() { return m_text; }
private:
	wxString m_text;
};