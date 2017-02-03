#include "messageballoon.h"
#include <wx/dc.h>
#include <wx/graphics.h>
wxIMPLEMENT_DYNAMIC_CLASS(MyMessageBalloon, wxControl);
wxIMPLEMENT_DYNAMIC_CLASS(MessageBalloon, wxControl);
wxIMPLEMENT_DYNAMIC_CLASS(SystemMessageBalloon, wxControl);
MyMessageBalloon::MyMessageBalloon()
{
	Init();
}
MyMessageBalloon::MyMessageBalloon(wxWindow * parent, wxWindowID id,const wxDateTime & dateTime,const wxString& msg) : wxControl(parent,id,wxDefaultPosition,wxDefaultSize, wxBORDER_NONE)
{
	this->SetBackgroundColour(wxColor(255, 255, 255));
	Init();
	this->Connect(wxEVT_PAINT, wxPaintEventHandler(MyMessageBalloon::OnPaint), nullptr, this);
	this->Connect(wxEVT_SIZE, wxSizeEventHandler(MyMessageBalloon::OnSize), nullptr, this);
	
	this->m_text = msg;
	this->m_timeString = dateTime.FormatTime();


	wxWindowDC dc(this);
	wxGraphicsContext * gc = wxGraphicsContext::Create(dc);
	wxFont* font = wxFont::New(12, wxFontFamily::wxFONTFAMILY_SWISS, wxFONTFLAG_DEFAULT | wxFONTFLAG_ANTIALIASED, wxT("Noto Sans CJK KR"));
	wxGraphicsFont gfont = gc->CreateFont(*font);
	gc->SetFont(gfont);
	double width;
	double height;
	double desc;
	double exle;
	gc->GetTextExtent(msg, &width, &height, &desc, &exle);

	double timeStringWidth;
	double timeStringHeight;
	gc->GetTextExtent(m_timeString, &timeStringWidth,&timeStringHeight);
	if (timeStringWidth > width)
	{
		width = timeStringWidth;
	}
	height = height + timeStringHeight;
	this->SetMinSize(wxSize(width + 34, height + 16));
	this->SetBestSize(wxSize(width + 34, height + 16));
	delete font;
	delete gc;
	
}



void MyMessageBalloon::OnPaint(wxPaintEvent & event)
{
	wxPaintDC dc(this);
	wxGraphicsContext * gc = wxGraphicsContext::Create(dc);
	if (gc != nullptr)
	{
		wxFont* font = wxFont::New(12, wxFontFamily::wxFONTFAMILY_SWISS, wxFONTFLAG_DEFAULT | wxFONTFLAG_ANTIALIASED, wxT("Noto Sans CJK KR"));
		if (font == nullptr)
		{
			font = new wxFont(dc.GetFont());
			font->SetPointSize(12);
		}
		wxFontMetrics metrics = dc.GetFontMetrics();
		wxGraphicsFont gfont = gc->CreateFont(*font);
		
		gc->SetFont(gfont);
		
		double width;
		double height;
		double timeStringWidth;
		double timeStringHeight;

		gc->GetTextExtent(m_timeString, &timeStringWidth, &timeStringHeight);
		gc->GetTextExtent(m_text, &width, &height);
		wxSize panelSize = this->GetSize();
		auto brush = gc->CreateBrush(wxBrush(wxColor(200, 255, 200)));
		auto pen = gc->CreatePen(wxPen(wxColor(0, 0, 0), 2));
		gc->SetBrush(brush);
		//gc->SetPen(pen);
		gc->DrawRoundedRectangle(panelSize.x - (width + 12) - 2, timeStringHeight + 2, width + 10, height + 10, 5);
		gc->DrawText(m_text, panelSize.x - (width + 7) - 2, timeStringHeight + 2 + (height + 10 - height) / 2);
		gc->DrawText(m_timeString, panelSize.x - timeStringWidth -2, 0);
		delete font;
		delete gc;
	}
}

void MyMessageBalloon::OnSize(wxSizeEvent & event)
{
	Refresh();
	event.Skip();
}

MessageBalloon::MessageBalloon()
{
	Init();
}
MessageBalloon::MessageBalloon(wxWindow * parent, wxWindowID id, const wxString & sender, const wxDateTime & dateTime, const wxString& msg) : wxControl(parent, id, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE)
{
	this->SetBackgroundColour(wxColor(255, 255, 255));
	Init();
	this->Connect(wxEVT_PAINT, wxPaintEventHandler(MessageBalloon::OnPaint), nullptr, this);
	this->Connect(wxEVT_SIZE, wxSizeEventHandler(MessageBalloon::OnSize), nullptr, this);
	
	this->m_text = msg;
	this->m_timeString = dateTime.FormatTime();
	this->m_sender = sender;

	wxWindowDC dc(this);
	wxGraphicsContext * gc = wxGraphicsContext::Create(dc);
	wxFont* font = wxFont::New(12, wxFontFamily::wxFONTFAMILY_SWISS, wxFONTFLAG_DEFAULT | wxFONTFLAG_ANTIALIASED, wxT("Noto Sans CJK KR"));
	wxGraphicsFont gfont = gc->CreateFont(*font);
	gc->SetFont(gfont);
	double width;
	double height;
	double desc;
	double exle;
	gc->GetTextExtent(m_text, &width, &height, &desc, &exle);

	double timeStringWidth;
	double timeStringHeight;
	gc->GetTextExtent(m_timeString, &timeStringWidth, &timeStringHeight);
	if (timeStringWidth > width)
	{
		width = timeStringWidth;
	}
	double senderNameWidth;
	double senderNameHeight;
	gc->GetTextExtent(sender, &senderNameWidth, &senderNameHeight);
	if (senderNameWidth > width)
	{
		width = senderNameWidth;
	}
	height = height + senderNameHeight + timeStringHeight;
	this->SetMinSize(wxSize(width + 34, height + 16));
	this->SetBestSize(wxSize(width + 34, height + 16));
	delete font;
	delete gc;
	
}



