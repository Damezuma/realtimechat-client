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
#include "channelpage.h"
const char * SERVER_IP = "localhost";

wxDECLARE_EVENT(wxEVT_RECV_MESSAGE, wxThreadEvent);
wxDEFINE_EVENT(wxEVT_RECV_MESSAGE, wxThreadEvent);

wxDECLARE_EVENT(wxEVT_RECV_MY_MESSAGE, wxThreadEvent);
wxDEFINE_EVENT(wxEVT_RECV_MY_MESSAGE, wxThreadEvent);

wxDECLARE_EVENT(wxEVT_COME_NEW_MEMEBER, wxThreadEvent);
wxDEFINE_EVENT(wxEVT_COME_NEW_MEMEBER, wxThreadEvent);

wxDECLARE_EVENT(wxEVT_LEAVE_MEMEBER, wxThreadEvent);
wxDEFINE_EVENT(wxEVT_LEAVE_MEMEBER, wxThreadEvent);

wxDECLARE_EVENT(wxEVT_TRABSFER_OPEN_SUCCESS, wxThreadEvent);
wxDEFINE_EVENT(wxEVT_TRABSFER_OPEN_SUCCESS, wxThreadEvent);

wxDECLARE_EVENT(wxEVT_TRABSFER_OPEN_FAILED, wxThreadEvent);
wxDEFINE_EVENT(wxEVT_TRABSFER_OPEN_FAILED, wxThreadEvent);

wxDECLARE_EVENT(wxEVT_RECV_TRABSFER_CREATE_SUCCESS, wxThreadEvent);
wxDEFINE_EVENT(wxEVT_RECV_TRABSFER_CREATE_SUCCESS, wxThreadEvent);

wxDECLARE_EVENT(wxEVT_RECV_TRABSFER_CREATE_FAILED, wxThreadEvent);
wxDEFINE_EVENT(wxEVT_RECV_TRABSFER_CREATE_FAILED, wxThreadEvent);

template <typename K,typename T>
using Dict = std::unordered_map<K, T>;
template <typename T>
using Refernce = std::shared_ptr<T>;
template <typename T>
using WeakReference = std::weak_ptr<T>;

struct ChatMessage
{
	wxString sender;
	wxString senderHash;
	wxString msg;
	wxString room;
	wxString text;
	wxString time;
};
class SendMessageThread : public wxThreadHelper
{
public:
	SendMessageThread(wxMessageQueue<wxString> * msgQueue,wxString name)
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
			object["name"] =this-> m_name.ToUTF8().data();
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
		wxString msg;
		while (m_msgQueue->ReceiveTimeout(1000, msg) == wxMSGQUEUE_NO_ERROR)
		{
			nlohmann::json obj;
			obj["type"] = "TEXT";
			obj["hash"] = m_hash.ToUTF8().data();
			obj["room"] = "lounge";
			obj["value"] = msg.ToUTF8().data();

			std::string data = obj.dump();
			data.push_back('\n');
			long writeSize = 0;
			while (writeSize != data.length())
			{
				writeSize += client->Write(data.c_str() + writeSize, data.length() - writeSize).LastWriteCount();
			}
		}
		return nullptr;
	}
