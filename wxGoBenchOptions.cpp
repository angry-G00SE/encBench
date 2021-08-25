#include <wx/wxprec.h>

// if not possible then:
#ifndef WX_PRECOMP
  #include <wx/wx.h>
#endif

#include "wxGoBenchOptions.hpp"

wxBEGIN_EVENT_TABLE(wxGoBenchOptions, wxWindow)
  EVT_RADIOBOX(ID_RADIO_BOX, wxGoBenchOptions::OnRadio)
wxEND_EVENT_TABLE()

wxGoBenchOptions::wxGoBenchOptions(wxWindow* parent) :
  wxWindow(parent, ID_GO_WINDOW) {
    wxString choices[] = {wxString("Default"), wxString("Iterations"), wxString("Time")};
    inputChoiceRadio = new wxRadioBox ( this, ID_RADIO_BOX, wxString("Input Options"), wxDefaultPosition, wxDefaultSize, 3, choices,
                                        0, wxRA_SPECIFY_COLS );
    wxStaticText* iterText = new wxStaticText(this, wxID_ANY, wxString("Nbr Iterations"));
    inputIterChoice = new wxChoice(this, wxID_ANY);
    inputIterChoice->Append(wxString("10"));
    inputIterChoice->Append(wxString("100"));
    inputIterChoice->Append(wxString("1,000"));
    inputIterChoice->Append(wxString("10,000"));
    inputIterChoice->Append(wxString("100,000"));
    inputIterChoice->Append(wxString("1,000,000"));
    inputIterChoice->Disable();


    wxStaticText* timeText = new wxStaticText(this, wxID_ANY, wxString("Time (s)"));
    inputTime = new wxTextCtrl(this, ID_INPUT_TIME, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
    inputTime->Disable();

    wxStaticText* sizeText = new wxStaticText(this, wxID_ANY, wxString("Size"));
    inputSize = new wxChoice(this, wxID_ANY);
    inputSize->Append(wxString("128 bytes"));
    inputSize->Append(wxString("1 Mb"));
    inputSize->Append(wxString("5 Mb"));
    inputSize->Append(wxString("50 Mb"));
    inputSize->Append(wxString("100 Mb"));
    inputSize->Append(wxString("200 Mb"));

    wxStaticText* threadText = new wxStaticText(this, wxID_ANY, wxString("Nbr Threads"));
    inputThreads = new wxTextCtrl(this, ID_INPUT_THREADS);

    wxStaticText* benchText = new wxStaticText(this, wxID_ANY, wxString("Nbr Benchmarks"));
    inputBench = new wxTextCtrl(this, ID_INPUT_THREADS);

    //////////////////////////////////////////////////////////////////////////////////////////////////////
    // Setting-up  Sizers
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    wxBoxSizer* go_size_sizer = new wxBoxSizer(wxHORIZONTAL);
    go_size_sizer->Add(sizeText, 0, wxLEFT | wxRIGHT | wxBOTTOM | wxALIGN_CENTER_VERTICAL, 5);
    go_size_sizer->Add(inputSize, 1, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 5);

    wxBoxSizer* go_thread_sizer = new wxBoxSizer(wxHORIZONTAL);
    go_thread_sizer->Add(threadText, 0, wxLEFT | wxRIGHT | wxBOTTOM | wxALIGN_CENTER_VERTICAL, 5);
    go_thread_sizer->Add(inputThreads, 1, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 5);

    wxBoxSizer* go_bench_sizer = new wxBoxSizer(wxHORIZONTAL);
    go_bench_sizer->Add(benchText, 0, wxLEFT | wxRIGHT | wxBOTTOM | wxALIGN_CENTER_VERTICAL, 5);
    go_bench_sizer->Add(inputBench, 1,  wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 5);

    wxBoxSizer* go_iter_sizer = new wxBoxSizer(wxHORIZONTAL);
    go_iter_sizer->Add(iterText, 0, wxLEFT | wxRIGHT | wxBOTTOM | wxALIGN_CENTER_VERTICAL, 5);
    go_iter_sizer->Add(inputIterChoice, 1, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 5);

    wxBoxSizer* go_time_sizer = new wxBoxSizer(wxHORIZONTAL);
    go_time_sizer->Add(timeText, 0, wxLEFT | wxRIGHT | wxBOTTOM | wxALIGN_CENTER_VERTICAL, 5);
    go_time_sizer->Add(inputTime, 1, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 5);

    wxBoxSizer* go_main_sizer = new wxBoxSizer(wxVERTICAL);
    go_main_sizer->Add(inputChoiceRadio, 0, wxEXPAND | wxALL, 5);
    go_main_sizer->Add(go_iter_sizer, 1, wxEXPAND);
    go_main_sizer->Add(go_time_sizer, 1, wxEXPAND);
    go_main_sizer->Add(go_size_sizer, 1, wxEXPAND);
    go_main_sizer->Add(go_thread_sizer, 1, wxEXPAND);
    go_main_sizer->Add(go_bench_sizer, 1, wxEXPAND);

    this->SetSizerAndFit(go_main_sizer);
}

void wxGoBenchOptions::OnRadio(wxCommandEvent& event) {
  switch(event.GetInt()) {
    case 0: if (inputIterChoice->IsEnabled())
              inputIterChoice->Disable();
            if (inputTime->IsEnabled())
              inputTime->Disable();
            break;

    case 1: if (inputTime->IsEnabled())
              inputTime->Disable();
            inputIterChoice->Enable();
            break;

    case 2: if (inputIterChoice->IsEnabled())
              inputIterChoice->Disable();
            inputTime->Enable();
            break;

    default: break;
  }
}
