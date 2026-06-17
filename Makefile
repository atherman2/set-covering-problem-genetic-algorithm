EXE = build/set-cover
DS = source/data_structure.cpp
RG = source/random_generator.cpp

all: $(EXE)

$(EXE): main.cpp $(DS) $(RG)
	g++ main.cpp $(DS) $(RG) -Iheaders -o $(EXE)

run: $(EXE)
	$(EXE)
