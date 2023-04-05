#include "MorpherFrame.h"

MorpherPanel::MorpherPanel(wxFrame* parent)
	: wxDialog(parent, wxID_ANY,"Morphed Images", wxDefaultPosition, wxSize(1000,1000))
{
	wxPanel* wrapper = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize);
	wxSizer* wrapperSizer = new wxBoxSizer(wxVERTICAL);
	slider = new wxSlider(this, wxID_ANY, 0, 0, 9, wxDefaultPosition, wxDefaultSize);
	bitmap = new wxStaticBitmap(this, wxID_ANY,wxString("image"), wxDefaultPosition, wxSize(640,480));
	wrapperSizer->Add(bitmap, wxSizerFlags().CenterHorizontal().Border(wxALL,40));
	wrapperSizer->Add(slider, wxSizerFlags().CenterHorizontal().Border(wxALL,40));
	SetSizer(wrapperSizer);

	Bind(wxEVT_SLIDER, &MorpherPanel::OnScroll, this, wxID_ANY);
}

void MorpherPanel::morphImage(wxImage source, wxImage dest, std::vector<Line> sourceLines, std::vector<Line> destLines, int frames)
{
	/*
	* input source image, dest image
	* output vector of morphed images
	* for each intermediate frame (t) {
	* warp(source img, Lines)
	* warp(dest img, Lines)
	* Blend(Warped source, warped dest, curr frame (t))
	}
	*/

	float n = 1.0 / frames;
	for (int t = 0; t <= frames; t++) {
		////calculate interpolated line
		wxLogDebug(wxString(std::to_string(t*n)));
		std::vector<Line> interpolatedLines = interpolateLines(sourceLines, destLines, n*t);
		//Ws = warp(S,Lt)
		wxImage ws = warpImage(source, sourceLines, interpolatedLines);
		wxImage wd = warpImage(dest, destLines, interpolatedLines);
		//Wd = warp(D,Lt)
		wxImage iT = blendImage(ws, wd, t*n);
		//destroy the intermediate images
		morphImages.push_back(iT);
	}
	morphImages.push_back(dest);
	//move to a paint event
	slider->SetMax(morphImages.size()-1);
	bitmap->SetBitmap(wxBitmap(morphImages[currentFrame]));
}

std::vector<Line> MorpherPanel::interpolateLines(std::vector<Line> sourceLines, std::vector<Line> destLines, float t)
{
	std::vector<Line> interpolatedLines;
	for (int i = 0; i < sourceLines.size(); i++) {

		wxPoint start = wxPoint(round((1 - t) * sourceLines[i].start.x + t * destLines[i].start.x), round((1 - t) * sourceLines[i].start.y + t * destLines[i].start.y));
		wxPoint end = wxPoint(round((1 - t) * sourceLines[i].end.x + t * destLines[i].end.x), round((1 - t) * sourceLines[i].end.y + t * destLines[i].end.y));

		interpolatedLines.push_back({start,end});
	}
	return interpolatedLines;
}

