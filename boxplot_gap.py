"""
- Run "make test"        -> generates "results/results_with_ls.csv"
- Run "make test NO_LS=1" -> generates "results/results_without_ls.csv"
- Run "python boxplot_gap.py" -> generates "instance_summary.csv" and comparison boxplots.
"""

import pandas as pd
import matplotlib.pyplot as plt
import os

FOLDER = "results"
WITH_LS_FILE = os.path.join(FOLDER, "results_with_ls.csv")
WITHOUT_LS_FILE = os.path.join(FOLDER, "results_without_ls.csv")
BEST_KNOWN_FILE = os.path.join(FOLDER, "best_known.csv")
SUMMARY_OUTPUT_FILE = os.path.join(FOLDER, "instance_summary.csv")


def load_variant(path, label):
    if not os.path.exists(path):
        print(f"WARNING: could not find '{path}'. Skipping '{label}'.")
        return None
    df = pd.read_csv(path)
    df["variant"] = label
    return df


def main():
    with_ls = load_variant(WITH_LS_FILE, "with_ls")
    without_ls = load_variant(WITHOUT_LS_FILE, "without_ls")

    frames = [df for df in (with_ls, without_ls) if df is not None]
    if not frames:
        print("ERROR: no results files found. Run 'make test' and/or "
              "'make test NO_LS=1' first.")
        return

    results = pd.concat(frames, ignore_index=True)

    if not os.path.exists(BEST_KNOWN_FILE):
        print(f"ERROR: could not find '{BEST_KNOWN_FILE}'.")
        return

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

    # Calculate summary metrics per instance AND per variant
    summary = data.groupby(["instance", "variant"]).agg(
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

    variants_present = sorted(data["variant"].unique())

    # Generate a comparison plot per instance: with_ls vs without_ls side by side
    for inst in sorted(data["instance"].unique()):
        inst_data = data[data["instance"] == inst]

        box_data = []
        labels = []
        for variant in variants_present:
            values = inst_data[inst_data["variant"] == variant]["gap"].values
            if len(values) > 0:
                box_data.append(values)
                labels.append(variant)

        if not box_data:
            continue

        clean_name = inst.replace("/", "_").replace(".dat", "")
        output_path = os.path.join(boxplot_folder, f"boxplot_{clean_name}.png")

        plt.figure(figsize=(5, 5))
        plt.boxplot(box_data, tick_labels=labels, showmeans=True)
        plt.ylabel("GAP (%)")
        plt.title(f"GAP Distribution - {clean_name}")
        plt.tight_layout()

        plt.savefig(output_path, dpi=120)
        plt.close()

    print(f"Summary saved to '{SUMMARY_OUTPUT_FILE}'")
    print(f"Boxplots saved to '{boxplot_folder}/'")


if __name__ == "__main__":
    main()