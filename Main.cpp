#include <wx/wx.h>
#include "ImageMorpherGUI.h"
#include <iostream>
#include <fstream>
enum {
    ID_Button = 1
};

class ImageMorpher : public wxApp {
public:
    bool OnInit() {
        wxInitAllImageHandlers();
        ImageMorpherFrame* morpherGUI = new ImageMorpherFrame();
        morpherGUI->Show(true);
        return true;
    }
};

wxIMPLEMENT_APP(ImageMorpher); 