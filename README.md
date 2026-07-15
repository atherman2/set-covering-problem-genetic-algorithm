# Set Covering Problem (SCP) with Genetic Algorithm
Implementation of a Genetic Algorithm (GA) for the Set Covering Problem (SCP), with optional swap-based local search, automated testing across multiple instances, and GAP chart generation.


## Requirements
- `g++` with C++17 support
- Python 3 with `pandas` and `matplotlib`

```bash
pip install pandas matplotlib
```


## Build and Run

### Simple run
Runs a single instance (quick test, no files generated):

```bash
make run
make run file=Testing/Teste_01.dat
```

---

### Full tests
Runs the algorithm 10 times for each instance, measuring solution cost and execution time. Use the `NO_LS=1` flag to disable local search (pure GA):

```bash
make test           # with local search
make test NO_LS=1   # without local search
```

CSV format: `instance,run,cost,time_seconds`.

---

### Convergence
Records the best cost per generation (1 run per instance):

```bash
make convergence
make convergence NO_LS=1
```
CSV format:
- generation,best_cost

---

### Analysis and Charts
Python scripts for analyzing results:

```bash
python boxplot_gap.py
python plot_convergence.py
```

Output:
- `results/instance_summary.csv` (per-instance statistics)
- `results/boxplot/boxplot_<Instance>.png` (GAP)
- `results/convergence_plots/convergence_<Instance>.png` (curves)

Requirements:
- `boxplot_gap.py` -> run `make test` first
- `plot_convergence.py` -> run `make convergence` first


## Recommended Workflow

1. Cost/time tests (10 runs per instance)

```bash
make test
make test NO_LS=1
```

2. Convergence curves (1 run per instance)

```bash
make convergence
make convergence NO_LS=1
```

3. Analysis and charts

```bash
python boxplot_gap.py
python plot_convergence.py
```

## Project Structure

```text
├── headers/              # .hpp files (declarations)
├── source/               # .cpp files (implementations)
├── Testing/              # SCP instances (.dat)
├── results/              # Output (CSVs and charts)
├── main.cpp              # Runs a single instance
├── testing.cpp           # Runs 10 rounds per instance
├── convergence.cpp       # Saves evolution per generation
├── boxplot_gap.py        # Generates boxplots and GAP statistics
├── plot_convergence.py   # Generates convergence charts
└── Makefile
```
