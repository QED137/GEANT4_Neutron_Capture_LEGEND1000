#!/usr/bin/env python3
"""Plot concentration, energy, geometry, and detector-response scan results."""
import csv
import re
import sys
from pathlib import Path

import matplotlib.pyplot as plt


CAPTURE = re.compile(
    r"Neutron captures\s*:\s*H = (\d+)\s+Gd = (\d+)\s+other = (\d+)"
)
BIN = re.compile(r"capture_(time|radius)_bin\s+([0-9.eE+-]+)\s+([0-9.eE+-]+)\s+(\d+)")
ESCAPE = re.compile(r"Escaped neutrons = (\d+)")
EVENTS = re.compile(r"The run is (\d+)")
ISOTOPE = re.compile(r"capture_isotope\s+(\S+)\s+(\d+)")
LENGTH_STATS = re.compile(r"capture_length_stats\s+(\S+)\s+(\d+)\s+([0-9.eE+-]+)\s+([0-9.eE+-]+)")
THRESHOLD = re.compile(r"edep_threshold ([0-9]+) MeV (\d+)")
CONTAINMENT_BIN = re.compile(
    r"containment_bin\s+([0-9.eE+-]+)\s+([0-9.eE+-]+)\s+(\d+)"
)
CONTAINMENT_SUMMARY = re.compile(
    r"Capture energy containment summary: events = (\d+) mean_fraction = "
    r"([0-9.eE+-]+) rms = ([0-9.eE+-]+)"
)


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
    escaped = ESCAPE.search(text)
    isotopes = dict(ISOTOPE.findall(text))
    length_stats = {
        isotope: {"count": int(count), "mean": float(mean), "rms": float(rms)}
        for isotope, count, mean, rms in LENGTH_STATS.findall(text)
    }
    thresholds = {int(threshold): int(count) for threshold, count in THRESHOLD.findall(text)}
    containment_bins = {
        (float(low), float(high)): int(count)
        for low, high, count in CONTAINMENT_BIN.findall(text)
    }
    containment_summary = CONTAINMENT_SUMMARY.search(text)
    gamma = re.search(
        r"Capture gamma summary: events = (\d+) mean_energy = ([0-9.eE+-]+) (\w+) "
        r"mean_multiplicity = ([0-9.eE+-]+)",
        text,
    )
    return {
        "h": h, "gd": gd, "other": other, "share": gd / total if total else 0,
        "events": int(EVENTS.search(text).group(1)) if EVENTS.search(text) else 0,
        "escaped": int(escaped.group(1)) if escaped else 0,
        "isotopes": isotopes, "length_stats": length_stats,
        "thresholds": thresholds, "gamma": gamma.groups() if gamma else None,
        "containment_bins": containment_bins,
        "containment": (
            (int(containment_summary.group(1)),
             float(containment_summary.group(2)),
             float(containment_summary.group(3)))
            if containment_summary else None
        ),
        "bins": bins,
    }


