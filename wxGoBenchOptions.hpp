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
  wxRadioBox* GetInputChoiceRadio();
  wxChoice* GetInputSize();
  wxChoice* GetInputIter();
  wxTextCtrl* GetInputTime();
  wxTextCtrl* GetInputThreads();
  wxTextCtrl* GetInputBench();

  wxGoBenchOptions(wxWindow* parent);

  wxRadioBox* inputChoiceRadio;

  wxChoice* inputIterChoice;
  wxChoice* inputSize;
  wxTextCtrl* inputTime;
  wxTextCtrl* inputThreads;
  wxTextCtrl* inputBench;

  DECLARE_EVENT_TABLE()
private:
  wxCheckBox* memProfile;
  wxCheckBox* cpuProfile;

  void OnRadio(wxCommandEvent& event);
};

#endif
