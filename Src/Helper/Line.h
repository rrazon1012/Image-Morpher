#pragma once
#include <wx/gdicmn.h>
//used for calculating the initial warp as well as the cross dissolve
struct Line {
	Line(){}
	Line(wxPoint s, wxPoint e) {
		start = s;
		end = e;
		distance = 0; //make a math helper;
	}
	wxPoint start;
	wxPoint end;
	int distance=0;
};