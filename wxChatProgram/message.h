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
	Message(MessageType type, std::string && room, std::string&& sender,const std::string& time)
	{
		m_sender =std::move(sender);
		m_room = std::move(room);
		m_type = std::move(type);
		m_time.ParseRfc822Date(time);
	}
	MessageType GetMessageType() { return m_type; }
	std::string GetSender() { return m_sender; }
	std::string GetRoom() { return m_room; }
	wxDateTime GetTime() { return m_time; }
protected:
	std::string m_sender;
	std::string m_room;
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
		std::string&& room,
		std::string&& sender,
		std::string& time,
		std::vector<Member> && memberList)
		:Message(type, std::move(room), std::move(sender), time),m_memberList(memberList)

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
		std::string&& room,
		std::string&& sender,
		std::string&& text,
		const std::string & time)
		:Message(MessageType::ChatMessage,std::move(room), std::move(sender),time)
	{
		m_text = std::move(text);
	}
	std::string GetText() { return m_text; }
private:
	std::string m_text;
};