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
  ID_COLLAPSIBLE_PANE = 10,
  ID_UNSELECT_ALL_2 = 11,
  ID_REMOVE_SELECTED = 12,
  ID_CLEAR = 13
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

class MyFrame : public wxFrame {
public:

  MyFrame(const wxString&, const wxPoint&, const wxSize&);

private:

  // useful for storing list item data
  struct ItemData
  {
    std::string name;
    std::string description;
  };

  // container for algoSelectionList's items data
  std::unordered_set<std::unique_ptr<ItemData>> itemDataSet;

  // to indicate sort direction for lists
  int sortDirection = 1;
  unsigned int maxThreads;

  // these are declared here because these will be called outside MyFrame constructor
  wxListView* algoSelectionList;
  wxListView* algoInfoList;
  wxPanel* left;
  wxGoBenchOptions* go_options_win;
  wxHorizontalBarChart* chart_graph;
  wxChoice* dataType;
  wxTextCtrl* sysInfo;

  // indicate which language is currently selected
  language currentLanguge;

  void addListItem(const std::string& method, const std::string& description);

  void addAlgorithmBenchResult( const wxString&, const wxString&, const wxString&, const wxString&,
                                const wxString&);
  void addAlgorithmBenchResult(const std::vector<wxString>&);

  void populateAlgoList();
  void populateSystemInfo();

  void removeDuplicate(long);
  void removeDuplicate(wxString);

  // events
  void OnSearch(wxCommandEvent&);
  void OnDraw(wxCommandEvent&);
  void OnUnselect(wxCommandEvent&);
  void OnUnselect2(wxCommandEvent&);
  void OnRemoveSelected(wxCommandEvent&);
  void OnClear(wxCommandEvent&);
  void OnAbout(wxCommandEvent&);
  void OnExit(wxCommandEvent&);
  void OnCollapsiblePaneChange(wxCollapsiblePaneEvent& event);
  void runBenchmark(wxCommandEvent&);


  // functions to generate commands for supported languages
  std::string getGOCommand(const wxString& method);
  //std::string getCPPCommand(const wxString&);
  //std::string getCSCommmand(const wxString&);
  //std::string getPythonCommand(const wxString&);

  std::vector<wxString> getGOBenchResults(const wxString& method);
  //std::vector<wxString> getCPPBenchResults(const wxString& method);
  //std::vector<wxString> getCSBenchResults(const wxString& method);
  //std::vector<wxString> getPythonBenchmarkResults(const wxString& method);

  void sortByColumn(int);

  static int compareInts(int, int, int);
  static int compareStrings(const std::string&, const std::string&, int);

  static int nameSortCallBack(wxIntPtr item1, wxIntPtr item2, wxIntPtr direction);
  static int descSortCallBack(wxIntPtr item1, wxIntPtr item2, wxIntPtr directinon);

  // event table attached to this window/class
  wxDECLARE_EVENT_TABLE();

};
#endif
