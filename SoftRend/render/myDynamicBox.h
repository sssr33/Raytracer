#pragma once
#include "wx/wx.h"
class myDynamicBox : public wxStaticBox
{
public:
	myDynamicBox(void);
	myDynamicBox(wxWindow *parent, wxWindowID id,
                const wxString& label, bool expanded);
	~myDynamicBox(void);

	void Collapse();
	void Expand();

	void OnLeftUp(wxMouseEvent& event);

	DECLARE_EVENT_TABLE()
	
private:
	bool isExpanded;
	wxString label;
	
};

//class myDynamicBox : public wxPanel
//{
//public:
//	myDynamicBox(void);
//	myDynamicBox(wxWindow *parent, wxWindowID id,
//                const wxString& label, bool expanded);
//	~myDynamicBox(void);
//
//	void Collapse();
//	void Expand();
//	void AddItem(wxWindow *w, int proportion = 0, int flag = 0, int border = 0, wxObject *userData = (wxObject *)0);
//	wxStaticBox *getRealParent()
//	{
//		return sb;
//	}
//
//	DECLARE_EVENT_TABLE()
//	
//private:
//	bool isExpanded;
//	wxStaticBox *sb;
//
//	void OnLeftUp(wxMouseEvent& event);
//};