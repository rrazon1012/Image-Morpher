#pragma once
#include <wx/wx.h>
#include "../Image Drawing/ImagePanel.h"
#include "../Morpher/MorpherFrame.h"
#include <iostream>
#include <fstream>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>      // redefines the new() operator 
#endif

class ImageMorpherFrame: public wxFrame
{
public:
	ImageMorpherFrame();
	void InitializeGUI();
private:
	//the path of the file to be opened/edited
	wxString CurrentDocPath;
	ImagePanel* leftImage;
	ImagePanel* rightImage;
	enum {
		ID_Hello = 1,
		wxID_Image1 = 2, //left image event
		wxID_Image2 = 3, //right image event
		wxID_Morph = 4   //morph event
	};
	void OnLoadImage(wxCommandEvent& event);
	void OnExit(wxCommandEvent& event);
	void OnAbout(wxCommandEvent& event);
	void OnSyncLines(DrawEvent& event);
	void OnMorph(wxCommandEvent& event);
};

