#ifndef WX_GO_BENCH_OPTIONS
#define WX_GO_BENCH_OPTIONS

#define ID_GO_WINDOW 10000

enum {
  ID_RADIO_BOX = 100,
  ID_INPUT_TIME = 101,
  ID_INPUT_THREADS = 102,
  ID_INPUT_BENCH = 103
};

class wxGoBenchOptions : public wxWindow {
public:
  // these are declared here because these will be called outside wxGoBenchOptions's constructor
  wxChoice* inputIterChoice;
  wxChoice* inputSize;
  wxTextCtrl* inputTime;
  wxTextCtrl* inputThreads;
  wxTextCtrl* inputBench;
  wxRadioBox* inputChoiceRadio;

  // constructor
  wxGoBenchOptions(wxWindow* parent);

private:
  // these are declared here because these will be called outside wxGoBenchOptions's constructor
  wxCheckBox* memProfile;
  wxCheckBox* cpuProfile;

  // events
  void OnRadio(wxCommandEvent& event);


  wxDECLARE_EVENT_TABLE();
};

#endif
