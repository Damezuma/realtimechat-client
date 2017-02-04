#include "application.h"
#include <wx/wx.h>
#include "guidesign.h"
#include <wx/socket.h>
#include <wx/protocol/protocol.h>
#include <thread>
#include <wx/msgqueue.h>
#include "json.hpp"
#include <wx/activityindicator.h>
#include <wx/datetime.h>
#include "messageballoon.h"
#include "inputnamedialog.h"
#include "member.h"
#include <unordered_map>
#include <memory>
#include "message.h"
#include "channelpage.h"
#include "utility.hpp"
const char * SERVER_IP = "localhost";

wxDECLARE_EVENT(wxEVT_COME_MESSAGE, wxThreadEvent);
wxDEFINE_EVENT(wxEVT_COME_MESSAGE, wxThreadEvent);

wxDECLARE_EVENT(wxEVT_TRABSFER_OPEN_SUCCESS, wxThreadEvent);
wxDEFINE_EVENT(wxEVT_TRABSFER_OPEN_SUCCESS, wxThreadEvent);

wxDECLARE_EVENT(wxEVT_TRABSFER_OPEN_FAILED, wxThreadEvent);
wxDEFINE_EVENT(wxEVT_TRABSFER_OPEN_FAILED, wxThreadEvent);

wxDECLARE_EVENT(wxEVT_RECV_TRABSFER_CREATE_SUCCESS, wxThreadEvent);
wxDEFINE_EVENT(wxEVT_RECV_TRABSFER_CREATE_SUCCESS, wxThreadEvent);

wxDECLARE_EVENT(wxEVT_RECV_TRABSFER_CREATE_FAILED, wxThreadEvent);
wxDEFINE_EVENT(wxEVT_RECV_TRABSFER_CREATE_FAILED, wxThreadEvent);

void MainFrame::SendEventMessage(Message & message)
{
	auto it = m_roomPages.find(message.GetRoom());
	if (it != m_roomPages.end())
	{
		it->second->EventProcedure(message);
	}
}

void MainFrame::AddNewChannelPage(std::shared_ptr<Room> room)
{
	auto * page = new ChannelPage(m_auinotebook1, room);
	std::string roomName = room->GetName();
	m_auinotebook1->AddPage(page, MakeFromUTF8String(roomName));
	this->m_roomPages.insert(std::make_pair(roomName, page));
}

void MainFrame::OnClickNewRoom(wxCommandEvent & event)
{
	std::string roomName = MakeFromWxString( m_textCtrl3->GetValue());
	auto * app = dynamic_cast<Application*>(wxApp::GetInstance());
	app->EnterRoom(roomName);
}


