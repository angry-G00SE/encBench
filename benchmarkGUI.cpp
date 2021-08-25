#include <wx/wxprec.h>
#include <wx/listctrl.h>
#include <wx/splitter.h>
#include <wx/srchctrl.h>
#include <wx/thread.h>

#include <string>
#include <memory>
#include <unordered_set>
// if not possible then:
#ifndef WX_PRECOMP
  #include <wx/wx.h>
#endif

#include "benchmarkGUI.hpp"
#include "wxHorizontalBarChart.hpp"
#include "wxGoBenchOptions.hpp"

#if defined(_WIN32)
#elif defined(__linux__)
  #include "pstream.h"
#elif defined(__APPLE__) && defined(__MACH__)
#endif


wxIMPLEMENT_APP(MyApp);

wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
  EVT_MENU(wxID_EXIT, MyFrame::OnExit)
  EVT_MENU(wxID_ABOUT, MyFrame::OnAbout)
  EVT_BUTTON(ID_RUN_BENCHMARK, MyFrame::runBenchmark)
  EVT_BUTTON(ID_DRAW_GRAPH, MyFrame::OnDraw)
  EVT_SEARCHCTRL_SEARCH_BTN(ID_SEARCH_BOX, MyFrame::OnSearch)
wxEND_EVENT_TABLE()

bool MyApp::OnInit() {
  MyFrame* frame = new MyFrame("Crypto Benchmark", wxDefaultPosition, wxDefaultSize);
  frame->Show(true);

  return true;
}

