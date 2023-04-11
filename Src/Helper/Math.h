#pragma once
#include <wx/wx.h>
#include <cmath>
//header files that contain math helper functions

namespace Math {
	struct vector {
		float x;
		float y;
		//Vector() { };
		vector(float X, float Y) { x = X, y = Y; }
	};
	//compare to
	bool isPointWithinCircle(wxPoint pt, wxPoint center, int radius);
	float dotProduct(Math::vector a, Math::vector b);
	float norm(Math::vector a);
	int clamp(int val, int min, int max);
	Math::vector subtract(Math::vector a, Math::vector b);
}