class SendMessageThread : public wxThreadHelper
{
public:
	SendMessageThread(wxMessageQueue<std::string> * msgQueue,const std::string& name)
	{
		m_name = name;
		m_msgQueue = msgQueue;
		client = nullptr;
	}
	~SendMessageThread()
	{
		if (client != nullptr)
		{
			char ss[] = "{\"type\":\"EXIT\",\"value\":\"\",\"room\":\"\"}\n";
			client->Write(ss, 38);
			client->Close();
			delete client;
		}
	}
	bool Init()
	{
		if (client != nullptr)
		{
			return true;
		}
		wxIPV4address ipv4addr;
		ipv4addr.Hostname(SERVER_IP);
		ipv4addr.Service(2016);
		client = new wxSocketClient();

		if (client->Connect(ipv4addr) == false)
		{
			delete client;
			client = nullptr;
			return false;
		}
		nlohmann::json object;
		if (m_name.length() != 0)
		{
			object["name"] =this-> m_name;
		}
		else
		{
			object["name"] = nullptr;
		}
		

		std::string s = object.dump();
		s.append("\n");
		long writeSize = 0;
		while (writeSize != s.length())
		{
			writeSize += client->Write(s.c_str() + writeSize, s.length() - writeSize).LastWriteCount();
		}
		s.clear();
		long readByteSize = 0;
		unsigned char readBytes[1024] = { 0 };
		std::vector<unsigned char> buffer;
		bool isEndMsg = false;
		while (isEndMsg == false)
		{
			readByteSize = client->Read(readBytes, 1024).LastReadCount();
			if (readByteSize <= 0)
			{
				
				if (client->Error())
				{
					wxSocketError error = client->LastError();
					return false;
				}
			}
			for (long i = 0; i < readByteSize; i++)
			{
				if (readBytes[i] == '\n' && isEndMsg == false)
				{
					isEndMsg = true;
				}
				else if (isEndMsg == false)
				{
					s.push_back(readBytes[i]);
				}
				else
				{
					buffer.push_back(readBytes[i]);
				}
			}
		}
		object = nlohmann::json::parse(s.c_str());
		std::string name = object.value("name", "");
		std::string hash = object.value("id", "");
		m_hash = wxString::FromUTF8(hash.c_str());
		wxThreadEvent * event = new wxThreadEvent(wxEVT_TRABSFER_OPEN_SUCCESS);
		event->SetString(wxString::FromUTF8(hash.c_str()));
		wxApp::GetInstance()->QueueEvent(event);
		return true;
	}
	virtual void * Entry() override
	{
		if (Init() == false)
		{
			wxThreadEvent * event = new wxThreadEvent(wxEVT_TRABSFER_OPEN_FAILED);
			wxApp::GetInstance()->QueueEvent(event);
			return nullptr;
		}
		else
		{

		}
		std::string msg;
		while (m_msgQueue->ReceiveTimeout(1000, msg) == wxMSGQUEUE_NO_ERROR)
		{
			msg.push_back('\n');
			long writeSize = 0;
			while (writeSize != msg.length())
			{
				writeSize += client->Write(msg.c_str() + writeSize, msg.length() - writeSize).LastWriteCount();
			}
		}
		return nullptr;
	}
private:
	std::string m_name;
	wxSocketClient* client;
	wxMessageQueue<std::string> * m_msgQueue;
	std::string m_hash;
};
class RecvThread : public wxThread
{

public:
	RecvThread(const std::string & hash)
	{
		m_hashId = hash;
	}
	virtual void * Entry() override
	{
		wxSocketClient* client =new wxSocketClient();
		wxIPV4address ipv4addr;
		ipv4addr.Hostname(SERVER_IP);
		ipv4addr.Service(2017);
		
		std::string sendValue = m_hashId;
		if (client->Connect(ipv4addr))
		{
			sendValue.append("\n");
			long writeSize = 0;
			while (writeSize != sendValue.length())
			{
				writeSize += client->Write(sendValue.c_str() + writeSize, sendValue.length() - writeSize).LastWriteCount();
			}

			std::vector<unsigned char> buffer;
			int step = 0;
			while (true)
			{
				long readByteSize = 0;
				unsigned char readBytes[1024] = { 0 };

				bool isEndMsg = false;
				std::string msg;
				for (unsigned char ch : buffer)
				{
					msg.push_back(ch);
				}
				buffer.clear();
				while (isEndMsg == false)
				{
					readByteSize = client->Read(readBytes, 1024).LastReadCount();
					if (readByteSize <= 0)
					{
						if (client->Error())
						{
							client->Destroy();
							return nullptr;
						}
					}
					for (long i = 0; i < readByteSize; i++)
					{
						if (readBytes[i] == '\n' && isEndMsg == false)
						{
							isEndMsg = true;
						}
						else if (isEndMsg)
						{
							buffer.push_back(readBytes[i]);
						}
						else
						{
							msg.push_back(readBytes[i]);
						}
					}
				}
				
				switch (step)
				{
				case 0:
					if (m_hashId == msg)
					{
						wxThreadEvent * e = new wxThreadEvent(wxEVT_RECV_TRABSFER_CREATE_SUCCESS);
						wxApp::GetInstance()->QueueEvent(e);
					}
					step++;
					break;
				case 1:
					ParseMessage(msg);
					break;
				}
			}
		}
		return nullptr;
	}
private:
	void ParseMessage(const std::string & msg)
	{
		Message * message = nullptr;
		nlohmann::json obj = nlohmann::json::parse(msg.c_str());
		std::string type = obj.value("type", "");
		std::string room = obj.value("room", "");
		std::string sender = obj.value("sender", "");
		std::string time = obj.value("time", "");
		if (type == "CHAT_SEND")
		{
			std::string text = obj.value("text", "");
			message = new MessageComeChat(
				std::move(room),
				std::move(sender),
				std::move(text),
				time);
		}
		else if (type == "ENTER_NEW_MEMBER_IN_ROOM")
		{
			std::vector<Member> memberlist;	
			nlohmann::json list = obj["members"];
			for (auto & it : list)
			{
				std::string name = it.value("name","");
				std::string hash_id = it.value("hash_id","");
				memberlist.push_back(Member(std::move(name),std::move(hash_id)));
			}
			message = new MessageAboutRoomEvent(
				MessageType::ComeNewMemberInRoom,
				std::move(room),
				std::move(sender),
				time,
				std::move(memberlist)
				);
		}
		else if (type == "MEMBER_GET_OUT_ROOM")
		{
			std::vector<Member> memberlist;
			nlohmann::json list = obj["members"];
			for (auto & it : list)
			{
				std::string name = it.value("name", "");
				std::string hash_id = it.value("hash_id", "");
				memberlist.push_back(Member(std::move(name), std::move(hash_id)));
			}
			message = new MessageAboutRoomEvent(
				MessageType::LeaveMemberFromRoom,
				std::move(room),
				std::move(sender),
				time,
				std::move(memberlist)
			);
		}
		else if (type == "EXIT_SERVER")
		{
			std::vector<Member> memberlist;
			nlohmann::json list = obj["members"];
			for (auto & it : list)
			{
				std::string name = it.value("name", "");
				std::string hash_id = it.value("hash_id", "");
				memberlist.push_back(Member(std::move(name), std::move(hash_id)));
			}
			message = new MessageAboutRoomEvent(
				MessageType::ExitServer,
				std::move(room),
				std::move(sender),
				time,
				std::move(memberlist)
			);
		}
		else if (type == "DISCONNECT_USER")
		{
			std::vector<Member> memberlist;
			nlohmann::json list = obj["members"];
			for (auto & it : list)
			{
				std::string name = it.value("name", "");
				std::string hash_id = it.value("hash_id", "");
				memberlist.push_back(Member(std::move(name), std::move(hash_id)));
			}
			message = new MessageAboutRoomEvent(
				MessageType::DisconnectedServer,
				std::move(room),
				std::move(sender),
				time,
				std::move(memberlist)
			);
		}
		if (message != nullptr)
		{
			wxThreadEvent * event = new wxThreadEvent(wxEVT_COME_MESSAGE);
			event->SetPayload<Message*>(message);
			wxApp::GetInstance()->QueueEvent(event);
		}
	}
	std::string m_hashId;
};