MyFrame::MyFrame(const wxString& name, const wxPoint& pos, const wxSize& size)
        : wxFrame(nullptr, wxID_ANY, name, pos, size) {

  SetMinSize(wxSize(640, 480));
  currentLanguge = language::GO;    // currently only Go

  ////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Setting Menu bar and Status bar
  ////////////////////////////////////////////////////////////////////////////////////////////////////////
  wxMenu* menuFile = new wxMenu;
  menuFile->Append(wxID_EXIT);

  wxMenu* menuHelp = new wxMenu;
  menuHelp->Append(wxID_ABOUT);

  wxMenuBar* menuBar = new wxMenuBar;
  menuBar->Append(menuFile, "&File");
  menuBar->Append(menuHelp, "&Help");

  SetMenuBar(menuBar);

  CreateStatusBar();
  SetStatusText("Welcome to Crypto Benchmark!");


  ////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Splitters' declaration and definition
  ////////////////////////////////////////////////////////////////////////////////////////////////////////
  wxSplitterWindow* main_splitter = new wxSplitterWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                                          wxSP_BORDER | wxSP_LIVE_UPDATE);

  wxSplitterWindow* right_splitter = new wxSplitterWindow(main_splitter, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                                          wxSP_BORDER | wxSP_LIVE_UPDATE);

  wxSplitterWindow* upper_right_splitter = new wxSplitterWindow(right_splitter, wxID_ANY, wxDefaultPosition,
                                                                wxDefaultSize, wxSP_BORDER | wxSP_LIVE_UPDATE);

  ///////////////////////////////////////////////////////////////////////////////////////////////////////
  // Setting-up Panels
  ///////////////////////////////////////////////////////////////////////////////////////////////////////
  wxPanel* left = new wxPanel(main_splitter);
  wxPanel* bottom_right = new wxPanel(right_splitter);
  wxPanel* upper_mid = new wxPanel(upper_right_splitter);
  wxPanel* top_right_right = new wxPanel(upper_right_splitter);

  //////////////////////////////////////////////////////////////////////////////////////////////////////
  // Splitters Configuration
  //////////////////////////////////////////////////////////////////////////////////////////////////////
  upper_right_splitter->SetMinimumPaneSize(100);
  upper_right_splitter->SplitVertically(upper_mid, top_right_right);
  upper_right_splitter->SetSashGravity(1);

  right_splitter->SetSashGravity(0.8);
  right_splitter->SetMinimumPaneSize(100);
  right_splitter->SplitHorizontally(upper_right_splitter, bottom_right);

  main_splitter->SetSashGravity(0);
  main_splitter->SetMinimumPaneSize(200);
  main_splitter->SplitVertically(left, right_splitter);

  //////////////////////////////////////////////////////////////////////////////////////////////////////
  // Setting-up list of Cryptography Algorithms
  //////////////////////////////////////////////////////////////////////////////////////////////////////
  algoSelectionList = new wxListView(left);
  algoSelectionList->AppendColumn("ID");
  algoSelectionList->AppendColumn("Name");
  algoSelectionList->AppendColumn("Description");

  algoSelectionList->SetColumnWidth(0, 40);
  algoSelectionList->SetColumnWidth(1, 80);
  algoSelectionList->SetColumnWidth(2, 150);

  algoSelectionList->Bind(wxEVT_LIST_COL_CLICK, [this](wxListEvent& evt) {
    this->sortByColumn(evt.GetColumn());
  });

  addListItem(123, "some item", "description here!");
  addListItem(456, "name here", "2nd item!");
  addListItem(789, "name here again", "3rd item!");

  //////////////////////////////////////////////////////////////////////////////////////////////////////
  // Initializing Selected Algorithm Info list
  //////////////////////////////////////////////////////////////////////////////////////////////////////
  algoInfoList = new wxListView(bottom_right);
  algoInfoList->AppendColumn("Method");
  algoInfoList->AppendColumn("Median");
  algoInfoList->AppendColumn("Nbr Iteration");
  algoInfoList->AppendColumn("Nbr Alloc");
  algoInfoList->AppendColumn("Byte per Alloc");

  //////////////////////////////////////////////////////////////////////////////////////////////////////
  // Setting-up  Search Box
  //////////////////////////////////////////////////////////////////////////////////////////////////////
  wxSearchCtrl* searchBox = new wxSearchCtrl( left, ID_SEARCH_BOX, wxEmptyString, wxDefaultPosition,
                                              wxDefaultSize, wxTE_PROCESS_ENTER);
  searchBox->SetDescriptiveText("Search Algorithm by Name");

  //////////////////////////////////////////////////////////////////////////////////////////////////////
  // Setting-up  Buttons
  //////////////////////////////////////////////////////////////////////////////////////////////////////
  wxButton* runBench = new wxButton(left, ID_RUN_BENCHMARK, "Run Benchmark");
  wxButton* drawGraphs = new wxButton(bottom_right, ID_DRAW_GRAPH, "Draw");
  wxButton* unselectAll = new wxButton(bottom_right, ID_UNSELECT_ALL, "Unselect All");

  //////////////////////////////////////////////////////////////////////////////////////////////////////
  // Setting-up Choice Boxes
  //////////////////////////////////////////////////////////////////////////////////////////////////////
  wxChoice* dataType = new wxChoice(bottom_right, ID_DATA_CHOICE);
  dataType->Insert("Median", 0);
  dataType->Insert("Nbr Alloc", 1);
  dataType->Insert("Byte per Alloc", 2);
  dataType->SetSelection(0);

  wxChoice* graphType = new wxChoice(bottom_right, ID_GRAPH_CHOICE);
  graphType->Insert("Barchart", 0);
  graphType->Insert("Boxplot", 1);
  graphType->SetSelection(0);

  //////////////////////////////////////////////////////////////////////////////////////////////////////
  // testing chart library
  //////////////////////////////////////////////////////////////////////////////////////////////////////
  wxHorizontalBarChart* chart_test = new wxHorizontalBarChart(upper_mid);

  wxString s1 = "Yo Mama!";
  wxString s2 = "HEEEEEEY";
  wxString s3 = "Holyyyy!";
  wxString s4 = "hihihihi!";

  chart_test->AddChart(s1, 35.8);
  chart_test->AddChart(s2, 120.8);
  chart_test->AddChart(s3, 180.4);
  chart_test->AddChart(s3, 100);
  chart_test->AddChart(s4, 200);
  chart_test->AddChart(s1, 10);
  chart_test->AddChart(s2, 400);
  chart_test->AddChart(wxT("some method"), 300);
  chart_test->AddChart(wxT("another method"), 240);

  chart_test->UpdateMax();

  chart_test->SetXAxisLabel(wxT("Time (ms)"));
  chart_test->SetYAxisLabel(wxT("Method Name"));
  chart_test->SetTitle(wxT("Time Mean"));

  //////////////////////////////////////////////////////////////////////////////////////////////////////
  // Setting-up  Sizers
  //////////////////////////////////////////////////////////////////////////////////////////////////////
  wxGoBenchOptions* test_window = new wxGoBenchOptions(left);

  wxBoxSizer* chart_sizer = new wxBoxSizer(wxVERTICAL);
  chart_sizer->Add(chart_test, 1, wxEXPAND);
  upper_mid->SetSizerAndFit(chart_sizer);

  wxBoxSizer* main_sizer = new wxBoxSizer(wxVERTICAL);
  main_sizer->Add(searchBox, 0, wxEXPAND | wxTOP | wxLEFT | wxRIGHT, 5);
  main_sizer->Add(algoSelectionList, 1, wxEXPAND | wxALL, 5);
  main_sizer->Add(test_window, 0, wxEXPAND);
  main_sizer->Add(runBench, 0, wxALIGN_RIGHT | wxRIGHT | wxBOTTOM, 5);
  left->SetSizerAndFit(main_sizer);

  wxBoxSizer* nd_bottom_right_sizer = new wxBoxSizer(wxHORIZONTAL);
  nd_bottom_right_sizer->Add(drawGraphs, 1, wxALIGN_BOTTOM | wxBOTTOM | wxRIGHT, 5);

  wxBoxSizer* nd_right_sizer = new wxBoxSizer(wxVERTICAL);
  nd_right_sizer->Add(dataType, 0, wxEXPAND | wxRIGHT | wxTOP | wxBOTTOM, 5);
  nd_right_sizer->Add(graphType, 0, wxEXPAND | wxRIGHT | wxBOTTOM, 5);
  nd_right_sizer->Add(unselectAll, 0, wxEXPAND | wxBOTTOM | wxRIGHT, 5);
  nd_right_sizer->Add(nd_bottom_right_sizer, 1, wxEXPAND);
  //nd_right_sizer->Add(drawGraphs, 1);

  wxBoxSizer* nd_sizer = new wxBoxSizer(wxHORIZONTAL);
  nd_sizer->Add(algoInfoList, 1, wxEXPAND | wxALL, 5);
  nd_sizer->Add(nd_right_sizer, 0, wxEXPAND);
  bottom_right->SetSizerAndFit(nd_sizer);

}

