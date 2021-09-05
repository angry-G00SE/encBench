#include <wx/wxprec.h>
#include <wx/listctrl.h>
#include <wx/splitter.h>
#include <wx/srchctrl.h>
#include <wx/thread.h>
#include <wx/collpane.h>
#include <wx/spinctrl.h>


#include <fstream>
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
#include "wxHorizontalBarChart.hpp"
#include "benchmarkGUI.hpp"



wxIMPLEMENT_APP(MyApp);

wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
  EVT_MENU(wxID_EXIT, MyFrame::OnExit)
  EVT_MENU(wxID_ABOUT, MyFrame::OnAbout)
  EVT_BUTTON(ID_RUN_BENCHMARK, MyFrame::runBenchmark)
  EVT_BUTTON(ID_DRAW_GRAPH, MyFrame::OnDraw)
  EVT_BUTTON(ID_UNSELECT_ALL, MyFrame::OnUnselect)
  EVT_BUTTON(ID_UNSELECT_ALL_2, MyFrame::OnUnselect2)
  EVT_BUTTON(ID_REMOVE_SELECTED, MyFrame::OnRemoveSelected)
  EVT_BUTTON(ID_CLEAR, MyFrame::OnClear)
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
  wxPanel* right_top_right = new wxPanel(upper_right_splitter);

  //////////////////////////////////////////////////////////////////////////////////////////////////////
  // Splitters Configuration
  //////////////////////////////////////////////////////////////////////////////////////////////////////
  upper_right_splitter->SetMinimumPaneSize(100);
  upper_right_splitter->SplitVertically(upper_mid, right_top_right);
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
  algoSelectionList->AppendColumn("Method");
  algoSelectionList->AppendColumn("Description");

  algoSelectionList->SetColumnWidth(0, 80);
  algoSelectionList->SetColumnWidth(1, 150);

  algoSelectionList->Bind(wxEVT_LIST_COL_CLICK, [this](wxListEvent& evt) {
    this->sortByColumn(evt.GetColumn());
  });
  populateAlgoList();

  //////////////////////////////////////////////////////////////////////////////////////////////////////
  // Initializing Selected Algorithm Info list
  //////////////////////////////////////////////////////////////////////////////////////////////////////
  algoInfoList = new wxListView(bottom_right);
  algoInfoList->AppendColumn("Method");
  algoInfoList->AppendColumn("Mean");
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
  wxButton* unselectAll_2 = new wxButton(left, ID_UNSELECT_ALL_2, "Unselect All");
  wxButton* removeSelected = new wxButton(bottom_right, ID_REMOVE_SELECTED, "rm Selected");
  wxButton* clearList = new wxButton(bottom_right, ID_CLEAR, "Clear All");

  //////////////////////////////////////////////////////////////////////////////////////////////////////
  // Setting-up Choice Boxes
  //////////////////////////////////////////////////////////////////////////////////////////////////////
  dataType = new wxChoice(bottom_right, ID_DATA_CHOICE);
  dataType->Insert("Mean", 0);
  dataType->Insert("Nbr Alloc", 1);
  dataType->Insert("Byte/Alloc", 2);
  dataType->SetSelection(0);

  wxChoice* graphType = new wxChoice(bottom_right, ID_GRAPH_CHOICE);
  graphType->Insert("Barchart", 0);
  //graphType->Insert("Boxplot", 1);
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
  // setting-up system information textCtrl and determining nbr cpu cores (max threads)
  //////////////////////////////////////////////////////////////////////////////////////////////////////
  sysInfo = new wxTextCtrl( right_top_right, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize,
                            wxTE_READONLY | wxTE_MULTILINE);

  populateSystemInfo();
  go_options_win->inputThreads->SetRange(0, maxThreads);

  //////////////////////////////////////////////////////////////////////////////////////////////////////
  // Setting-up Chart
  //////////////////////////////////////////////////////////////////////////////////////////////////////
  chart_graph = new wxHorizontalBarChart(upper_mid);

  //chart_graph->SetXAxisLabel(wxT("Time (ms)"));
  chart_graph->SetYAxisLabel(wxT("Method Name"));
  //chart_graph->SetTitle(wxT("Time Mean"));

  //////////////////////////////////////////////////////////////////////////////////////////////////////
  // Setting-up  Sizers
  //////////////////////////////////////////////////////////////////////////////////////////////////////
  wxBoxSizer* infoSZ = new wxBoxSizer(wxVERTICAL);
  infoSZ->Add(sysInfo, 1, wxEXPAND | wxALL, 5);
  right_top_right->SetSizerAndFit(infoSZ);

  wxBoxSizer* choiceSZ = new wxBoxSizer(wxHORIZONTAL);
  choiceSZ->Add(dataType, 0, wxRIGHT, 5);
  choiceSZ->Add(graphType, 0);

  wxBoxSizer* selectionSZ = new wxBoxSizer(wxHORIZONTAL);
  selectionSZ->Add(removeSelected, 1, wxRIGHT, 5);
  selectionSZ->Add(clearList, 0);

  wxBoxSizer* bottom_left = new wxBoxSizer(wxHORIZONTAL);
  bottom_left->Add(unselectAll_2, 0, wxEXPAND | wxRIGHT, 5);
  bottom_left->Add(runBench, 1, wxEXPAND);


  wxBoxSizer* paneSz = new wxBoxSizer(wxVERTICAL);
  paneSz->Add(go_options_win, 1, wxEXPAND);
  options_pane_win->SetSizer(paneSz);
  paneSz->SetSizeHints(options_pane_win);


  wxBoxSizer* chartsz = new wxBoxSizer(wxVERTICAL);
  chartsz->Add(chart_graph, 1, wxEXPAND);
  upper_mid->SetSizerAndFit(chartsz);

  wxBoxSizer* main_sizer = new wxBoxSizer(wxVERTICAL);
  main_sizer->Add(searchBox, 0, wxEXPAND | wxTOP | wxLEFT | wxRIGHT, 5);
  main_sizer->Add(algoSelectionList, 1, wxEXPAND | wxALL, 5);
  main_sizer->Add(options_pane, 0, wxEXPAND | wxBOTTOM, 5);
  main_sizer->Add(bottom_left, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 5);
  left->SetSizerAndFit(main_sizer);

  wxBoxSizer* nd_bottom_right_sizer = new wxBoxSizer(wxHORIZONTAL);
  nd_bottom_right_sizer->Add(drawGraphs, 1, wxALIGN_BOTTOM | wxBOTTOM | wxRIGHT, 5);

  wxBoxSizer* nd_right_sizer = new wxBoxSizer(wxVERTICAL);
  nd_right_sizer->Add(choiceSZ, 0, wxEXPAND | wxRIGHT | wxTOP | wxBOTTOM, 5);
  nd_right_sizer->Add(unselectAll, 0, wxEXPAND | wxBOTTOM | wxRIGHT, 5);
  nd_right_sizer->Add(selectionSZ, 0, wxEXPAND | wxBOTTOM | wxRIGHT, 5);
  nd_right_sizer->Add(nd_bottom_right_sizer, 1, wxEXPAND);
  //nd_right_sizer->Add(drawGraphs, 1);

  wxBoxSizer* nd_sizer = new wxBoxSizer(wxHORIZONTAL);
  nd_sizer->Add(algoInfoList, 1, wxEXPAND | wxALL, 5);
  nd_sizer->Add(nd_right_sizer, 0, wxEXPAND);
  bottom_right->SetSizerAndFit(nd_sizer);

}

