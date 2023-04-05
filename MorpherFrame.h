#pragma once
#include <wx/wx.h>
#include <wx/numdlg.h>
#include "Line.h"
#include <vector>
#include "Math.h"
#include <algorithm>
#include <iostream>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>      // redefines the new() operator 
#endif

//make this a dialog rather than a new frame so users can only interact with this window while its open
class MorpherPanel : public wxDialog
{
private:
	std::vector<wxImage> morphImages;
	wxStaticBitmap* bitmap;
	wxSlider* slider;
	int currentFrame = 0;
public:
	MorpherPanel(wxFrame* parent); //sourceImage destImage, source lines, destination lines, intermediate frames
	void morphImage(wxImage source, wxImage dest, std::vector<Line> sourceLines, std::vector<Line> destLines, int frames);
	std::vector<Line> interpolateLines(std::vector<Line> sourceLines, std::vector<Line> destLines, float t);
	wxImage warpImage(wxImage source, std::vector<Line> sourceLines, std::vector<Line> destLines);
	wxImage blendImage(wxImage source, wxImage dest, float t);
	void OnScroll(wxCommandEvent& event);
};

