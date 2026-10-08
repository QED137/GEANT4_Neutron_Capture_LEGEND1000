#!/usr/bin/env python3
"""Plot concentration, energy, capture-time, and capture-radius scan results."""
import csv
import re
import sys
from pathlib import Path

import matplotlib.pyplot as plt


CAPTURE = re.compile(
    r"Neutron captures\s*:\s*H = (\d+)\s+Gd = (\d+)\s+other = (\d+)"
)
BIN = re.compile(r"capture_(time|radius)_bin\s+([0-9.eE+-]+)\s+([0-9.eE+-]+)\s+(\d+)")


def read_log(path):
    text = path.read_text(encoding="utf-8")
    match = CAPTURE.search(text)
    if not match:
        raise ValueError(f"capture summary missing from {path}")
    h, gd, other = map(int, match.groups())
    bins = {"time": {}, "radius": {}}
    for kind, low, high, count in BIN.findall(text):
        bins[kind][(float(low), float(high))] = int(count)
    total = h + gd + other
    return {"h": h, "gd": gd, "other": other, "share": gd / total if total else 0, "bins": bins}


def main():
    if len(sys.argv) != 2:
        raise SystemExit("usage: plot_scans.py scan_results")
    root = Path(sys.argv[1])
    cases = list(csv.DictReader((root / "cases.csv").open(encoding="utf-8")))
    data = {row["log"]: read_log(root / row["log"]) for row in cases}

    concentration = [row for row in cases if row["scan"] == "concentration"]
    fig, ax = plt.subplots(figsize=(7, 4.5))
    ax.plot([float(row["value"]) * 100 for row in concentration],
            [data[row["log"]]["share"] * 100 for row in concentration], "o-")
    ax.set_xlabel("Gd concentration by mass (%)")
    ax.set_ylabel("Gd share of captures (%)")
    ax.set_title("Gd capture share versus concentration")
    ax.grid(True, alpha=0.25)
    fig.tight_layout()
    fig.savefig(root / "gd_share_vs_concentration.png", dpi=150)
    plt.close(fig)

    energy = [row for row in cases if row["scan"] == "energy"]
    labels = []
    water = []
    gd = []
    for row in energy:
        if row["gd_fraction"] == "0.0":
            labels.append(row["energy"])
            water.append(data[row["log"]]["share"] * 100)
        else:
            gd.append(data[row["log"]]["share"] * 100)
    fig, ax = plt.subplots(figsize=(8, 4.5))
    x = range(len(labels))
    ax.plot(x, water, "o-", label="pure water")
    ax.plot(x, gd, "o-", label="0.1% Gd water")
    ax.set_xticks(list(x), labels, rotation=30, ha="right")
    ax.set_xlabel("Initial neutron energy")
    ax.set_ylabel("Gd share of captures (%)")
    ax.set_title("Gd capture share versus neutron energy")
    ax.grid(True, alpha=0.25)
    ax.legend()
    fig.tight_layout()
    fig.savefig(root / "gd_share_vs_energy.png", dpi=150)
    plt.close(fig)

    base = next(row for row in energy if row["energy"] == "2.5 MeV" and row["gd_fraction"] == "0.0")
    gd_base = next(row for row in energy if row["energy"] == "2.5 MeV" and row["gd_fraction"] == "0.001")
    fig, axes = plt.subplots(2, 2, figsize=(11, 7), sharey="row")

    def plot_histogram(axis, kind, xlabel, x_scale=1.0):
        for row, label in ((base, "pure water"), (gd_base, "0.1% Gd water")):
            bins = sorted(data[row["log"]]["bins"][kind].items())
            if not bins:
                raise ValueError(f"{kind} histogram missing from {row['log']}")
            edges = [b[0][0] for b in bins] + [bins[-1][0][1]]
            axis.step([edge * x_scale for edge in edges],
                      [b[1] for b in bins] + [bins[-1][1]], where="post", label=label)
        axis.set_xlabel(xlabel)
        axis.set_ylabel("Capture count")
        axis.grid(True, alpha=0.25)
        axis.legend()

    # The simulation records time in microseconds. Show milliseconds on the
    # plot and provide an early-time zoom covering the first 250 microseconds.
    plot_histogram(axes[0, 0], "time", "Capture time (ms)", x_scale=1e-3)
    axes[0, 0].set_title("Full capture-time distribution")
    plot_histogram(axes[0, 1], "time", "Capture time (ms)", x_scale=1e-3)
    axes[0, 1].set_xlim(0, 0.25)
    axes[0, 1].set_title("Early-time zoom: 0–0.25 ms (0–250 μs)")
    plot_histogram(axes[1, 0], "radius", "Capture radius (cm)")
    axes[1, 0].set_title("Capture-radius distribution")
    axes[1, 1].axis("off")
    fig.suptitle("Capture time and position at 2.5 MeV")
    fig.tight_layout()
    fig.savefig(root / "capture_time_position_comparison.png", dpi=150)
    print(f"saved plots in {root}")


if __name__ == "__main__":
    main()
