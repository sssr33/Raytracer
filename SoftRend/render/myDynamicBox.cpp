#include "myDynamicBox.h"


BEGIN_EVENT_TABLE(myDynamicBox, wxStaticBox)
	EVT_LEFT_UP(myDynamicBox::OnLeftUp)
END_EVENT_TABLE()

myDynamicBox::myDynamicBox(void)
{
}


myDynamicBox::~myDynamicBox(void)
{
}

myDynamicBox::myDynamicBox(wxWindow *parent, wxWindowID id,
                const wxString& label, bool expanded) : wxStaticBox(parent, id, label)
{
	this->label = label;
	/*if(expanded)
	{
		this->Expand();
	}
	else
	{
		this->Collapse();
	}*/
}

void myDynamicBox::Collapse()
{
	wxSizer *sizer = this->GetSizer();

	if(sizer)
	{
		isExpanded = false;
		this->SetLabelText(label + " +");

		int count = sizer->GetItemCount();

		for(int i = 0; i < count; i++)
		{
			sizer->Hide(size_t(i));
		}

		this->SetSizeHints(wxDefaultSize);
	}
}
void myDynamicBox::Expand()
{
	wxSizer *sizer = this->GetSizer();

	if(sizer)
	{
		isExpanded = true;
		this->SetLabelText(label + " -");
		sizer->SetSizeHints(this);

		int count = sizer->GetItemCount();

		for(int i = 0; i < count; i++)
		{
			sizer->Show(size_t(i));
		}
	}

}

void myDynamicBox::OnLeftUp(wxMouseEvent& event)
{

	MessageBox(0,0,0,0);

	if(this->isExpanded)
	{
		this->Collapse();
	}
	else
	{
		this->Expand();
	}
}

//myDynamicBox::myDynamicBox(void)
//{
//}
//
//
//myDynamicBox::~myDynamicBox(void)
//{
//	if(sb)
//	{
//		delete sb;
//		sb = 0;
//	}
//}
//
//myDynamicBox::myDynamicBox(wxWindow *parent, wxWindowID id,
//                const wxString& label, bool expanded) : wxPanel(parent, id)
//{
//	sb = 0;
//	wxBoxSizer *bsizer = new wxBoxSizer(wxVERTICAL);
//
//	wxFlexGridSizer *fsizer = new wxFlexGridSizer(1, 0, 0);
//	sb = new wxStaticBox(this, wxID_ANY, label);
//
//	//wxStaticText *st = new wxStaticText(sb, wxID_ANY, "This window is a child of the staticbox"); 
//	//fsizer->Add(st, 0, wxALL | wxGROW | wxALIGN_CENTER, 25);
//
//	sb->SetSizer(fsizer);
//	sb->GetSizer()->SetSizeHints(sb);
//
//	//this->SetSizer(fsizer);
//	bsizer->Add(sb , 0, wxALL | wxGROW | wxALIGN_CENTER,0);
//
//	//this->SetSize(sb->GetSize());
//
//	this->SetSizer(bsizer);
//	bsizer->SetSizeHints(this);
//
//	
//	this->isExpanded = expanded;
//	//this->Expand();
//
//	//this->SetSizeHints(sb->GetSize());
//
//	//sb->SetSizer(fsizer);
//
//	//if(expanded)
//	//{
//	//	this->Expand();
//	//}
//	//else
//	//{
//	//	this->Collapse();
//	//}
//}

//void myDynamicBox::AddItem(wxWindow *w, int proportion, int flag, int border, wxObject *userData)
//{
//	wxSizer *sizer = sb->GetSizer();
//	//this->sb->GetSizer()->Add(w, proportion, flag, border, userData);
//	sizer->Add(w, proportion, flag, border, userData);
//
//	if(isExpanded)
//	{
//		sizer->SetSizeHints(sb);
//		this->GetSizer()->SetSizeHints(this);
//	}
//	else
//	{
//		sizer->Hide(w);
//		sb->SetSizeHints(wxDefaultSize);
//		this->SetSizeHints(wxDefaultSize);
//	}
//}
//
//void myDynamicBox::Collapse()
//{
//	wxSizer *sizer = sb->GetSizer();
//
//	if(sizer)
//	{
//		isExpanded = false;
//		int count = sizer->GetItemCount();
//
//		for(int i = 0; i < count; i++)
//		{
//			sizer->Hide(size_t(i));
//		}
//
//		//sb->SetSizeHints(wxDefaultSize);
//		//this->SetSizeHints(wxDefaultSize);
//		//sizer->SetSizeHints(sb);
//		sb->SetSizeHints(wxSize(100,100));
//	}
//}
//void myDynamicBox::Expand()
//{
//	wxSizer *sizer = sb->GetSizer();
//
//	if(sizer)
//	{
//		isExpanded = true;
//		
//		//this->SetSizeHints(sb->GetSize());
//		//this->GetSizer()->SetSizeHints(this);
//
//		int count = sizer->GetItemCount();
//
//		for(int i = 0; i < count; i++)
//		{
//			sizer->Show(size_t(i));
//		}
//
//		//this->SetSizeHints(sb->GetSize());
//		sizer->SetSizeHints(sb);
//	}
//
//}
//
//void myDynamicBox::OnLeftUp(wxMouseEvent& event)
//{
//
//	//MessageBox(0,0,0,0);
//
//	if(this->isExpanded)
//	{
//		this->Collapse();
//	}
//	else
//	{
//		this->Expand();
//	}
//}