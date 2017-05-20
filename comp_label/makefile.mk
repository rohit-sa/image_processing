objects = comp_label.o feature.o preprocessing.o
LIBS = `pkg-config --libs opencv`
CFLAGS = -std=c++11 `pkg-config --cflags opencv`

comp_label : $(objects)
	g++ $(objects) -o comp_label $(LIBS) $(CFLAGS)

comp_label.o : comp_label.cpp feature.h preprocessing.h
	g++ $(CFLAGS) $(LIBS) -c comp_label.cpp

feature.o : feature.cpp feature.h
	g++ $(CFLAGS) $(LIBS) -c feature.cpp
	
preprocessing.o : preprocessing.cpp preprocessing.h
	g++ $(CFLAGS) $(LIBS) -c preprocessing.cpp

.PHONY : clean
clean : 
	-rm comp_label $(objects)
