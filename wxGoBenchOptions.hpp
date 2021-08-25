#ifndef WX_GO_BENCH_OPTIONS
#define WX_GO_BENCH_OPTIONS

#define ID_GO_WINDOW 10000

enum {
  ID_RADIO_BOX = 0,
  ID_INPUT_TIME = 1
};

class wxGoBenchOptions : public wxWindow {
public:
  wxGoBenchOptions(wxWindow* parent);

  DECLARE_EVENT_TABLE()

private:
  wxRadioBox* inputChoiceRadio;

  wxChoice* inputIterChoice;
  wxTextCtrl* inputTime;

  wxChoice* inputSizeChoice;
  wxTextCtrl* inputThreads;
  wxTextCtrl* inputBench;

  wxCheckBox* memProfile;
  wxCheckBox* cpuProfile;

  void OnRadio(wxCommandEvent& event);
};

#endif
