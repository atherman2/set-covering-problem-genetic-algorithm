EXE = build/set-cover
EXE_TEST = build/testing
DS = source/data_structure.cpp
RG = source/random_generator.cpp
POP = source/population.cpp

all: $(EXE)

$(EXE): main.cpp $(DS) $(RG) $(POP)
	g++ main.cpp $(DS) $(RG) $(POP) -Iheaders -O2 -o $(EXE)

run: $(EXE)
	$(EXE) $(file)

ifeq ($(NO_LS),1)
TEST_FLAGS = -DNO_LOCAL_SEARCH
else
TEST_FLAGS =
endif

.PHONY: test
test:
	g++ testing.cpp $(DS) $(RG) $(POP) -Iheaders -O2 $(TEST_FLAGS) -o $(EXE_TEST)
	$(EXE_TEST)

ifeq ($(NO_LS),1)
CONV_FLAGS = -DNO_LOCAL_SEARCH
else
CONV_FLAGS =
endif

.PHONY: convergence
convergence:
	g++ convergence.cpp $(DS) $(RG) $(POP) -Iheaders -O2 $(CONV_FLAGS) -o build/convergence
	build/convergence
