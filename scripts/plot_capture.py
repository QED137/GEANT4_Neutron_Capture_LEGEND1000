#!/usr/bin/env python3
# plot_capture.py -- compare neutron capture share: pure water vs 0.1% Gd water
# usage: python3 plot_capture.py log_water.txt log_gd.txt
import re, sys
import matplotlib.pyplot as plt

logs = sys.argv[1:]
assert len(logs) >= 2, "usage: plot_capture.py <pure water log> <Gd water log>"

labels, caps = [], []
for path in logs:
    txt = open(path).read()
    m = re.search(r"Neutron captures\s*:\s*H = (\d+)\s+Gd = (\d+)\s+other = (\d+)", txt)
    assert m, f"no capture summary found in {path}"
    h, gd, o = map(int, m.groups())
    labels.append("pure water" if gd == 0 else "0.1% Gd water")
    caps.append((h, gd, o))
    print(f"{labels[-1]:>14s}: H={h:6d}  Gd={gd:6d}  other={o:6d}  "
          f"-> Gd share = {100*gd/(h+gd+o):.1f}%")

fig, ax = plt.subplots(figsize=(6, 4))
x = range(len(labels))
h = [c[0] for c in caps]
gd = [c[1] for c in caps]
o = [c[2] for c in caps]
ax.bar(x, h, label="capture on H", color="tab:blue")
ax.bar(x, gd, bottom=h, label="capture on Gd", color="tab:red")
ax.bar(x, o, bottom=[i + j for i, j in zip(h, gd)], label="other", color="gray")
for i, c in enumerate(caps):
    tot = sum(c)
    if c[1] > 0:
        ax.text(i, c[1] / 2 + c[0], f"{100*c[1]/tot:.1f}%",
                ha="center", va="center", color="white", fontsize=12, fontweight="bold")
ax.set_xticks(list(x))
ax.set_xticklabels(labels)
ax.set_ylabel("neutron captures / 100k events")
ax.set_title("Neutron capture share: effect of 0.1% Gd in water")
ax.legend()
plt.tight_layout()
plt.savefig("gd_capture_share.png", dpi=150)
print("saved gd_capture_share.png")
