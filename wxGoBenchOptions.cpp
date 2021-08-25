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

    inputIterChoice = new wxChoice(this, wxID_ANY);
    inputIterChoice->SetLabel("Nbr Iterations");
    inputIterChoice->Append(wxString("10"));
    inputIterChoice->Append(wxString("100"));
    inputIterChoice->Append(wxString("1,000"));
    inputIterChoice->Append(wxString("10,000"));
    inputIterChoice->Append(wxString("100,000"));
    inputIterChoice->Append(wxString("1,000,000"));
    inputIterChoice->Disable();

    inputTime = new wxTextCtrl(this, ID_INPUT_TIME, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
    inputTime->SetLabel("Time (s)");
    inputTime->Disable();

    main_sizer = new wxBoxSizer(wxVERTICAL);
    main_sizer->Add(inputChoiceRadio, 0, wxEXPAND);
    main_sizer->Add(inputIterChoice, 1, wxEXPAND);
    main_sizer->Add(inputTime, 1, wxEXPAND);
    this->SetSizerAndFit(main_sizer);
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
  }
}
