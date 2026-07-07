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
BOXPLOT_FILE = os.path.join(FOLDER, "boxplot_gap.png")


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
    print(f"Instance summary saved to '{SUMMARY_OUTPUT_FILE}'")
    print("\n--- Summary ---")
    for _, row in summary.iterrows():
        for col in summary.columns:
            print(f"{col}: {row[col]}")
        print()

    sorted_instances = sorted(data["instance"].unique())
    boxplot_data = [
        data[data["instance"] == inst]["gap"].values
        for inst in sorted_instances
    ]

    plt.figure(figsize=(10, 6))
    plt.boxplot(boxplot_data, label=sorted_instances, showmeans=True)
    plt.ylabel("GAP (%)")
    plt.xlabel("Instance")
    plt.title("GAP distribution per instance (10 runs)")
    plt.xticks(rotation=45, ha="right")
    plt.tight_layout()
    plt.savefig(BOXPLOT_FILE, dpi=150)
    print(f"\nBoxplot saved to '{BOXPLOT_FILE}'")

if __name__ == "__main__":
    main()