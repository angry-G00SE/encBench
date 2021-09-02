# Compiler gcc/g++
CXX = g++

# Compiler Flags
CXXFLAGS = -c

# folder containing GUI files
GUIDIR = GUI

benchmark : $(GUIDIR)/benchmarkGUI.o $(GUIDIR)/wxHorizontalBarChart.o $(GUIDIR)/wxGoBenchOptions.o
	$(CXX) -o $@ $^ `wx-config --libs`

$(GUIDIR)/wxGoBenchOptions.o : $(GUIDIR)/wxGoBenchOptions.cpp $(GUIDIR)/wxGoBenchOptions.hpp
	$(CXX) -o $@ $(CXXFLAGS) `wx-config --cxxflags` $<

$(GUIDIR)/wxHorizontalBarChart.o : $(GUIDIR)/wxHorizontalBarChart.cpp $(GUIDIR)/wxHorizontalBarChart.hpp
	$(CXX) -o $@ $(CXXFLAGS) `wx-config --cxxflags` $<

$(GUIDIR)/benchmarkGUI.o : $(GUIDIR)/benchmarkGUI.cpp $(GUIDIR)/benchmarkGUI.hpp $(GUIDIR)/wxHorizontalBarChart.hpp $(GUIDIR)/wxGoBenchOptions.hpp
	$(CXX) -o $@ $(CXXFLAGS) `wx-config --cxxflags` $<

.PHONY = clean

clean :
	rm -f benchmark $(GUIDIR)/*.o
