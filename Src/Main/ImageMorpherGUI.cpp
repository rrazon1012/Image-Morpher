#include "ImageMorpherGUI.h"
#include "../Image Drawing/ImagePanel.h"
#include <string>

ImageMorpherFrame::ImageMorpherFrame()
    : wxFrame(nullptr, wxID_ANY, "Image Morpher",wxDefaultPosition, wxSize(1000,1000))
{
    InitializeGUI();
}

void ImageMorpherFrame::InitializeGUI()
{
    wxMenu* menuFile = new wxMenu;
    menuFile->Append(wxID_Image1, "Left Image", wxString("Source image to be used for morphing"));
    menuFile->Append(wxID_Image2, "Right Image", wxString("Destination image to be used for morphing"));
    menuFile->Append(wxID_Morph, "Morph Images", wxString("Morph the two current images with the given lines"));
    menuFile->AppendSeparator();
    menuFile->Append(wxID_EXIT);

    wxMenu* menuHelp = new wxMenu;
    menuHelp->Append(wxID_ABOUT);

    wxMenuBar* menuBar = new wxMenuBar;
    menuBar->Append(menuFile, "&File");
    menuBar->Append(menuHelp, "&Help");
    SetMenuBar(menuBar);

    
    //main encompassing panel that holds everything
    wxPanel* wrapperPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize);
    
    //sizer for the main panel
    wxBoxSizer* wrapperSizer = new wxBoxSizer(wxVERTICAL);

    //two image panels that will hold the two pictures to be morphed
    leftImage = new ImagePanel(wrapperPanel, wxT("src/Images/Lynn-Posluns-640x480.jpg"), wxBITMAP_TYPE_JPEG, LeftPane);
    rightImage = new ImagePanel(wrapperPanel, wxT("src/Images/lisa-hischnik-ryzhaya.jpg"), wxBITMAP_TYPE_JPEG, RightPane);

    wxBoxSizer* imageSizer = new wxBoxSizer(wxHORIZONTAL);
    imageSizer->Add(leftImage, 1, wxALL|wxEXPAND,2);
    imageSizer->Add(rightImage, 1, wxALL|wxEXPAND, 2);

    wrapperSizer->Add(imageSizer,1, wxALL|wxEXPAND);

    wrapperPanel->SetSizerAndFit(wrapperSizer);
    this->Fit();

    CreateStatusBar();
    SetStatusText("Morph two images by selecting a source image and destination, adding lines for features, and morph by specifying the number of frames");

    //binding the events
    Bind(wxEVT_MENU, &ImageMorpherFrame::OnLoadImage, this, wxID_Image1);
    Bind(wxEVT_MENU, &ImageMorpherFrame::OnLoadImage, this, wxID_Image2);
    Bind(wxEVT_MENU, &ImageMorpherFrame::OnAbout, this, wxID_ABOUT);
    Bind(wxEVT_MENU, &ImageMorpherFrame::OnExit, this, wxID_EXIT);
    Bind(myEVT_DRAW, &ImageMorpherFrame::OnSyncLines, this, LeftPane);
    Bind(myEVT_DRAW, &ImageMorpherFrame::OnSyncLines, this, RightPane);
    Bind(wxEVT_MENU, &ImageMorpherFrame::OnMorph, this, wxID_Morph);


    //controls that are dynamically allocated are deallocated automatically
}

void ImageMorpherFrame::OnExit(wxCommandEvent& event)
{
    Close(true);
}

void ImageMorpherFrame::OnAbout(wxCommandEvent& event)
{
    wxMessageBox("This is an image morpher that uses the bier-neely algorithm (feature based) for morphing.",
        "About Image Morpher", wxOK | wxICON_INFORMATION);
}

/*
* this event is sent whenever an image panel (source/destination images) sends the custom event defined in drawevent
*/
void ImageMorpherFrame::OnSyncLines(DrawEvent& event)
{
    if (event.GetId() == LeftPane) {
        rightImage->drawnLines.push_back(event.getLine());
        rightImage->Refresh();
    }
    else {
        leftImage->drawnLines.push_back(event.getLine());
        leftImage->Refresh();
    }
    
}

void ImageMorpherFrame::OnMorph(wxCommandEvent& event)
{
    MorpherPanel* morphDialog = new MorpherPanel(this);
    if (leftImage->GetSize() == rightImage->GetSize()) {
        
        wxNumberEntryDialog* frameNumDialog = 
            new wxNumberEntryDialog(this, wxString(""), wxString("Enter desired number of frames"), wxString("L"), 4.0, 4.0, 20.0, wxDefaultPosition);
        
        if (frameNumDialog->ShowModal() == wxID_OK) { //a value was entered
            
            morphDialog->morphImage(leftImage->image, rightImage->image, leftImage->drawnLines, rightImage->drawnLines, frameNumDialog->GetValue());
            morphDialog->ShowModal();
        }
        
        frameNumDialog->Destroy();
    }
    else {
        wxMessageBox("Images are different sizes",
            "Warning", wxOK | wxICON_INFORMATION);
    }
    morphDialog->Destroy();
}

void ImageMorpherFrame::OnLoadImage(wxCommandEvent& event)
{
    //opengs a new file dialog and populates the arguments: parent, dialog text, default dir, default file, wildcard for file type, dialog style (open,save,etc.)
    wxFileDialog* imageDialog = new wxFileDialog(
        this, wxString("Choose an image to open"), wxEmptyString, wxEmptyString,
        wxString("PNG files (*.png)|*.png| JPEG files (*.jpg)|*.jpg"),
            wxFD_OPEN, wxDefaultPosition
    );

    if (imageDialog->ShowModal() == wxID_OK) {
        CurrentDocPath = imageDialog->GetPath();

        int id = event.GetId();
        
        wxBitmapType imageType = imageDialog->GetFilterIndex() == 0 ? wxBITMAP_TYPE_PNG : wxBITMAP_TYPE_JPEG;
        
        switch (id) { //determines which button sent the event and adds it to the correct image
        case wxID_Image1:
            leftImage->loadImage(CurrentDocPath, imageType); //add image type checking
            break;
        case wxID_Image2:
            rightImage->loadImage(CurrentDocPath, imageType);
            break;
        };
    }

    imageDialog->Destroy();
}