bool Application::OnInit()
{
	if (wxSocketBase::Initialize() == false)
	{
		wxMessageBox(wxT("네트워크 연결을 할 수 없습니다. 프로그램을 종료합니다."));
		return false;
	}
	this->Connect(wxEVT_TRABSFER_OPEN_SUCCESS,  (wxEventFunction)&Application::OnOpenSuccess, nullptr, (wxEvtHandler*)this);
	this->Connect( wxEVT_TRABSFER_OPEN_FAILED,  (wxEventFunction)&Application::OnOpenFailed, nullptr, (wxEvtHandler*)this);
	this->Connect(wxEVT_RECV_TRABSFER_CREATE_SUCCESS, (wxEventFunction)&Application::OnRecvOpenSuccess, nullptr, (wxEvtHandler*)this);
	this->Connect(wxEVT_RECV_TRABSFER_CREATE_FAILED, (wxEventFunction)&Application::OnRecvOpenFailed, nullptr, (wxEvtHandler*)this);
	this->Connect(wxEVT_COME_MESSAGE, (wxEventFunction)&Application::OnComeMessage, nullptr, this);

	auto dialog = new NameInputDialog();
	if (dialog->ShowModal() == wxID_CLOSE)
	{
		return false;
	}


	m_mainFrame = new MainFrame();
		
	m_mainFrame->Show();
		
	m_indicator = new wxActivityIndicator(m_mainFrame);
	m_indicator->Show();
	m_indicator->Start();
		
	m_mainFrame->Enable(false);

	m_mainFrame->Connect(wxEVT_CLOSE_WINDOW, (wxEventFunction)&Application::OnCloseMainFrame, nullptr, this);

	m_sendMessageThread = new SendMessageThread(&m_msgQueue,MakeFromWxString(dialog->GetName()));
	delete dialog;
	m_sendMessageThread->CreateThread();
	m_sendMessageThread->GetThread()->Run();

	return true;
}
int Application::OnExit()
{
	if (m_sendMessageThread != nullptr)
	{
		wxThread * thread = m_sendMessageThread->GetThread();
		if (thread != nullptr)
		{
			if (thread->IsAlive())
			{
				thread->Delete();
			}
		}
		delete m_sendMessageThread;
	}	
	return 0;
}

