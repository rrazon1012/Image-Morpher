#include "Math.h"

namespace Math {

	bool isPointWithinCircle(wxPoint pt, wxPoint center, int radius)
	{
		return (pow((pt.x - center.x), 2) + pow((pt.y - center.y), 2) < pow(radius, 2));
	}
	float dotProduct(Math::vector a, Math::vector b)
	{
		return (a.x * b.x) + (a.y * b.y);
	}
	float norm(Math::vector a)
	{
		return sqrt((a.x * a.x) + (a.y * a.y));
	}
	int clamp(int val, int min, int max)
	{
		if (val >= min && val <= max) {
			return val;
		}
		else {
			if (val < min) {
				return min;
			}
			else {
				return max;
			}
		}
	}
	Math::vector subtract(Math::vector a, Math::vector b)
	{
		Math::vector sum = {a.x-b.x,a.y - b.y};
		return sum;
	}
}