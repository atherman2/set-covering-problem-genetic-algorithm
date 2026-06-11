EXE = build/set-cover
DS = source/data_structure.cpp

all: $(EXE)

$(EXE): main.cpp $(DS)
	g++ main.cpp $(DS) -Iheaders -o $(EXE)

run: $(EXE)
	$(EXE)
