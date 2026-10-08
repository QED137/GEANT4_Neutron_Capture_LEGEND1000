#!/usr/bin/env python3
"""Run concentration and neutron-energy scans using a built neu executable."""
import argparse
import csv
import os
import subprocess
from pathlib import Path


CONCENTRATIONS = (0.0, 0.00001, 0.00005, 0.0001, 0.0005, 0.001, 0.002, 0.005, 0.01)
ENERGIES = ("0.025 eV", "100 eV", "1 keV", "100 keV", "1 MeV", "2.5 MeV", "14.1 MeV")


def make_macro(path, gd_fraction, energy, events, output_name):
    path.write_text(
        "\n".join(
            [
                "/control/verbose 0",
                "/run/verbose 0",
                "/testhadr/det/setRadius 30 cm",
                f"/testhadr/det/setGdFraction {gd_fraction:.8g}",
                "/run/initialize",
                f"/analysis/setFileName {output_name}",
                "/analysis/h1/set 24 150 0. 300. cm",
                "/analysis/h1/set 25 200 0. 1000. microsecond",
                "/analysis/h1/set 26 100 0. 30. cm",
                "/gun/particle neutron",
                f"/gun/energy {energy}",
                f"/run/beamOn {events}",
                "",
            ]
        ),
        encoding="utf-8",
    )


def run_case(executable, macro, log):
    with log.open("w", encoding="utf-8") as stream:
        subprocess.run(
            [str(executable), str(macro)],
            stdout=stream,
            stderr=subprocess.STDOUT,
            check=True,
        )


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("executable", type=Path)
    parser.add_argument("--output", type=Path, default=Path("scan_results"))
    parser.add_argument("--events", type=int, default=10000)
    args = parser.parse_args()
    args.output.mkdir(parents=True, exist_ok=True)
    macros = args.output / "macros"
    macros.mkdir(exist_ok=True)

    rows = []
    for fraction in CONCENTRATIONS:
        name = f"concentration_{fraction:g}".replace(".", "p")
        macro = macros / f"{name}.mac"
        log = args.output / f"{name}.txt"
        make_macro(macro, fraction, "2.5 MeV", args.events, name)
        run_case(args.executable, macro, log)
        rows.append({"scan": "concentration", "value": fraction, "energy": "2.5 MeV",
                     "gd_fraction": fraction, "log": log.name})

    for energy in ENERGIES:
        for fraction, material in ((0.0, "water"), (0.001, "gd")):
            energy_name = energy.replace(" ", "").replace(".", "p")
            name = f"energy_{energy_name}_{material}"
            macro = macros / f"{name}.mac"
            log = args.output / f"{name}.txt"
            make_macro(macro, fraction, energy, args.events, name)
            run_case(args.executable, macro, log)
            rows.append({"scan": "energy", "value": energy, "energy": energy,
                         "gd_fraction": fraction, "log": log.name})

    with (args.output / "cases.csv").open("w", newline="", encoding="utf-8") as stream:
        writer = csv.DictWriter(stream, fieldnames=("scan", "value", "energy", "gd_fraction", "log"))
        writer.writeheader()
        writer.writerows(rows)
    for path in args.output.rglob("*"):
        mode = path.stat().st_mode | (0o777 if path.is_dir() else 0o666)
        path.chmod(mode)
    args.output.chmod(args.output.stat().st_mode | 0o777)
    print(f"completed {len(rows)} cases in {args.output}")


if __name__ == "__main__":
    main()
