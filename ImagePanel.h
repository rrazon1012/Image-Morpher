#ifndef _IMAGE_PANEL_H
#pragma once
#include <wx/wx.h>
#include "Line.h"
#include <vector>
#include <memory>
#include "DrawEvent.h"
#include "Math.h"


enum PaneNUM {
	LeftPane = 5,
	RightPane = 6
};

class ImagePanel : public wxPanel
{
private:
	wxBitmap resized;
	int w, h;
	const int radius = 7;

	wxPoint dragSource;

	bool isDragging = false;
	bool isDrawing = false;
	bool moveStart = false;

	void drawLine(wxDC& dc, wxPoint dragsource, wxPoint dragEnd);

	int paneId;
	wxPoint* currPoint = nullptr; // currently dragged point
public:
	wxImage image;
	std::vector<Line> drawnLines;
	ImagePanel(wxFrame* parent, wxString file, wxBitmapType format, int pID);
	ImagePanel(wxPanel* parent, wxString file, wxBitmapType format, int pID);
	void paintEvent(wxPaintEvent& event);
	void paintNow();
	void OnSize(wxSizeEvent& event);
	void render(wxDC& dc);
	void loadImage(wxString file, wxBitmapType format);
	void OnLeftDown(wxMouseEvent& event);
	void mouseMoved(wxMouseEvent& event);
	void OnLeftUp(wxMouseEvent& event);
	void OnAddLine(DrawEvent& event);

	DECLARE_EVENT_TABLE();
};
#endif