void MyFrame::addListItem(int id, const std::string& name, const std::string& description) {
  int index = algoSelectionList->GetItemCount();

  algoSelectionList->InsertItem(index, std::to_string(id));
  algoSelectionList->SetItem(index, 1, name);
  algoSelectionList->SetItem(index, 2, description);

  ItemData data {id, name, description};
  auto dataPtr = std::make_unique<ItemData>(data);

  algoSelectionList->SetItemData(index, reinterpret_cast<wxIntPtr>(dataPtr.get()));

  itemDataSet.insert(std::move(dataPtr));
}

void MyFrame::sortByColumn(int index) {
  switch (index) {
    case 0: algoSelectionList->SortItems(&idSortCallBack, sortDirection);
            break;
    case 1: algoSelectionList->SortItems(&nameSortCallBack, sortDirection);
            break;
    case 2: algoSelectionList->SortItems(&descSortCallBack, sortDirection);
            break;

    default: break;
  }
  this->sortDirection = -this->sortDirection;
}

int MyFrame::compareInts(int id1, int id2, int direction) {
  return id1 == id2 ? 0 : (id1<id2 ? -direction: direction);
}

int MyFrame::compareStrings(const std::string& s1, const std::string& s2, int direction) {
  return s1.compare(s2) * direction;
}

int MyFrame::idSortCallBack(wxIntPtr item1, wxIntPtr item2, wxIntPtr direction) {
  return compareInts(reinterpret_cast<ItemData*>(item1)->id, reinterpret_cast<ItemData*>(item2)->id,
              static_cast<int>(direction));
}

int MyFrame::nameSortCallBack(wxIntPtr item1, wxIntPtr item2, wxIntPtr direction) {
  return compareStrings(reinterpret_cast<ItemData*>(item1)->name, reinterpret_cast<ItemData*>(item2)->name,
                static_cast<int>(direction));
}

int MyFrame::descSortCallBack(wxIntPtr item1, wxIntPtr item2, wxIntPtr direction) {
  return compareStrings( reinterpret_cast<ItemData*>(item1)->description,
                  reinterpret_cast<ItemData*>(item2)->description,
                  static_cast<int>(direction));
}

void MyFrame::OnExit(wxCommandEvent& evt) {
  Close(true);
}

void MyFrame::OnAbout(wxCommandEvent& evt) {
  wxMessageBox( "This is a benchmark tool to estimate the performance of certain cryptography algorithms",
                "About Crypto Benchmark", wxOK | wxICON_INFORMATION);
}

void MyFrame::addAlgorithmBenchResult(const wxString& name, const wxString& median,
                                      const wxString& nbrIter, const wxString& nbrAlloc,
                                      const wxString& bytePerAlloc) {
    long index = algoInfoList->GetItemCount();
    algoInfoList->InsertItem(index, name);
    algoInfoList->SetItem(index, 1, median);
    algoInfoList->SetItem(index, 2, nbrIter);
    algoInfoList->SetItem(index, 3, nbrAlloc);
    algoInfoList->SetItem(index, 4, bytePerAlloc);
}

