"""
Run "make convergence" and "make convergence NO_LS=1" first,
then run "python plot_convergence.py" to generate one plot per instance.
"""

import pandas as pd
import matplotlib.pyplot as plt
import os
import glob

FOLDER = os.path.join("results", "convergence")
OUTPUT_FOLDER = os.path.join("results", "convergence_plots")


def main():
    if not os.path.isdir(FOLDER):
        print(f"ERROR: '{FOLDER}' not found. Run 'make convergence' first.")
        return

    os.makedirs(OUTPUT_FOLDER, exist_ok=True)

    with_ls_files = glob.glob(os.path.join(FOLDER, "*_with_ls.csv"))
    instance_names = sorted(
        os.path.basename(f).replace("_with_ls.csv", "") for f in with_ls_files
    )

    if not instance_names:
        print(f"ERROR: no '*_with_ls.csv' files found in '{FOLDER}'.")
        return

    for name in instance_names:
        with_ls_path = os.path.join(FOLDER, f"{name}_with_ls.csv")
        without_ls_path = os.path.join(FOLDER, f"{name}_without_ls.csv")

        plt.figure(figsize=(8, 5))

        if os.path.exists(with_ls_path):
            df = pd.read_csv(with_ls_path)
            plt.plot(df["generation"], df["best_cost"], label="With local search")
        else:
            print(f"WARNING: '{with_ls_path}' not found.")

        if os.path.exists(without_ls_path):
            df = pd.read_csv(without_ls_path)
            plt.plot(df["generation"], df["best_cost"], label="Without local search")
        else:
            print(f"WARNING: '{without_ls_path}' not found.")

        plt.xlabel("Generation")
        plt.ylabel("Best cost found")
        plt.title(f"Convergence - {name}")
        plt.legend()
        plt.tight_layout()

        output_path = os.path.join(OUTPUT_FOLDER, f"convergence_{name}.png")
        plt.savefig(output_path, dpi=120)
        plt.close()
        print(f"Saved '{output_path}'")


if __name__ == "__main__":
    main()