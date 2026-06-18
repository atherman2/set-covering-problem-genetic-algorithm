EXE = build/set-cover
DS = source/data_structure.cpp
RG = source/random_generator.cpp
POP = source/population.cpp

all: $(EXE)

$(EXE): main.cpp $(DS) $(RG) $(POP)
	g++ main.cpp $(DS) $(RG) $(POP) -Iheaders -o $(EXE)

run: $(EXE)
	$(EXE)
