#pragma once
#include<wx/wx.h>
#include"guidesign.h"
#include <wx/activityindicator.h>
#include "message.h"
#include <wx/msgqueue.h>
#include "channelpage.h"
#include <unordered_map>
template <typename K, typename T>
using Dict = std::unordered_map<K, T>;
class SendMessageThread;
class MainFrame : public MyFrame1
{
public:
	MainFrame() ;
	virtual ~MainFrame()
	{

	}
	void SendEventMessage(Message & message);
	void AddNewChannelPage(std::shared_ptr<Room> room);
	void RemoveChannelPage(const std::string & roomName);
protected:
	void OnClickNewRoom(wxCommandEvent & event) override;
	
	wxMessageQueue<std::string> m_msgQueue;
	Dict<std::string, ChannelPage*> m_roomPages;
	int m_pageId;

};
class Application : public wxApp
{
public:

	virtual bool OnInit() override;
	virtual int OnExit() override;
	void OnCloseMainFrame(wxCloseEvent & event);
	void OnOpenSuccess(wxThreadEvent & event);
	void OnOpenFailed(wxThreadEvent & event);
	void OnRecvOpenSuccess(wxThreadEvent & event);
	void OnRecvOpenFailed(wxThreadEvent & event);
	void OnComeMessage(wxThreadEvent & event);
	void ChatMessage(const std::string & roomName, const std::string & msg);
	void EnterRoom(const std::string & roomName);
	void LeaveRoom(const std::string & roomName);
	std::string GetUserHashId() { return m_hashId; }
private:
	Dict<std::string, std::shared_ptr<Room>> m_rooms;
	wxActivityIndicator * m_indicator = nullptr;
	MainFrame * m_mainFrame = nullptr;
	std::string m_hashId;
	wxMessageQueue<std::string*> m_msgQueue;
	SendMessageThread * m_sendMessageThread = nullptr;
};