void Application::OnCloseMainFrame(wxCloseEvent & event)
{
	m_mainFrame = nullptr;
	event.Skip();
}
void Application::OnOpenSuccess(wxThreadEvent & event)
{
	m_hashId = MakeFromWxString(event.GetString());
	wxThread * thread = new RecvThread(m_hashId);
	thread->Run();
}
void Application::OnOpenFailed(wxThreadEvent & event)
{
	m_indicator->Stop();
	m_indicator->Destroy();
	wxMessageBox(wxT("서버와 연결할 수 없습니다."));
	Exit();
}
void Application::OnRecvOpenSuccess(wxThreadEvent & event)
{
	m_indicator->Stop();
	m_indicator->Destroy();
	m_mainFrame->Enable(true);
}
void Application::OnRecvOpenFailed(wxThreadEvent & event)
{
	m_indicator->Stop();
	m_indicator->Destroy();
	wxMessageBox(wxT("서버와 연결할 수 없습니다."));
	Exit();
}
void Application::OnComeMessage(wxThreadEvent & event)
{
	Message * message = nullptr;
	message = event.GetPayload<Message*>();
	IHasMemberList * objectHasMemberList = dynamic_cast<IHasMemberList*>(message);
	if (objectHasMemberList != nullptr)
	{
		std::string roomName = message->GetRoom();
		auto it = m_rooms.find(roomName);
		if (it == m_rooms.end())
		{
			std::shared_ptr<Room> newRoom(new Room(std::move(roomName), objectHasMemberList->GetMemberList()));
			m_rooms.insert(std::make_pair(message->GetRoom(), newRoom));
			m_mainFrame->AddNewChannelPage(newRoom);
		}
	}
	m_mainFrame->SendEventMessage(*message);
	if (message != nullptr)
	{
		delete message;
	}
}

void Application::ChatMessage(const std::string & roomName, const std::string & msg)
{
	//TODO:메시지 큐에 넣는 코드 재작성...
	//이 한 줄은...
	
	nlohmann::json obj;
	obj["type"] = "TEXT";
	obj["hash"] = m_hashId;
	obj["room"] = roomName;
	obj["value"] = msg;

	std::string data = obj.dump();
	m_msgQueue.Post(data);

	bool needRelive = false;
	needRelive = m_sendMessageThread->GetThread() != nullptr;
	if (needRelive == false)
	{
		needRelive = m_sendMessageThread->GetThread()->IsAlive() == false;
	}

	if (needRelive)
	{
		m_sendMessageThread->CreateThread();
		m_sendMessageThread->GetThread()->Run();
	}
}

void Application::EnterRoom(const std::string & roomName)
{
	nlohmann::json obj;
	obj["type"] = "ENTER";
	obj["hash"] = m_hashId;
	obj["room"] = roomName;
	obj["value"] = "";

	std::string data = obj.dump();
	m_msgQueue.Post(data);

	bool needRelive = false;
	needRelive = m_sendMessageThread->GetThread() != nullptr;
	if (needRelive == false)
	{
		needRelive = m_sendMessageThread->GetThread()->IsAlive() == false;
	}

	if (needRelive)
	{
		m_sendMessageThread->CreateThread();
		m_sendMessageThread->GetThread()->Run();
	}
}

void Application::LeaveRoom(const std::string & roomName)
{
	m_rooms.erase(roomName);
	nlohmann::json obj;
	obj["type"] = "LEAVE";
	obj["hash"] = m_hashId;
	obj["room"] = roomName;
	obj["value"] = "";

	std::string data = obj.dump();
	m_msgQueue.Post(data);

	bool needRelive = false;
	needRelive = m_sendMessageThread->GetThread() != nullptr;
	if (needRelive == false)
	{
		needRelive = m_sendMessageThread->GetThread()->IsAlive() == false;
	}

	if (needRelive)
	{
		m_sendMessageThread->CreateThread();
		m_sendMessageThread->GetThread()->Run();
	}
}
	
IMPLEMENT_APP(Application);