void MessageBalloon::OnPaint(wxPaintEvent & event)
{
	wxPaintDC dc(this);
	wxGraphicsContext * gc = wxGraphicsContext::Create(dc);
	if (gc != nullptr)
	{
		wxFont* font = wxFont::New(12, wxFontFamily::wxFONTFAMILY_SWISS, wxFONTFLAG_DEFAULT | wxFONTFLAG_ANTIALIASED, wxT("Noto Sans CJK KR"));
		if (font == nullptr)
		{
			font = new wxFont(dc.GetFont());
			font->SetPointSize(12);
		}
		wxGraphicsFont gfont = gc->CreateFont(*font);
		gc->SetFont(gfont);

		wxSize size = this->GetSize();

		wxString test = m_text;
		double width;
		double height;
		double desc;
		double exle;
		double timeStringWidth;
		double timeStringHeight;
		double senderNameWidth;
		double senderNameHeight;

		gc->GetTextExtent(m_sender, &senderNameWidth, &senderNameHeight);
		gc->GetTextExtent(m_timeString, &timeStringWidth, &timeStringHeight);
		gc->GetTextExtent(test, &width, &height,&desc,&exle);
		
		auto brush = gc->CreateBrush(wxBrush(wxColor(200, 200, 200)));
		auto pen = gc->CreatePen(wxPen(wxColor(0, 0, 0), 2));
		gc->SetBrush(brush);
		//gc->SetPen(pen);
		gc->DrawRoundedRectangle(5, senderNameHeight + timeStringHeight + 2, width + 10, height + 10, 5);
		gc->DrawText(m_text,8,  senderNameHeight + timeStringHeight +  2 + (height + 10 - height)/2);

		gc->DrawText(m_sender, 2, 1);
		gc->DrawText(m_timeString, 2, senderNameHeight + 1);
		delete font;
		delete gc;
		
	}
}

void MessageBalloon::OnSize(wxSizeEvent & event)
{
	Refresh();
	event.Skip();
}

SystemMessageBalloon::SystemMessageBalloon()
{
	Init();
}
SystemMessageBalloon::SystemMessageBalloon(wxWindow * parent, wxWindowID id,  const wxString& msg) : wxControl(parent, id, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE)
{
	Init();
	this->SetBackgroundColour(wxColor(255, 255, 255));
	this->Connect(wxEVT_PAINT, wxPaintEventHandler(SystemMessageBalloon::OnPaint), nullptr, this);
	this->Connect(wxEVT_SIZE, wxSizeEventHandler(SystemMessageBalloon::OnSize), nullptr, this);

	this->m_text = msg;
	wxWindowDC dc(this);
	wxGraphicsContext * gc = wxGraphicsContext::Create(dc);
	wxFont* font = wxFont::New(12, wxFontFamily::wxFONTFAMILY_SWISS, wxFONTFLAG_DEFAULT | wxFONTFLAG_ANTIALIASED, wxT("Noto Sans CJK KR"));
	wxGraphicsFont gfont = gc->CreateFont(*font);
	gc->SetFont(gfont);
	double width;
	double height;
	double desc;
	double exle;
	gc->GetTextExtent(m_text, &width, &height, &desc, &exle);
	this->SetMinSize(wxSize(width + 16, height + 16));
	this->SetBestSize(wxSize(width + 16, height + 16));
	delete font;
	delete gc;
}



void SystemMessageBalloon::OnPaint(wxPaintEvent & event)
{
	wxPaintDC dc(this);
	wxGraphicsContext * gc = wxGraphicsContext::Create(dc);
	if (gc != nullptr)
	{
		wxFont* font = wxFont::New(12, wxFontFamily::wxFONTFAMILY_SWISS, wxFONTFLAG_DEFAULT | wxFONTFLAG_ANTIALIASED, wxT("Noto Sans CJK KR"));
		if (font == nullptr)
		{
			font = new wxFont(dc.GetFont());
			font->SetPointSize(12);
		}
		wxGraphicsFont gfont = gc->CreateFont(*font);
		gc->SetFont(gfont);

		wxSize size = this->GetSize();

		wxString test = m_text;
		double width;
		double height;
		double desc;
		double exle;
		gc->GetTextExtent(test, &width, &height, &desc, &exle);

		auto brush = gc->CreateBrush(wxBrush(wxColor(233, 233, 233)));
		auto pen = gc->CreatePen(wxPen(wxColor(0, 0, 0), 2));
		gc->SetBrush(brush);
		gc->SetPen(pen);
		gc->DrawRoundedRectangle(3,  2, width + 10, height + 10, 5);
		gc->DrawText(m_text, 7,  2 + (height + 10 - height) / 2);

		delete font;
		delete gc;

	}
}

void SystemMessageBalloon::OnSize(wxSizeEvent & event)
{
	Refresh();
	event.Skip();
}


