# Compiler gcc/g++
CXX = g++

# Compiler Flags
CXXFLAGS = -g -c

benchmark : benchmarkGUI.o wxHorizontalBarChart.o
	$(CXX) -g -o $@ $^ `wx-config --cxxflags --libs`

wxHorizontalBarChart.o : wxHorizontalBarChart.cpp wxHorizontalBarChart.hpp
	$(CXX) $(CXXFLAGS) `wx-config --cxxflags --libs` $<

benchmarkGUI.o : benchmarkGUI.cpp benchmarkGUI.hpp wxHorizontalBarChart.hpp
	$(CXX) $(CXXFLAGS) `wx-config --cxxflags --libs` $<

.PHONY = clean

clean :
	rm -f benchmark *.o
