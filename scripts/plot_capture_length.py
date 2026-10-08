#!/usr/bin/env python3
"""Plot neutron capture path-length histograms from simulation logs."""
import re
import sys

import matplotlib.pyplot as plt


if len(sys.argv) < 3:
    raise SystemExit(
        "usage: plot_capture_length.py <pure water log> <Gd water log>"
    )

pattern = re.compile(
    r"capture_length_bin\s+([0-9.eE+-]+)\s+([0-9.eE+-]+)\s+(\d+)"
)

fig, ax = plt.subplots(figsize=(7, 4.5))
for path in sys.argv[1:]:
    bins = {}
    with open(path, encoding="utf-8") as stream:
        for line in stream:
            match = pattern.search(line)
            if match:
                low, high, count = match.groups()
                bins[(float(low), float(high))] = int(count)

    if not bins:
        raise SystemExit(f"no capture-length histogram found in {path}")

    ordered = sorted(bins)
    left = [item[0] for item in ordered]
    counts = [bins[item] for item in ordered]
    label = "pure water" if "gd" not in path.lower() else "0.1% Gd water"
    ax.step(
        left + [ordered[-1][1]],
        counts + [counts[-1]],
        where="post",
        label=label,
    )

ax.set_xlabel("Neutron path length before capture (cm)")
ax.set_ylabel("Capture count")
ax.set_title("Neutron capture path length")
ax.grid(True, alpha=0.25)
ax.legend()
fig.tight_layout()
fig.savefig("capture_length_comparison.png", dpi=150)
print("saved capture_length_comparison.png")