void MyFrame::removeDuplicate(wxString itemName) {
    long searchedIndex = algoInfoList->GetNextItem(-1);
    while(searchedIndex != -1) {
      wxString searchedName = algoInfoList->GetItemText(searchedIndex, 0);
      if (searchedName.IsSameAs(itemName, true)) {
        algoInfoList->DeleteItem(searchedIndex);
        break;
      }

      searchedIndex = algoInfoList->GetNextItem(searchedIndex);
    }
}

void MyFrame::removeDuplicate(long index) {
  // check if we found similar index thus removing old benchmark
  // result for that algorithm
  long searchedIndex = algoInfoList->GetNextItem(-1);
  while(searchedIndex != -1) {
    if (searchedIndex == index) {
      algoInfoList->DeleteItem(searchedIndex);
      break;
    }

    searchedIndex = algoInfoList->GetNextItem(searchedIndex);
  }
}

void MyFrame::runBenchmark(wxCommandEvent& evt) {

  long index = algoSelectionList->GetFirstSelected();
  // check whether there is at least one element selected
  if (index != -1) {

    wxString itemName = algoSelectionList->GetItemText(index, 1);
    removeDuplicate(itemName);

    // reading data here
    std::string cmdStr;               // a string to store the command

    // setting-up the command according to the chosen options
    //std::vector<std::string> v = getBenchResults(itemName);
    //addAlgorithmBenchResult(v);
    addAlgorithmBenchResult(algoSelectionList->GetItemText(index, 1),
                            "-", "-", "-", "-");

    index = algoSelectionList->GetNextSelected(index);
    while (index != -1) {

      itemName = algoSelectionList->GetItemText(index, 1);
      removeDuplicate(itemName);
      addAlgorithmBenchResult(algoSelectionList->GetItemText(index, 1),
                              "-", "-", "-", "-");
      index = algoSelectionList->GetNextSelected(index);
    }
  }
}

void MyFrame::OnSearch(wxCommandEvent& evt) {
  long currentIndex = algoSelectionList->GetNextItem(-1);
  while (currentIndex != -1) {
    if (algoSelectionList->GetItemText(currentIndex, 1).IsSameAs(evt.GetString(), false)) {
      algoSelectionList->Select(currentIndex);
      break;
    }
    currentIndex = algoSelectionList->GetNextItem(currentIndex);
  }
}

void MyFrame::OnDraw(wxCommandEvent& evt) {

}

// according to the default/selected options this function will populate a std::vector
// with benchmark results of method
// method : corresponding method's name
std::vector<wxString> MyFrame::getBenchResults(const wxString& method) {

  // setting-up the command and parsing benchmark results
  switch(currentLanguge) {

    case GO : return getGOBenchResults(method, getGOCommand(method));

    // these are not yet implemented
    //case CPP : return getCPPBenchResults(method, getCPPCommand(method));

    //case CS : return getCSBenchResults(method, getCSCommmand(method));

    //case PYTHON : return getPythonBenchmarkResults(method, getPythonCommand(method));

    default : return std::vector<wxString>();

  }
}


///////////////////////////////////////////////////////////////////////////////////////
// this function generates a GOLANG command to benchmark method with provided option(s)
//
// method: encryption method name
//
// returns a wxString (std::string )
///////////////////////////////////////////////////////////////////////////////////////
wxString MyFrame::getGOCommand(const wxString& method) {

  wxString cmd;             // string that will hold the generated GOLANG command
  //wxString inputSize;       // user's chosen number of bytes if any

  //// if user selected something use the selected option, else use default option
  //if( inputSizeChoice->GetSelection() != wxNOT_FOUND) {
  //  inputSize = inputSizeChoice->GetStringSelection();
  //}

  //// if user provided the number of threads use the provided number, else use default option
  //if (!threads.IsEmpty()) {

  //}

  //// if user provided the number of times to run the benchmark use the provided number, else use default option
  //if (!benchCount.IsEmpty()) {

  //}

  //// generate GOLANG command here
  //cmd << "go " << "put options here and bla bla bla";

  return cmd;
}

std::vector<wxString> MyFrame::getGOBenchResults(const wxString& method, const wxString& cmd) {
  std::vector<wxString> v;
  // verifying platform and acting accordingly
  #if defined(_WIN32)
  #elif defined(__linux__)
    redi::ipstream process(cmd.ToStdString(), redi::pstreams::pstdout);
  #elif defined(__APPLE__) && defined(__MACH__)
  #endif
  return v;
}