private:
	wxString m_name;
	wxSocketClient* client;
	wxMessageQueue<wxString> * m_msgQueue;
	wxString m_hash;
};
class RecvThread : public wxThread
{

public:
	RecvThread(const wxString & hash)
	{
		m_hash = hash;
	}
	virtual void * Entry() override
	{
		wxSocketClient* client =new wxSocketClient();
		wxIPV4address ipv4addr;
		ipv4addr.Hostname(SERVER_IP);
		ipv4addr.Service(2017);
		
		std::string sendValue = m_hash.ToStdString();
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
					if (m_hash == msg)
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
		nlohmann::json obj = nlohmann::json::parse(msg.c_str());
		std::string type = obj.value("type", "");
		if (type == "CHAT_SEND")
		{
			ChatMessage * msg = new ChatMessage();
			std::string temp;
			temp = obj.value("text", "");
			msg->text = wxString::FromUTF8(temp.c_str());
			temp = obj.value("sender", "");
			msg->sender = wxString::FromUTF8(temp.c_str());
			temp = obj.value("time", "");
			msg->time = wxString::FromUTF8(temp.c_str());
			temp = obj.value("sender hash", "");
			msg->senderHash = wxString::FromUTF8(temp.c_str());
			
			wxThreadEvent * event = nullptr;
			if (msg->senderHash == m_hash)
			{
				event = new wxThreadEvent(wxEVT_RECV_MY_MESSAGE);
			}
			else
			{
				event = new wxThreadEvent(wxEVT_RECV_MESSAGE);
			}
			event->SetPayload<ChatMessage*>(msg);

			wxApp::GetInstance()->QueueEvent(event);
		}
		else if (type == "ENTER_NEW_MEMBER_IN_ROOM")
		{
			std::string sender = obj.value("sender", "");
			std::vector<wxString>* memberlist = new std::vector<wxString>();
			
			nlohmann::json list = obj["member list"];
			for (auto & it : list)
			{
				std::string name = it;
				memberlist->push_back(wxString::FromUTF8(name.c_str()));
			}

			wxThreadEvent * event = new wxThreadEvent(wxEVT_COME_NEW_MEMEBER);
			event->SetString(wxString::FromUTF8(sender.c_str()));
			event->SetPayload<std::vector<wxString>*>(memberlist);

			wxApp::GetInstance()->QueueEvent(event);
		}
		else if (type == "MEMBER_GET_OUT_ROOM")
		{
			std::string newMeber = obj.value("member", "");
			std::vector<wxString>* memberlist = new std::vector<wxString>();

			nlohmann::json list = obj["member list"];
			for (auto & it : list)
			{
				std::string name = it;
				memberlist->push_back(wxString::FromUTF8(name.c_str()));
			}

			wxThreadEvent * event = new wxThreadEvent(wxEVT_LEAVE_MEMEBER);
			event->SetString(wxString::FromUTF8(newMeber.c_str()));
			event->SetPayload<std::vector<wxString>*>(memberlist);

			wxApp::GetInstance()->QueueEvent(event);
		}
	}
	wxString m_hash;
};



class MainFrame : public MyFrame1
{
public:
	MainFrame() : MyFrame1(nullptr)
	{
	}
	virtual ~MainFrame()
	{

	}
protected:
	wxMessageQueue<wxString> m_msgQueue;
	Dict<wxString, ChannelPage*> m_roomPages;

};
class Application : public wxApp
{
public:
	virtual bool OnInit() override
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
		this->Connect(wxEVT_LEAVE_MEMEBER, (wxEventFunction)&Application::OnLeaveMember, nullptr, this);
		this->Connect(wxEVT_COME_NEW_MEMEBER, (wxEventFunction)&Application::OnComeNewMember, nullptr, this);
		this->Connect(wxEVT_RECV_MESSAGE, (wxEventFunction)&Application::OnRecvMessage, nullptr, this);
		this->Connect(wxEVT_RECV_MY_MESSAGE, (wxEventFunction)&Application::OnRecvMyMessage, nullptr, this);

		auto dialog = new NameInputDialog();
		if (dialog->ShowModal() == wxID_CLOSE)
		{
			return false;
		}


		m_mainFrame = new MainFrame();
		
		m_mainFrame->Show();
		this->Connect(ID_TXT_CHAT_MESSAGE_INPUT,wxEVT_CHAR , wxKeyEventHandler(Application::OnCharTxtCtrl), nullptr, this);
		m_indicator = new wxActivityIndicator(m_mainFrame);
		m_indicator->Show();
		m_indicator->Start();
		
		m_mainFrame->Enable(false);

