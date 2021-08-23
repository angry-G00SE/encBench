#ifndef BENCHMARK_GUI
#define BENCHMARK_GUI

enum {
  ID_RUN_BENCHMARK = 0,
  ID_SEARCH_BOX = 1,
  ID_DRAW_GRAPH = 2,
  ID_UNSELECT_ALL = 3,
  ID_GRAPH_CHOICE = 4,
  ID_DATA_CHOICE = 5,
  ID_BYTE_CHOICE = 6
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
  wxTextCtrl* threadsInput;
  wxTextCtrl* benchCountInput;
  wxChoice* inputSizeChoice;

  wxString threads;         // user input for number of threads to use
  wxString benchCount;      // user input for number of times the benchmark will be executed
                            // on the corresponding encryption method

  input providedInput;

  language currentLanguge;

  void addListItem(int, const std::string&, const std::string&);
  void runBenchmark(wxCommandEvent&);

  std::vector<wxString> getBenchResults(const wxString&);

  void addAlgorithmBenchResult( const wxString&, const wxString&, const wxString&, const wxString&,
                                const wxString&);
  void addAlgorithmBenchResult(const std::vector<wxString>&);


  void removeDuplicate(long);
  void removeDuplicate(wxString);
  void OnSearch(wxCommandEvent&);
  void OnDraw(wxCommandEvent&);

  // functions to generate commands for supported languages
  wxString getGOCommand(const wxString&);
  //wxString getCPPCommand(const wxString&);
  //wxString getCSCommmand(const wxString&);
  //wxString getPythonCommand(const wxString&);

  std::vector<wxString> getGOBenchResults(const wxString&, const wxString&);
  //std::vector<wxString> getCPPBenchResults(const wxString&, const wxString&);
  //std::vector<wxString> getCSBenchResults(const wxString&, const wxString&);
  //std::vector<wxString> getPythonBenchmarkResults(const wxString&, const wxString&);

  struct ItemData
  {
    int id;
    std::string name;
    std::string description;
  };

  void sortByColumn(int);
  int sortDirection = 1;

  std::unordered_set<std::unique_ptr<ItemData>> itemDataSet;

  static int compareInts(int, int, int);
  static int compareStrings(const std::string&, const std::string&, int);

  static int idSortCallBack(wxIntPtr item1, wxIntPtr item2, wxIntPtr direction);
  static int nameSortCallBack(wxIntPtr item1, wxIntPtr item2, wxIntPtr direction);
  static int descSortCallBack(wxIntPtr item1, wxIntPtr item2, wxIntPtr directinon);

  //  Menu Event handlers
  void OnAbout(wxCommandEvent&);
  void OnExit(wxCommandEvent&);

  // event table attached to this window/class
  wxDECLARE_EVENT_TABLE();

};
#endif
