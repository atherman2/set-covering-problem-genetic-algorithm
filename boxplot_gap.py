"""
- Run "make test" first -> this generates "results/results.csv"
- Run "python boxplot_gap.py" -> this generates "instance_summary.csv" and "boxplot_gap.png".
"""

import pandas as pd
import matplotlib.pyplot as plt
import os

FOLDER = "results"
RESULTS_FILE = os.path.join(FOLDER, "results.csv")
BEST_KNOWN_FILE = os.path.join(FOLDER, "best_known.csv")
SUMMARY_OUTPUT_FILE = os.path.join(FOLDER, "instance_summary.csv")


def main():
    if not os.path.exists(RESULTS_FILE):
        print(f"ERROR: could not find '{RESULTS_FILE}'. "
              f"Run 'make test' first to generate it.")
        return

    if not os.path.exists(BEST_KNOWN_FILE):
        print(f"ERROR: could not find '{BEST_KNOWN_FILE}'.")
        return

    results = pd.read_csv(RESULTS_FILE)
    best_known = pd.read_csv(BEST_KNOWN_FILE)

    data = results.merge(best_known, on="instance", how="left")

    missing = data[data["best_known"].isna()]["instance"].unique()
    if len(missing) > 0:
        print("WARNING: instances without a defined best_known (ignored in GAP):")
        for f in missing:
            print(f"  - {f}")
        data = data.dropna(subset=["best_known"])

    data["gap"] = (
        (data["cost"] - data["best_known"]) / data["best_known"]
    ) * 100

    # Calculate summary metrics per instance
    summary = data.groupby("instance").agg(
        best_cost=("cost", "min"),
        avg_cost=("cost", "mean"),
        worst_cost=("cost", "max"),
        best_gap=("gap", "min"),
        avg_gap=("gap", "mean"),
        worst_gap=("gap", "max"),
        avg_time_s=("time_seconds", "mean"),
        runs=("cost", "count"),
    ).reset_index()
    summary = summary.round(2)
    summary.to_csv(SUMMARY_OUTPUT_FILE, index=False)

    boxplot_folder = os.path.join(FOLDER, "boxplot")
    os.makedirs(boxplot_folder, exist_ok=True)

    # Generate a separate plot for each instance
    for inst in sorted(data["instance"].unique()):
        inst_data = data[data["instance"] == inst]["gap"].values
        
        clean_name = inst.replace("/", "_").replace(".dat", "")
        output_path = os.path.join(boxplot_folder, f"boxplot_{clean_name}.png")
        
        plt.figure(figsize=(5, 5))
        plt.boxplot(inst_data, label=[inst], showmeans=True)
        plt.ylabel("GAP (%)")
        plt.title(f"GAP Distribution - {clean_name}")
        plt.tight_layout()
        
        plt.savefig(output_path, dpi=120)
        plt.close()

if __name__ == "__main__":
    main()