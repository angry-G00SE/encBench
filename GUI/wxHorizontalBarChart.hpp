#ifndef WX_HORIZONTAL_BAR_CHART
#define WX_HORIZONTAL_BAR_CHART

struct wxBar {
  wxString name;
  double value;
};

class wxHorizontalBarChart : public wxWindow {

private:

  // container holding charts
  std::vector<wxBar> chartsVect;


  wxString title;             // title of the chart
  wxString labelX, labelY;    // Put Unit between parenthesis here
  double maxValue;            // usefull for determining horizontal expansion + x_axis scale

public:

  // constructor
  wxHorizontalBarChart(wxWindow* parent);

  // similar to OnPaint, except you can call it whenever you want
  void paintNow();

  // this is where all the drawing is done
  void render(wxDC& dc);

  // adds a chart corresponding to method name with a double value
  void AddChart(const wxString&, double);
  void AddChart(const wxBar&);

  // Setting up Axis labels and title
  void SetXAxisLabel(const wxString&);
  void SetYAxisLabel(const wxString&);
  void SetTitle(const wxString&);

  // update max value
  void UpdateMax();

  // removes/clears all benchmark results
  void ClearChart();

  // paint event handler
  void OnPaint(wxPaintEvent& evt);

  DECLARE_EVENT_TABLE()
};
#endif
