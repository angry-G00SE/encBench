#include <wx/wxprec.h>
#include <wx/listctrl.h>
#include <wx/splitter.h>
#include <wx/srchctrl.h>
#include <wx/thread.h>
#include <wx/collpane.h>

#include <string>
#include <memory>
#include <unordered_set>
// if not possible then:
#ifndef WX_PRECOMP
  #include <wx/wx.h>
#endif

#if defined(_WIN32)
#elif defined(__linux__)
  #include "pstream.h"
#elif defined(__APPLE__) && defined(__MACH__)
#endif

#include "wxGoBenchOptions.hpp"
#include "benchmarkGUI.hpp"
#include "wxHorizontalBarChart.hpp"


wxIMPLEMENT_APP(MyApp);

wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
  EVT_MENU(wxID_EXIT, MyFrame::OnExit)
  EVT_MENU(wxID_ABOUT, MyFrame::OnAbout)
  EVT_BUTTON(ID_RUN_BENCHMARK, MyFrame::runBenchmark)
  EVT_BUTTON(ID_DRAW_GRAPH, MyFrame::OnDraw)
  EVT_SEARCHCTRL_SEARCH_BTN(ID_SEARCH_BOX, MyFrame::OnSearch)
  EVT_COLLAPSIBLEPANE_CHANGED(ID_COLLAPSIBLE_PANE, MyFrame::OnCollapsiblePaneChange)
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
  left = new wxPanel(main_splitter);
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

  addListItem(123, "MD5", "description here!");
  addListItem(456, "SHA1", "2nd item!");
  addListItem(789, "SHA224", "3rd item!");

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
  // setting-up collapsible pane
  //////////////////////////////////////////////////////////////////////////////////////////////////////
  wxCollapsiblePane* options_pane = new wxCollapsiblePane(left, ID_COLLAPSIBLE_PANE, wxString("Tool Option(s)"));
  wxWindow* options_pane_win = options_pane->GetPane();

  //////////////////////////////////////////////////////////////////////////////////////////////////////
  // setting-up language-dependant benchmark tool options
  //////////////////////////////////////////////////////////////////////////////////////////////////////
  // currently only Go
  go_options_win = new wxGoBenchOptions(options_pane_win);

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
  wxBoxSizer* paneSz = new wxBoxSizer(wxVERTICAL);
  paneSz->Add(go_options_win, 1, wxEXPAND);
  options_pane_win->SetSizer(paneSz);
  paneSz->SetSizeHints(options_pane_win);


  wxBoxSizer* chart_sizer = new wxBoxSizer(wxVERTICAL);
  chart_sizer->Add(chart_test, 1, wxEXPAND);
  upper_mid->SetSizerAndFit(chart_sizer);

  wxBoxSizer* main_sizer = new wxBoxSizer(wxVERTICAL);
  main_sizer->Add(searchBox, 0, wxEXPAND | wxTOP | wxLEFT | wxRIGHT, 5);
  main_sizer->Add(algoSelectionList, 1, wxEXPAND | wxALL, 5);
  main_sizer->Add(options_pane, 0, wxEXPAND | wxBOTTOM, 5);
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

    std::vector<wxString> results = getGOBenchResults(itemName);

    addAlgorithmBenchResult(itemName, results[2], results[3], results[0], results[1]);

    index = algoSelectionList->GetNextSelected(index);
    while (index != -1) {

      itemName = algoSelectionList->GetItemText(index, 1);
      removeDuplicate(itemName);

      results = getGOBenchResults(itemName);

      addAlgorithmBenchResult(itemName, results[2], results[3], results[0], results[1]);
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


///////////////////////////////////////////////////////////////////////////////////////
// this function generates a GOLANG command to benchmark method with provided option(s)
//
// method: encryption method name
//
// returns a wxString (std::string )
///////////////////////////////////////////////////////////////////////////////////////
std::string MyFrame::getGOCommand(const wxString& method) {

  wxString cmd;
  cmd << "go run benchmarks/bench.go -algorithm=" << method << " ";

  // if user chose nbr-of-iteration or time as input, if data is entered, use it. Else use default option.
  switch(go_options_win->GetInputChoiceRadio()->GetSelection()) {

    // iterations selected.
    case 1: // append cmd with: go_options_win->inputIterChoice->GetStringSelection();
            {
            cmd << "-iterations=" << go_options_win->inputIterChoice->GetStringSelection() << " ";
            break;
            }

    // time selected
    case 2: {
            if (!go_options_win->GetInputTime()->IsEmpty())
              // append cmd with: go_options_win->inputTime->GetValue();
              cmd << "-time=" << go_options_win->inputTime->GetValue() << " ";
            break;
            }

    // else use default options
    default: break;
  }

  // if user selected something use the selected option, else use default option
  if( go_options_win->GetInputSize()->GetSelection() != 0) {
    cmd << "-size=" << go_options_win->GetInputSize()->GetStringSelection() << " ";
  }

  // if user provided the number of threads use the provided number, else use default option
  if (!go_options_win->GetInputThreads()->IsEmpty()) {
    // append cmd with: go_options_win->inputThreads->GetValue();
    cmd << "-threads=" << go_options_win->inputThreads->GetValue();
  }

  // if user provided the number of times to run the benchmark use the provided number, else use default option
  if (!go_options_win->GetInputBench()->IsEmpty()) {
    // append cmd with: go_options_win->inputBrench->GetValue();
  }

  return cmd.ToStdString();
}

std::vector<wxString> MyFrame::getGOBenchResults(const wxString& method) {

  std::vector<wxString> results;

  // verifying platform and populating an std::vector with benchmark results
  #if defined(_WIN32)
  #elif defined(__linux__)
    redi::ipstream in(getGOCommand(method), redi::pstreams::pstdout);
    std::string outputLine;

    int i = 1;
    while(std::getline(in, outputLine)) {
      // just to skip first 4 lines
      if (i>4)  {
        results.push_back(wxString(outputLine));
        std::cout << outputLine << std::endl;
      }
      else
        ++i;
    }
  #elif defined(__APPLE__) && defined(__MACH__)
  #endif

  return results;
}

void MyFrame::OnCollapsiblePaneChange(wxCollapsiblePaneEvent& event) {
  left->Layout();
}
