#include "ImagePanel.h"

wxDEFINE_EVENT(myEVT_DRAW, DrawEvent);

//events for drawing and editing lines
BEGIN_EVENT_TABLE(ImagePanel, wxPanel)
 // catch paint events
    EVT_PAINT(ImagePanel::paintEvent)
    //Size event
    EVT_SIZE(ImagePanel::OnSize)
    EVT_LEFT_DOWN(ImagePanel::OnLeftDown)
    EVT_LEFT_UP(ImagePanel::OnLeftUp)
    EVT_MOTION(ImagePanel::mouseMoved)
    EVT_DRAW_LINE(10,ImagePanel::OnAddLine)
END_EVENT_TABLE()

ImagePanel::ImagePanel(wxFrame* parent, wxString file, wxBitmapType format, int pID)
    : wxPanel(parent,wxID_ANY,wxDefaultPosition, wxDefaultSize)
{
    paneId = pID;
    // load the file... ideally add a check to see if loading was successful
    image.LoadFile(file, format);
    w = -1;
    h = -1;
    SetMinSize(image.GetSize());
    drawnLines.reserve(128);
    //MIGHT WANT to bind paint handler to paint event here
}

void ImagePanel::drawLine(wxDC& dc, wxPoint source, wxPoint end)
{
    dc.DrawLine(source, end);
    dc.DrawCircle(source, radius);
    dc.DrawCircle(end, radius);
}

ImagePanel::ImagePanel(wxPanel* parent, wxString file, wxBitmapType format, int pID)
    : wxPanel(parent)
{
    paneId = pID;
    image.LoadFile(file, format);
    w = -1;
    h = -1;
    SetMinSize(image.GetSize());
    drawnLines.reserve(128);
}

/*
 * Called by the system of by wxWidgets when the panel needs
 * to be redrawn. You can also trigger this call by
 * calling Refresh()/Update().
 */
void ImagePanel::paintEvent(wxPaintEvent& evt)
{
    // depending on your system you may need to look at double-buffered dcs
    wxPaintDC dc(this);
    render(dc);
}

/*
 * Alternatively, you can use a clientDC to paint on the panel
 * at any time. Using this generally does not free you from
 * catching paint events, since it is possible that e.g. the window
 * manager throws away your drawing when the window comes to the
 * background, and expects you will redraw it when the window comes
 * back (by sending a paint event).
 */
void ImagePanel::paintNow()
{
    // depending on your system you may need to look at double-buffered dcs
    wxClientDC dc(this); //temporary stack item, dont store as an object
    render(dc);
}

/*
 * Here we do the actual rendering. I put it in a separate
 * method so that it can work no matter what type of DC
 * (e.g. wxPaintDC or wxClientDC) is used.
 */
void ImagePanel::render(wxDC& dc)
{
    int neww, newh;
    dc.GetSize(&neww, &newh);

    if (neww != w || newh != h)
    {
        resized = wxBitmap(image.Scale(neww, newh /*, wxIMAGE_QUALITY_HIGH*/));
        w = neww;
        h = newh;
        dc.DrawBitmap(resized, 0, 0, false);
    }
    else {
        dc.DrawBitmap(resized, 0, 0, false);
    }

    //////explore drawing lines here

    if (drawnLines.size() != 0) {
        for (int i = 0; i < drawnLines.size(); i++) {
            drawLine(dc, drawnLines[i].start, drawnLines[i].end);
            dc.DrawCircle(drawnLines[i].start, radius);
            dc.DrawCircle(drawnLines[i].end, radius);
        }
    }
}

void ImagePanel::loadImage(wxString file, wxBitmapType format)
{

    image.LoadFile(file, format);
    w = -1;
    h = -1;
    SetMinSize(image.GetSize());
    
    //call refresh to redraw the panel to display new image.
    Refresh();
}

/*
* left mouse button down event.
* Checks if there is an existing line end at the click location
* to initiate either a move operation or a line createion
*/
void ImagePanel::OnLeftDown(wxMouseEvent& event)
{
    //default click operation is drawing
    isDragging = true;
    isDrawing = true;

    //add check for if the click hit an existing lines circle
    for (int i = 0; i < drawnLines.size(); i++) {
        if (Math::isPointWithinCircle(event.GetPosition(),drawnLines[i].start, radius))
        {
            currPoint = &drawnLines[i].start;
           // wxLogMessage("Clicked on start point");
            isDrawing = false;
            moveStart = true;
        }
        if (Math::isPointWithinCircle(event.GetPosition(), drawnLines[i].end, radius)) {
            currPoint = &drawnLines[i].end;
            //wxLogMessage("Clicked on end point");
            isDrawing = false;
            moveStart = false;
        }
    }

    if (isDrawing) {
        //dragging operation
        wxClientDC dc(this);
        render(dc);

        dc.DrawCircle(event.GetPosition().x, event.GetPosition().y, radius);
        dragSource.x = event.GetPosition().x, dragSource.y = event.GetPosition().y;
    }
    else {
        //moving operation
    }
    
    //start the dragging, may need to add a check later wether the dragging is for moving or drawing
}

void ImagePanel::mouseMoved(wxMouseEvent& event)
{
    if (isDragging) {
        wxClientDC dc(this);
        render(dc);
        if (isDrawing) {
            wxBrush redB = wxBrush(wxTheColourDatabase->Find("RED"));
            wxPen redP = wxPen(wxTheColourDatabase->Find("GOLD"), 2);

            dc.SetBrush(redB);
            dc.SetPen(redP);

            dc.DrawLine(dragSource.x, dragSource.y, event.GetPosition().x, event.GetPosition().y);
            dc.DrawCircle(dragSource.x, dragSource.y, radius);
        }
        else {
            currPoint->x = event.GetPosition().x, currPoint->y = event.GetPosition().y;
        }
    }
}

/*
* This event is called when the user ends the drag action which redraws the frame (check if this can be ommited to reduce draw calls)
* The line object is then created and added to the lines vector to keep track.
*/
void ImagePanel::OnLeftUp(wxMouseEvent& event)
{
    wxPoint dragEnd = event.GetPosition();
    if (isDrawing) {
        DrawEvent lineDrawEvent;
        lineDrawEvent.setLine(Line{ dragSource,dragEnd });
        lineDrawEvent.SetEventType(myEVT_DRAW);
        lineDrawEvent.SetId(10);
        ProcessWindowEvent(lineDrawEvent);
    }
    else {
        currPoint->x = dragEnd.x, currPoint->y = dragEnd.y;
        Refresh();
    }
    isDragging = false;
    isDrawing = false;
}

void ImagePanel::OnAddLine(DrawEvent& event) {
    drawnLines.push_back(event.getLine());
    Refresh();
    event.SetId(paneId);
    event.Skip();
}


/*
 * Here we call refresh to tell the panel to draw itself again.
 * So when the user resizes the image panel the image should be resized too.
 */
void ImagePanel::OnSize(wxSizeEvent& event) {
    Refresh();
    //skip the event.
    event.Skip();
}
