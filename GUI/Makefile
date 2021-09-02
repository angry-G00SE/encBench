# Compiler gcc/g++
CXX = g++

# Compiler Flags
CXXFLAGS = -c

benchmark : benchmarkGUI.o wxHorizontalBarChart.o wxGoBenchOptions.o
	$(CXX) -o $@ $^ `wx-config --libs`

wxGoBenchOptions.o : wxGoBenchOptions.cpp wxGoBenchOptions.hpp
	$(CXX) $(CXXFLAGS) `wx-config --cxxflags` $<

wxHorizontalBarChart.o : wxHorizontalBarChart.cpp wxHorizontalBarChart.hpp
	$(CXX) $(CXXFLAGS) `wx-config --cxxflags` $<

benchmarkGUI.o : benchmarkGUI.cpp benchmarkGUI.hpp wxHorizontalBarChart.hpp wxGoBenchOptions.hpp
	$(CXX) $(CXXFLAGS) `wx-config --cxxflags` $<

.PHONY = clean

clean :
	rm -f benchmark *.o
