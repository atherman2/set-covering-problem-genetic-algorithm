EXE = build/set-cover
EXE_TEST = build/testing
DS = source/data_structure.cpp
RG = source/random_generator.cpp
POP = source/population.cpp

all: $(EXE)

$(EXE): main.cpp $(DS) $(RG) $(POP)
	g++ main.cpp $(DS) $(RG) $(POP) -Iheaders -o $(EXE)

run: $(EXE)
	$(EXE) $(FILE)

$(EXE_TEST): testing.cpp $(DS) $(RG) $(POP)
	g++ testing.cpp $(DS) $(RG) $(POP) -Iheaders -O2 -o $(EXE_TEST)

test: $(EXE_TEST)
	$(EXE_TEST)