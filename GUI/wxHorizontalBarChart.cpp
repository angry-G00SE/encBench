#include <wx/wxprec.h>
#include <string>
// if not possible then:
#ifndef WX_PRECOMP
  #include <wx/wx.h>
#endif

#include "wxHorizontalBarChart.hpp"

wxBEGIN_EVENT_TABLE(wxHorizontalBarChart, wxWindow)
  //EVT_MOTION(wxHorizontalBarChart::OnMouseMoved)
  EVT_LEFT_DOWN(wxHorizontalBarChart::OnMouseDown)
  EVT_LEFT_UP(wxHorizontalBarChart::OnMouseReleased)
  //EVT_RIGHT_DOWN(wxHorizontalBarChart::OnRightClick)
  EVT_LEAVE_WINDOW(wxHorizontalBarChart::OnMouseLeftWindow)
  //EVT_KEY_DOWN(wxHorizontalBarChart::OnKeyPressed)
  //EVT_KEY_UP(wxHorizontalBarChart::OnKeyReleased)
  //EVT_MOUSEWHEEL(wxHorizontalBarChart::OnMouseWheelMoved)
  // put rest here when necessary

  // catch paint events
  EVT_PAINT(wxHorizontalBarChart::OnPaint)
wxEND_EVENT_TABLE()


wxHorizontalBarChart::wxHorizontalBarChart(wxWindow* parent) :
  wxWindow(parent, wxID_ANY), maxValue{0.0} {
  }

void wxHorizontalBarChart::OnPaint(wxPaintEvent& evt) {
  wxPaintDC dc(this);
  render(dc);
}

void wxHorizontalBarChart::paintNow() {
  wxClientDC dc(this);
  render(dc);
}



// where magic is done!
void wxHorizontalBarChart::render(wxDC& dc) {

  // setting-up drawing tools
  wxColor color = wxColor(78, 169, 200);
  wxBrush drawingBrush = wxBrush(color);
  dc.SetBrush(drawingBrush);
  dc.SetPen(wxNullPen);

  wxCoord maxW, maxH;
  dc.GetSize(&maxW, &maxH);

  // setting-up dynamic font style
  wxFont chartFont = wxFont(maxH*0.02, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_LIGHT);
  dc.SetFont(chartFont);

  // setting-up text color
  color = wxColor(120, 120, 120);
  dc.SetTextForeground(color);

  // setting-up background color
  dc.SetBackground(*wxWHITE_BRUSH);
  dc.Clear();

  // some simple percentage division of the graph
  wxCoord barHeight = (maxH - (maxH*0.2)) / (chartsVect.size()*2 + 1);
  wxCoord drawingFromH = maxH*0.1 + barHeight + 2*barHeight*chartsVect.size();
  wxCoord drawingFromW = maxW*0.2;


  // drawing X_Axis
  dc.SetPen(*wxLIGHT_GREY_PEN);
  wxCoord tempW = drawingFromW;       // so that the original value won't get lost - needed for later use
  dc.DrawLine(drawingFromW, drawingFromH, maxW*0.95, drawingFromH);
  for(int i=0; i<=10; i++) {

    // drawing numerical x_axis values
    wxCoord textW, textH;
    wxString numString = wxString::Format("%d", (int)( (i/10.0)*maxValue*0.75/0.7) );
    dc.GetTextExtent(numString, &textW, &textH);
    dc.DrawText(numString, tempW - textW/2, drawingFromH + 0.01*maxH);

    // drawing x_axis indicators
    dc.DrawLine(tempW, drawingFromH, tempW, drawingFromH + maxH*0.01);
    dc.DrawLine(tempW, drawingFromH, tempW, maxH*0.1);

    // add 10% of x_axis value each time
    tempW = drawingFromW + 0.75*maxW*((i+1)/10.0);     //  x_axis => 0.75maxW   ==> Y = 0.75maxW*X/x_axis
                                                       //  X      => Y
                                                       //  with x_axis = maxValue*0.75/0.7
  }


  // drawing Y_Axis
  wxCoord tempH = drawingFromH;
  dc.DrawLine(drawingFromW, tempH, drawingFromW, maxH*0.1);
  for (int i=0; i<chartsVect.size()+1; i++) {

    if (i == 0) {

      dc.DrawLine(drawingFromW, tempH, drawingFromW - 0.01*maxW, tempH);
      tempH -= 2.5*barHeight;

    } else if (i == chartsVect.size()-1) {

      dc.DrawLine(drawingFromW, tempH, drawingFromW - 0.01*maxW, tempH);
      tempH -= 2.5*barHeight;

    } else {

      dc.DrawLine(drawingFromW, tempH, drawingFromW - 0.01*maxW, tempH);
      tempH -= 2*barHeight;

    }

  }

  // drawing labels + Graph Title
  chartFont.SetWeight(wxFONTWEIGHT_BOLD);
  dc.SetFont(chartFont);

  wxCoord textW, textH;
  dc.GetTextExtent(labelX, &textW, &textH);

  dc.DrawText(labelX, maxW*0.575 - textW/2, maxH*0.95);

  dc.GetTextExtent(labelY, &textW, &textH);
  dc.DrawRotatedText(labelY, maxW*0.05, maxH*0.45 + textW/2, 90);

  dc.GetTextExtent(title, &textW, &textH);
  dc.DrawText(title, maxW*0.55 - textW/2, maxH*0.05);

  // drawing bars + cooresponding strings
  drawingFromH = maxH*0.1 + barHeight;
  for( const wxBar& c : chartsVect ) {
    wxCoord textW, textH;
    dc.GetTextExtent(c.name, &textW, &textH);

    dc.DrawRectangle(drawingFromW, drawingFromH, (c.value/maxValue)*0.7*maxW, barHeight);
    dc.DrawText(c.name, drawingFromW - textW - maxW*0.01, drawingFromH  + barHeight/2 - textH/2);
    drawingFromH += 2 * barHeight;
  }

} // where magic ends!





void wxHorizontalBarChart::OnMouseDown(wxMouseEvent& evt) {
  paintNow();
}

void wxHorizontalBarChart::OnMouseReleased(wxMouseEvent& evt) {
  paintNow();

  wxMessageBox(wxT("You pressed a custom button!"));
}

void wxHorizontalBarChart::OnMouseLeftWindow(wxMouseEvent& evt) {
    paintNow();
}


void wxHorizontalBarChart::AddChart(const wxBar& chart) {
  chartsVect.push_back(chart);
}

void wxHorizontalBarChart::AddChart(const wxString& name, double value) {
  wxBar newChart = {name, value};
  chartsVect.push_back(newChart);
  //paintNow();
}

void wxHorizontalBarChart::UpdateMax() {
  for (const wxBar& c : chartsVect) {
    if (c.value > maxValue)
      maxValue = c.value;
  }
}

void wxHorizontalBarChart::SetXAxisLabel(const wxString& text) {
  labelX = text;
}

void wxHorizontalBarChart::SetYAxisLabel(const wxString& text) {
  labelY = text;
}

void wxHorizontalBarChart::SetTitle(const wxString& text) {
  title = text;
}

void wxHorizontalBarChart::ClearChart() {
  chartsVect.clear();
  maxValue = 0.0;
}
