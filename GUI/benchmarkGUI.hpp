#ifndef BENCHMARK_GUI
#define BENCHMARK_GUI

enum {
  ID_RUN_BENCHMARK = 0,
  ID_SEARCH_BOX = 1,
  ID_DRAW_GRAPH = 2,
  ID_UNSELECT_ALL = 3,
  ID_GRAPH_CHOICE = 4,
  ID_DATA_CHOICE = 5,
  ID_BYTE_CHOICE = 6,
  ID_DEFAULT_CB = 7,
  ID_TIME_CB = 8,
  ID_ITERATION_CB = 9,
  ID_COLLAPSIBLE_PANE = 10
};

enum input {
  NBR_ITERATION,
  TIME,
  NONE
};

enum language {
  C,
  CPP,
  CS,
  PYTHON,
  GO
};

class MyApp : public wxApp
{
public:
  virtual bool OnInit();
};

class MyFrame : public wxFrame
{
public:
  MyFrame(const wxString&, const wxPoint&, const wxSize&);
private:
  // these are declared here because these will be called outside MyFrame constructor
  wxListView* algoSelectionList;
  wxListView* algoInfoList;
  wxPanel* left;
  wxGoBenchOptions* go_options_win;
  wxHorizontalBarChart* chart_graph;
  wxChoice* dataType;

  language currentLanguge;

  void addListItem(const std::string& method, const std::string& description);
  void runBenchmark(wxCommandEvent&);

  void addAlgorithmBenchResult( const wxString&, const wxString&, const wxString&, const wxString&,
                                const wxString&);
  void addAlgorithmBenchResult(const std::vector<wxString>&);

  void populateAlgoList();

  void removeDuplicate(long);
  void removeDuplicate(wxString);
  void OnSearch(wxCommandEvent&);
  void OnDraw(wxCommandEvent&);
  void OnUnselect(wxCommandEvent&);

  // functions to generate commands for supported languages
  std::string getGOCommand(const wxString& method);
  //std::string getCPPCommand(const wxString&);
  //std::string getCSCommmand(const wxString&);
  //std::string getPythonCommand(const wxString&);

  std::vector<wxString> getGOBenchResults(const wxString& method);
  //std::vector<wxString> getCPPBenchResults(const wxString& method);
  //std::vector<wxString> getCSBenchResults(const wxString& method);
  //std::vector<wxString> getPythonBenchmarkResults(const wxString& method);

  struct ItemData
  {
    std::string name;
    std::string description;
  };

  void sortByColumn(int);
  int sortDirection = 1;

  std::unordered_set<std::unique_ptr<ItemData>> itemDataSet;

  static int compareInts(int, int, int);
  static int compareStrings(const std::string&, const std::string&, int);

  static int nameSortCallBack(wxIntPtr item1, wxIntPtr item2, wxIntPtr direction);
  static int descSortCallBack(wxIntPtr item1, wxIntPtr item2, wxIntPtr directinon);

  //  Menu Event handlers
  void OnAbout(wxCommandEvent&);
  void OnExit(wxCommandEvent&);

  // more event handles
  void OnCollapsiblePaneChange(wxCollapsiblePaneEvent& event);

  // event table attached to this window/class
  wxDECLARE_EVENT_TABLE();

};
#endif
