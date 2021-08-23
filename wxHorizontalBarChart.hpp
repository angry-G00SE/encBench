#ifndef WX_HORIZONTAL_BAR_CHART
#define WX_HORIZONTAL_BAR_CHART

struct wxBar {
  wxString name;
  double value;
};

class wxHorizontalBarChart : public wxWindow {

private:
  std::vector<wxBar> chartsVect;    // Vector holding charts

  wxString title;             // title of the chart
  wxString labelX, labelY;    // Put Unit between parenthesis here
  double maxValue;            // usefull for determining horizontal expansion + x_axis scale

  const int minWidth = 350;
  const int minHeight = 150;

  wxFont chartFont;

public:
  wxHorizontalBarChart(wxFrame* parent);

  void paintNow();

  void render(wxDC& dc);

  // adds a chart corresponding to method name with a double value
  void AddChart(const wxString&, double);
  void AddChart(const wxBar&);

  // Setting up Axis labels
  void SetXAxisLabel(const wxString&);
  void SetYAxisLabel(const wxString&);

  void SetTitle(const wxString&);
  void SetChartFont();
  // update max value
  void UpdateMax();

  // paint event handler
  void OnPaint(wxPaintEvent& evt);

  // probably some usefull events
  void OnMouseMoved(wxMouseEvent& event);
  void OnMouseDown(wxMouseEvent& event);
  void OnMouseWheelMoved(wxMouseEvent& event);
  void OnMouseReleased(wxMouseEvent& event);
  void OnRightClick(wxMouseEvent& event);
  void OnMouseLeftWindow(wxMouseEvent& event);
  void OnKeyPressed(wxKeyEvent& event);
  void OnKeyReleased(wxKeyEvent& event);

  DECLARE_EVENT_TABLE()
};
#endif
