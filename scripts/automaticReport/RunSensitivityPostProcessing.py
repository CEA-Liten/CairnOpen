# -*- coding: utf-8 -*-
"""Sensitivity post processing."""

import sys
from pathlib import Path
import pandas as pd

import PerseePasteResults as ppr


def run_sensitivity_post_processing(app_home: Path, tab_param_name: str) -> None:
    # Resolve  path
    tab_param_path = app_home / tab_param_name

    # Load CSV
    df_sens = pd.read_csv(tab_param_path, sep=";", decimal=".", header=[0, 1], index_col=0)

    # Export sumupall file
    ppr.PasteResultsMonoLoc(
        str(app_home),
        "Report_s",
        "PLAN",
        file_out="sumupall_sens.csv",
        list_order=df_sens.index,
    )

def parse_args(argv: list[str]) -> dict:
    """
    Parse positional command-line arguments passed by PerseeGUI.

    Argument order (do not change — interface is fixed by PerseeGUI):
        1. app_home           - Working directory containing the study files
        2. tab_param_name     - Sampling CSV filename (default: tab_echantillonnage.csv)
    """
    if len(argv) <= 2:
        raise ValueError(f"Expected at least 2 arguments, got {len(argv) - 1}.")

    app_home          = Path(argv[1])
    tab_param_name    = argv[2] or "sampling.csv"

    return {
        "app_home":       app_home,
        "tab_param_name": tab_param_name
    }


if __name__ == "__main__":
    print("----------------------- Post Processing Arguments -----------------------------")
    print(sys.argv)

    args = parse_args(sys.argv)

    app_home:          Path      = args["app_home"]
    tab_param_name:    str       = args["tab_param_name"]

    print(f"App Home:      {app_home}")
    print(f"Sampling File: {tab_param_name}")

    run_sensitivity_post_processing(
        app_home=app_home,
        tab_param_name=tab_param_name
    )