void MyFrame::addListItem(const std::string& method, const std::string& description) {
  int index = algoSelectionList->GetItemCount();

  algoSelectionList->InsertItem(index, method);
  algoSelectionList->SetItem(index, 1, description);

  ItemData data {method, description};
  auto dataPtr = std::make_unique<ItemData>(data);

  algoSelectionList->SetItemData(index, reinterpret_cast<wxIntPtr>(dataPtr.get()));

  itemDataSet.insert(std::move(dataPtr));
}

void MyFrame::sortByColumn(int index) {
  switch (index) {
    case 0: algoSelectionList->SortItems(&nameSortCallBack, sortDirection);
            break;
    case 1: algoSelectionList->SortItems(&descSortCallBack, sortDirection);
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

void MyFrame::addAlgorithmBenchResult(const wxString& name, const wxString& Mean,
                                      const wxString& nbrIter, const wxString& nbrAlloc,
                                      const wxString& bytePerAlloc) {
    long index = algoInfoList->GetItemCount();
    algoInfoList->InsertItem(index, name);
    algoInfoList->SetItem(index, 1, Mean);
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

    wxString itemName = algoSelectionList->GetItemText(index, 0);
    removeDuplicate(itemName);

    std::vector<wxString> results = getGOBenchResults(itemName);

    addAlgorithmBenchResult(itemName, results[2], results[3], results[0], results[1]);
    algoInfoList->Update();

    index = algoSelectionList->GetNextSelected(index);
    while (index != -1) {

      itemName = algoSelectionList->GetItemText(index, 0);
      removeDuplicate(itemName);

      results = getGOBenchResults(itemName);

      addAlgorithmBenchResult(itemName, results[2], results[3], results[0], results[1]);
      algoInfoList->Update();
      index = algoSelectionList->GetNextSelected(index);
    }
  }
}

void MyFrame::OnSearch(wxCommandEvent& evt) {
  long currentIndex = algoSelectionList->GetNextItem(-1);
  while (currentIndex != -1) {
    if (algoSelectionList->GetItemText(currentIndex, 0).IsSameAs(evt.GetString(), false)) {
      algoSelectionList->Select(currentIndex);
      break;
    }
    currentIndex = algoSelectionList->GetNextItem(currentIndex);
  }
}

void MyFrame::OnDraw(wxCommandEvent& evt) {

  // remove all previous data elements from chart
  chart_graph->ClearChart();

  switch (dataType->GetSelection()) {
    case 0 :
            {
              chart_graph->SetXAxisLabel(wxT("Time (ns)"));
              chart_graph->SetTitle(wxT("Time Mean"));
              long index = algoInfoList->GetFirstSelected();
              while (index != -1) {
                double Mean;
                algoInfoList->GetItemText(index, 1).ToDouble(&Mean);
                chart_graph->AddChart(algoInfoList->GetItemText(index, 0), Mean);
                index = algoInfoList->GetNextSelected(index);
              }
              break;
            }
    case 1 :
            {
              chart_graph->SetXAxisLabel(wxT("Nbr Alloc"));
              chart_graph->SetTitle(wxT("Number of Allocations"));
              long index = algoInfoList->GetFirstSelected();
              while (index != -1) {
                long nbr_alloc;
                algoInfoList->GetItemText(index, 3).ToLong(&nbr_alloc);
                chart_graph->AddChart(algoInfoList->GetItemText(index, 0), nbr_alloc);
                index = algoInfoList->GetNextSelected(index);
              }
              break;
            }
    case 2 :
            {
              chart_graph->SetXAxisLabel(wxT("B/Alloc"));
              chart_graph->SetTitle(wxT("Bytes per Allocation"));
              long index = algoInfoList->GetFirstSelected();
              while (index != -1) {
                long bytes_per_alloc;
                algoInfoList->GetItemText(index, 4).ToLong(&bytes_per_alloc);
                chart_graph->AddChart(algoInfoList->GetItemText(index, 0), bytes_per_alloc);
                index = algoInfoList->GetNextSelected(index);
              }
              break;
            }

    default : break;
  }

  chart_graph->UpdateMax();
  chart_graph->paintNow();
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
  switch(go_options_win->inputChoiceRadio->GetSelection()) {

    // iterations selected.
    case 1: // append cmd with: go_options_win->inputIterChoice->GetStringSelection();
            {
            cmd << "-iterations=" << go_options_win->inputIterChoice->GetStringSelection() << " ";
            break;
            }

    // time selected
    case 2: {
            if (!go_options_win->inputTime->IsEmpty())
              // append cmd with: go_options_win->inputTime->GetValue();
              cmd << "-time=" << go_options_win->inputTime->GetValue() << " ";
            break;
            }

    // else use default options
    default:
            {
              cmd << "-iterations=1000" << " ";
              break;
            }

  }

  // if user selected something use the selected option, else use default option
  if( go_options_win->inputSize->GetSelection() != 0) {
    cmd << "-size=" << go_options_win->inputSize->GetStringSelection() << " ";
  }

  // if user provided the number of threads use the provided number, else use default option
  if (go_options_win->inputThreads->GetValue() != 0) {
    // append cmd with: go_options_win->inputThreads->GetValue();
    cmd << "-threads=" << go_options_win->inputThreads->GetValue();
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

void MyFrame::populateAlgoList() {
  std::ifstream filein("algorithms.txt");
  for (std::string line; std::getline(filein, line);) {
    size_t pos = line.find(":");
    addListItem(line.substr(0, pos), line.substr(pos + 1));
  }
}

void MyFrame::OnUnselect(wxCommandEvent& e) {
  long index = algoInfoList->GetFirstSelected();
  while (index != -1) {
    if (algoInfoList->IsSelected(index)) {
      algoInfoList->Select(index, false);
    }
    index = algoInfoList->GetNextSelected(index);
  }
}

void MyFrame::OnUnselect2(wxCommandEvent& e) {
  long index = algoSelectionList->GetFirstSelected();
  while (index != -1) {
    if (algoSelectionList->IsSelected(index)) {
      algoSelectionList->Select(index, false);
    }
    index = algoSelectionList->GetNextSelected(index);
  }
}

void MyFrame::OnRemoveSelected(wxCommandEvent& e) {
  long index = algoInfoList->GetFirstSelected();
  while(index != -1) {
    algoInfoList->DeleteItem(index);
    index = algoInfoList->GetNextSelected(index);
  }
}

void MyFrame::OnClear(wxCommandEvent& e) {
  algoInfoList->DeleteAllItems();
}

void MyFrame::populateSystemInfo() {
  #if defined(_WIN32)
  #elif defined(__linux__)

    // parsing operating system information
    std::string line_string;
    int line = 1;
    for(std::ifstream os_info("/etc/os-release"); std::getline(os_info, line_string); ++line) {

      // pretty name
      if (line == 5) {
        sysInfo->AppendText(wxString("Operating System : Linux"));
        sysInfo->AppendText(wxT("\n"));
        sysInfo->AppendText(wxString("Distribution : "));
        sysInfo->AppendText(wxString(line_string.substr(line_string.find("\"")) ) );
        sysInfo->AppendText(wxT("\n\n"));
      } else if (line > 5) {
        break;
      }
    }

    // parsing necessary cpu information
    line = 1;
    for (std::ifstream cpu_info("/proc/cpuinfo"); std::getline(cpu_info, line_string); ++line) {

      // model name
      if (line == 5) {
        sysInfo->AppendText(wxString(line_string));
        sysInfo->AppendText(wxT("\n"));

      // cache size
      } else if (line == 9) {
        sysInfo->AppendText(wxString(line_string));
        sysInfo->AppendText(wxT("\n"));
      // CPU cores
      } else if (line == 13) {
        sysInfo->AppendText(wxString(line_string));
        sysInfo->AppendText(wxT("\n\n"));

        line_string.erase(std::remove_if(line_string.begin(), line_string.end(), ::isspace), line_string.end());
        maxThreads = std::stoul(line_string.substr(line_string.find(":") + 1));
      }
    }

    // parsing necessary memory information
    std::ifstream mem_info("/proc/meminfo");
    std::getline(mem_info, line_string);             // total memory
    sysInfo->AppendText(wxString(line_string));
    sysInfo->AppendText(wxT("\n"));

  #elif defined(__APPLE__) && defined(__MACH__)
  #endif
}
