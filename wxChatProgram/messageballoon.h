#pragma once
#include <wx/wx.h>
class MyMessageBalloon :public wxControl
{
	wxDECLARE_DYNAMIC_CLASS(MyMessageBalloon);
public:
	MyMessageBalloon();
	MyMessageBalloon(wxWindow * parent, wxWindowID id, const wxDateTime & dateTime, const wxString& msg);
private:
	void Init() {
		// init widget's internals...
	}
	void OnPaint(wxPaintEvent & event);
	void OnSize(wxSizeEvent & event);
	wxString m_text;
	wxString m_timeString;
	
};
class MessageBalloon :public wxControl
{
	wxDECLARE_DYNAMIC_CLASS(MessageBalloon);
public:
	MessageBalloon();
	MessageBalloon(wxWindow * parent, wxWindowID id,const wxString & sender, const wxDateTime & dateTime, const wxString& msg);


private:
	void Init() {
		// init widget's internals...
	}
	void OnPaint(wxPaintEvent & event);
	void OnSize(wxSizeEvent & event);
	wxString m_text;
	wxString m_sender;
	wxString m_timeString;

};
class SystemMessageBalloon :public wxControl
{
	wxDECLARE_DYNAMIC_CLASS(SystemMessageBalloon);
public:
	SystemMessageBalloon();
	SystemMessageBalloon(wxWindow * parent, wxWindowID id,  const wxString& msg);


private:
	void Init() {
		// init widget's internals...
	}
	void OnPaint(wxPaintEvent & event);
	void OnSize(wxSizeEvent & event);
	wxString m_text;
};