		m_mainFrame->Connect(wxEVT_CLOSE_WINDOW, (wxEventFunction)&Application::OnCloseMainFrame, nullptr, this);

		m_sendMessageThread = new SendMessageThread(&m_msgQueue,dialog->GetName());
		delete dialog;
		m_sendMessageThread->CreateThread();
		m_sendMessageThread->GetThread()->Run();

		return true;
	}
	virtual int OnExit() override
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
	void OnRecvMessage(wxThreadEvent & event)
	{
		if (m_mainFrame != nullptr)
		{
			ChatMessage * msg = event.GetPayload<ChatMessage*>();
			wxDateTime dt;
			dt.ParseRfc822Date(msg->time);
			m_mainFrame->ShowMessage(dt, msg->sender, msg->text);
			delete msg;
		}
	}
	void OnRecvMyMessage(wxThreadEvent & event)
	{
		if (m_mainFrame != nullptr)
		{
			ChatMessage * msg = event.GetPayload<ChatMessage*>();
			wxDateTime dt;
			dt.ParseRfc822Date(msg->time);
			m_mainFrame->ShowMyMessage(dt, msg->text);
			delete msg;
		}
	}
	void OnCloseMainFrame(wxCloseEvent & event)
	{
		m_mainFrame = nullptr;
		event.Skip();
	}
	void OnOpenSuccess(wxThreadEvent & event)
	{
		wxThread * thread = new RecvThread(event.GetString());
		thread->Run();
	}
	void OnOpenFailed(wxThreadEvent & event)
	{
		m_indicator->Stop();
		m_indicator->Destroy();
		wxMessageBox(wxT("서버와 연결할 수 없습니다."));
		Exit();
	}
	void OnRecvOpenSuccess(wxThreadEvent & event)
	{
		m_indicator->Stop();
		m_indicator->Destroy();
		m_mainFrame->Enable(true);
	}
	void OnRecvOpenFailed(wxThreadEvent & event)
	{
		m_indicator->Stop();
		m_indicator->Destroy();
		wxMessageBox(wxT("서버와 연결할 수 없습니다."));
		Exit();
	}
	void OnComeNewMember(wxThreadEvent & event)
	{
		std::vector<wxString> * list = event.GetPayload<std::vector<wxString>*>();
		m_mainFrame->UpdateMemberList(*list);
		m_mainFrame->ShowSystemMessage(wxString::Format(wxT("[System]%s님이 입장하였습니다."), event.GetString()));
		delete list;
	}
	void OnLeaveMember(wxThreadEvent & event)
	{
		std::vector<wxString> * list = event.GetPayload<std::vector<wxString>*>();
		m_mainFrame->UpdateMemberList(*list);
		m_mainFrame->ShowSystemMessage(wxString::Format(wxT("[System]%s님이 떠났습니다."), event.GetString()));
		delete list;
	}
	void OnCharTxtCtrl(wxKeyEvent& event)
	{
		if (event.GetKeyCode() == wxKeyCode::WXK_RETURN)
		{
			if (!event.ShiftDown())
			{
				m_msgQueue.Post(m_mainFrame->GetMessage());
				if (m_sendMessageThread->GetThread() != nullptr)
				{
					if (m_sendMessageThread->GetThread()->IsAlive() == false)
					{
						m_sendMessageThread->CreateThread();
						m_sendMessageThread->GetThread()->Run();
					}
				}
				else
				{
					m_sendMessageThread->CreateThread();
					m_sendMessageThread->GetThread()->Run();
				}
			}
			else
			{
				event.Skip();
			}
		}
		else
		{
			event.Skip();
		}
	}
private:
	wxActivityIndicator * m_indicator = nullptr;
	MainFrame * m_mainFrame = nullptr;
	wxString m_hash;
	wxMessageQueue<wxString> m_msgQueue;
	SendMessageThread * m_sendMessageThread = nullptr;
};

IMPLEMENT_APP(Application);