wxImage MorpherPanel::warpImage(wxImage source, std::vector<Line> sourceLines, std::vector<Line> destLines)
{
	//initialize destination image to calculate the pixel to be taken from source image
	wxImage dest = wxImage(source.GetSize());
	int width = dest.GetWidth();
	int height = dest.GetHeight();

	for (int y = 0; y < dest.GetHeight(); y++) {
		for (int x = 0; x < dest.GetWidth(); x++) {
			
			Math::vector displacementSum = {0.0,0.0};
			float weightSum = 0.0;

			//add check that they have same size
			auto destIt = destLines.begin();
			auto sourceIt = sourceLines.begin();

			for (; destIt != destLines.end() && sourceIt != sourceLines.end(); destIt++,sourceIt++) {
				//calculate u and v for x
				//projection calculations for readability


				Math::vector P = { static_cast<float>(destIt->start.x), static_cast<float>(destIt->start.y) };
				Math::vector Q = { static_cast<float>(destIt->end.x), static_cast<float>(destIt->end.y) };
				Math::vector X = { static_cast<float>(x),static_cast<float>(y)};

				Math::vector PQ = Math::subtract(Q, P);
				Math::vector PX = Math::subtract(X, P);
				Math::vector PerpPQ = { PQ.y,-PQ.x };

				float u = Math::dotProduct(PX,PQ) / pow(Math::norm(PQ),2);
				float v = Math::dotProduct(PX,PerpPQ) / Math::norm(PQ);

				//calculate the pixel in source image Xn(xn,yn) corresponding to current destination pixel X(x,y)
				// Xn = Pn + u * PQn + (v * perp(PQn) / ||PQn||)
				Math::vector Pn = { static_cast<float>(sourceIt->start.x), static_cast<float>(sourceIt->start.y) };
				Math::vector Qn = { static_cast<float>(sourceIt->end.x), static_cast<float>(sourceIt->end.y) };

				Math::vector PQn = Math::subtract(Qn, Pn);
				Math::vector perpPQn = { PQn.y, -PQn.x };

				Math::vector PQnu = { PQn.x * u, PQn.y * u };
				float pqnNorm = Math::norm(PQn);
				Math::vector PQnv = { perpPQn.x * v / pqnNorm, perpPQn.y * v / pqnNorm };
				Math::vector Xn = { Pn.x + PQnu.x + PQnv.x,Pn.y + PQnu.y + PQnv.y };

				//calculate displacement
				Math::vector displacement = { Xn.x - x, Xn.y - y };

				//calculate weight
				float a = 1, b = 2,  p = 0; // a = tighter control vs smoother warping, b = line distance influence , p = influence of line length
				float distance = 0;
				float length = Math::norm(PQ);

				if (u < 0) {
					Math::vector XnP = {Xn.x - sourceIt->start.x, Xn.y - sourceIt->start.y};
					distance = sqrt(Math::dotProduct(XnP,XnP));
				}
				else if (u > 0) {
					Math::vector XnU = { Xn.x - sourceIt->end.x, Xn.y - sourceIt->end.y };
					distance = sqrt(Math::dotProduct(XnU, XnU));
				}
				else distance = abs(v);
				
				float weight = pow( pow(length, p) / (a + distance), b);
				//add to displacement sum
				displacementSum.x += displacement.x * weight, displacementSum.y += displacement.y * weight;
				weightSum += weight;
				//r,c = r,c + Dsum/weightsum
			}

			Math::vector Xn = { round(static_cast<float>(x) + (displacementSum.x / weightSum)), round(static_cast<float>(y) + (displacementSum.y / weightSum))};
			
			wxPoint pixel = wxPoint(Xn.x,Xn.y);
			/*wxLogDebug(wxString("source Pixel ")+ wxString(std::to_string(Xn.x)) + wxString(" ") + wxString(std::to_string(Xn.y)));
			wxLogDebug(wxString("dest Pixel ")+ wxString(std::to_string(x)) + wxString(" ") + wxString(std::to_string(y)));*/
			if (pixel.x < 0) 
			{
				pixel.x = 0; 
			}
			else if (pixel.x >= dest.GetWidth()) 
			{
				pixel.x = dest.GetWidth() - 1;
			}
			if (pixel.y < 0) {
				pixel.y = 0;
			}
			else if (pixel.y >= dest.GetHeight()) {
				pixel.y = dest.GetHeight() - 1;
			}
			
			//wxLogDebug(wxString(std::to_string(dest.GetWidth()-1)));

			/*wxLogDebug(wxString("X: ") + wxString(std::to_string(pixel.x)));
			wxLogDebug(wxString("Y: ") + wxString(std::to_string(pixel.y)));*/
			
			dest.SetRGB(x, y, source.GetRed(pixel.x,pixel.y), source.GetGreen(pixel.x,pixel.y), source.GetBlue(pixel.x,pixel.y));
		}
	}
	return dest;
}

wxImage MorpherPanel::blendImage(wxImage source, wxImage dest, float t)
{
	wxImage blendedImage = wxImage(source.GetSize());
	for (int y = 0; y < blendedImage.GetHeight(); y++) {
		for (int x = 0; x < blendedImage.GetWidth(); x++) {

			unsigned char r = Math::clamp((int)round( (((1-t) * source.GetRed(x, y)) + ((t) * (dest.GetRed(x, y))))), 0, 255);
			unsigned char g = Math::clamp((int)round( (((1-t) * source.GetGreen(x, y)) + ((t) * (dest.GetGreen(x, y))))), 0, 255);
			unsigned char b = Math::clamp((int)round( (((1-t) * source.GetBlue(x, y)) + ((t) * (dest.GetBlue(x, y))))), 0, 255);
			
			blendedImage.SetRGB(x, y, r,g,b);
		}
	}
	return blendedImage;
}

void MorpherPanel::OnScroll(wxCommandEvent& event)
{
	if (morphImages.size() != 0) {
		currentFrame = event.GetInt();
		bitmap->SetBitmap(morphImages[currentFrame]);
	}
}



/*
* 
Input: source image S, feature line-segment set P1Q1, P2Q2, ..., PnQn
Output: destination image D
	1: for each pixel with position X in the destination do
	2: Dsum = (0, 0)
	3: Wsum = 0
	4:	for each PiQi in the feature line-segment set do
	5:		calculate u and v for X based on PiQi using (3.2) and 3.3
	6:		find X0 in the source image with u and v using (3.1)
	7:		calculate displacement di = X0 ? X
	8:		calculate the weight w using (3.5)
	9:		Dsum = diw + Dsum
	10:		Wsum = w + Wsum
	11: endfor
	12: X0 = X + Dsum/Wsum
	13: if X0 falls outside the image domain then
	14:		find the pixel coordinate X0 closest to X0 on the boundary of the source image
	15:		update X0: X0 =X0c
	16: endif
	17: if X0 contains non-integer coordinate then
	18:		find the pixel coordinate X0 I by interpolating the neighbours of X0 and rounding the interpolation result
	19:		update X0: X0 =X0I
	20: endif
	21: copy the value of the pixel at X0 to that of the pixel at X: D(X) = S(X0)
	22:endfor
*/