def main():
    if len(sys.argv) != 2:
        raise SystemExit("usage: plot_scans.py scan_results")
    root = Path(sys.argv[1])
    cases = list(csv.DictReader((root / "cases.csv").open(encoding="utf-8")))
    data = {}
    valid_cases = []
    for row in cases:
        try:
            data[row["log"]] = read_log(root / row["log"])
            valid_cases.append(row)
        except ValueError as error:
            print(f"warning: skipping incomplete scan log: {error}", file=sys.stderr)
    cases = valid_cases

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
    energy_groups = {}
    for row in energy:
        energy_groups.setdefault(row["energy"], {})[row["gd_fraction"]] = row
    paired_energy = [
        (label, group["0.0"], group["0.001"])
        for label, group in energy_groups.items()
        if "0.0" in group and "0.001" in group
    ]
    labels = [item[0] for item in paired_energy]
    water = [data[item[1]["log"]]["share"] * 100 for item in paired_energy]
    gd = [data[item[2]["log"]]["share"] * 100 for item in paired_energy]
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

    base = next(
        (row for row in energy
         if row["energy"] == "2.5 MeV" and row["gd_fraction"] == "0.0"),
        None,
    )
    gd_base = next(
        (row for row in energy
         if row["energy"] == "2.5 MeV" and row["gd_fraction"] == "0.001"),
        None,
    )

    if base and gd_base:
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

        # The simulation records time in microseconds. Show milliseconds on
        # the plot and provide an early-time zoom covering 250 microseconds.
        plot_histogram(axes[0, 0], "time", "Capture time (ms)", x_scale=1e-3)
        axes[0, 0].set_title("Full capture-time distribution")
        plot_histogram(axes[0, 1], "time", "Capture time (ms)", x_scale=1e-3)
        axes[0, 1].set_xlim(0, 0.25)
        axes[0, 1].set_title("Early-time zoom: 0–0.25 ms")
        plot_histogram(axes[1, 0], "radius", "Capture radius (cm)")
        axes[1, 0].set_title("Capture-radius distribution")
        axes[1, 1].axis("off")
        fig.suptitle("Capture time and position at 2.5 MeV")
        fig.tight_layout()
        fig.savefig(root / "capture_time_position_comparison.png", dpi=150)
        plt.close(fig)
    else:
        print(
            "warning: skipping 2.5 MeV time/position plot; "
            "both water and Gd cases must be complete",
            file=sys.stderr,
        )

    radius_cases = [row for row in cases if row["scan"] == "radius"]
    if radius_cases:
        fig, ax = plt.subplots(figsize=(8, 4.5))
        for fraction, label in (("0.0", "pure water"), ("0.001", "0.1% Gd water")):
            selected = [row for row in radius_cases if row["gd_fraction"] == fraction]
            selected.sort(key=lambda row: float(row["value"]))
            ax.plot([float(row["value"]) for row in selected],
                    [100. * data[row["log"]]["escaped"] / max(data[row["log"]]["events"], 1)
                     for row in selected], "o-", label=label)
        ax.set_xlabel("Absorber radius (cm)")
        ax.set_ylabel("Escaped-neutron probability (%)")
        ax.set_title("Escape probability versus detector size")
        ax.grid(True, alpha=0.25)
        ax.legend()
        fig.tight_layout()
        fig.savefig(root / "escape_probability_vs_radius.png", dpi=150)
        plt.close(fig)

    isotope_counts = {}
    for row in cases:
        for isotope, count in data[row["log"]]["isotopes"].items():
            isotope_counts[isotope] = isotope_counts.get(isotope, 0) + int(count)
    if isotope_counts:
        fig, ax = plt.subplots(figsize=(8, 4.5))
        labels = sorted(isotope_counts)
        ax.bar(labels, [isotope_counts[label] for label in labels])
        ax.set_xlabel("Capture residual nucleus")
        ax.set_ylabel("Capture count")
        ax.set_title("Isotope-level capture breakdown")
        ax.tick_params(axis="x", rotation=35)
        fig.tight_layout()
        fig.savefig(root / "capture_isotope_breakdown.png", dpi=150)
        plt.close(fig)

    length_stats = {}
    for row in cases:
        for isotope, stats in data[row["log"]]["length_stats"].items():
            length_stats.setdefault(isotope, []).append(stats["mean"])
    if length_stats:
        fig, ax = plt.subplots(figsize=(8, 4.5))
        labels = sorted(length_stats)
        ax.boxplot([length_stats[label] for label in labels])
        ax.set_xticks(range(1, len(labels) + 1), labels)
        ax.set_xlabel("Capture residual nucleus")
        ax.set_ylabel("Mean capture path length (cm)")
        ax.set_title("Capture-length statistics by isotope")
        ax.grid(True, axis="y", alpha=0.25)
        fig.tight_layout()
        fig.savefig(root / "capture_length_by_isotope.png", dpi=150)
        plt.close(fig)

    response_rows = [row for row in cases if row["scan"] == "energy" and row["energy"] == "2.5 MeV"]
    if response_rows:
        fig, axes = plt.subplots(1, 2, figsize=(10, 4.5))
        for row in response_rows:
            label = "pure water" if row["gd_fraction"] == "0.0" else "0.1% Gd water"
            result = data[row["log"]]
            thresholds = sorted(result["thresholds"])
            axes[0].plot(thresholds, [result["thresholds"][t] for t in thresholds],
                         "o-", label=label)
            if result["gamma"]:
                axes[1].bar(label, float(result["gamma"][1]), label=label)
        axes[0].set_xlabel("Deposited-energy threshold (MeV)")
        axes[0].set_ylabel("Events above threshold")
        axes[0].set_title("Detector energy response")
        axes[0].grid(True, alpha=0.25)
        axes[0].legend()
        axes[1].set_ylabel("Mean capture-gamma energy (reported units)")
        axes[1].set_title("Capture gamma response")
        fig.tight_layout()
        fig.savefig(root / "detector_response.png", dpi=150)
        plt.close(fig)

        fig, axes = plt.subplots(1, 2, figsize=(11, 4.5))
        for row in response_rows:
            label = "pure water" if row["gd_fraction"] == "0.0" else "0.1% Gd water"
            result = data[row["log"]]
            bins = sorted(result["containment_bins"].items())
            if bins:
                edges = [item[0][0] for item in bins] + [bins[-1][0][1]]
                axes[0].step(edges, [item[1] for item in bins] + [bins[-1][1]],
                             where="post", label=label)
            if result["containment"]:
                axes[1].errorbar(
                    [label], [result["containment"][1]],
                    yerr=[result["containment"][2]], fmt="o", capsize=4,
                    label=label,
                )
        axes[0].set_xlabel("Deposited energy / capture-gamma energy")
        axes[0].set_ylabel("Capture-event count")
        axes[0].set_title("Capture energy-containment proxy")
        axes[0].set_xlim(0, 1.5)
        axes[0].grid(True, alpha=0.25)
        axes[0].legend()
        axes[1].set_ylabel("Mean containment proxy (mean ± RMS)")
        axes[1].set_title("Mean energy containment")
        axes[1].grid(True, axis="y", alpha=0.25)
        fig.tight_layout()
        fig.savefig(root / "energy_containment.png", dpi=150)
        plt.close(fig)

    if radius_cases:
        fig, ax = plt.subplots(figsize=(8, 4.5))
        for fraction, label in (("0.0", "pure water"), ("0.001", "0.1% Gd water")):
            selected = [row for row in radius_cases if row["gd_fraction"] == fraction]
            selected.sort(key=lambda row: float(row["value"]))
            selected = [row for row in selected if data[row["log"]]["containment"]]
            ax.errorbar(
                [float(row["value"]) for row in selected],
                [data[row["log"]]["containment"][1] for row in selected],
                yerr=[data[row["log"]]["containment"][2] for row in selected],
                fmt="o-", capsize=3, label=label,
            )
        ax.set_xlabel("Absorber radius (cm)")
        ax.set_ylabel("Mean containment proxy (mean ± RMS)")
        ax.set_title("Energy containment versus detector size")
        ax.grid(True, alpha=0.25)
        ax.legend()
        fig.tight_layout()
        fig.savefig(root / "energy_containment_vs_radius.png", dpi=150)
        plt.close(fig)
    print(f"saved plots in {root}")


if __name__ == "__main__":
    main()
