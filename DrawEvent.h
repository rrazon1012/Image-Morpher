#ifndef _DRAW_EVENT_H_
#pragma once
#include <wx/event.h>
#include "Line.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>      // redefines the new() operator 
#endif

class DrawEvent;

wxDECLARE_EVENT(myEVT_DRAW, DrawEvent);

class DrawEvent : public wxCommandEvent
{
public:
	DrawEvent(wxEventType commandtype = wxEVT_NULL, int id = 0)
		: wxCommandEvent(commandtype, id) {}

	DrawEvent(const DrawEvent& event)
		: wxCommandEvent(event) {this->setLine(event.getLine());}

	wxEvent* Clone() const { return new DrawEvent(*this); }

	Line getLine() const { return line; }
	void setLine(const Line& l) { line = l; }
private:
	Line line;
};

typedef void (wxEvtHandler::* DrawEventFunction)(DrawEvent&)
#define DrawEventFunctionHandler(func) wxEVENT_HANDLER_CAST(DrawEventFunction, func)                   

// Optional: define an event table entry
#define EVT_DRAW_LINE(id, func) \
 	wx__DECLARE_EVT1(myEVT_DRAW, id, DrawEventFunctionHandler(func))
#endif
;