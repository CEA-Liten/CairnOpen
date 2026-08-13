# -*- coding: utf-8 -*-
"""Sensitivity analysis runner with manual sampling for Cairn studies."""

import sys
from pathlib import Path
import pandas as pd

try:
    import cairn as crn
except ImportError:
    import cairnopen as crn

import PerseePasteResults as ppr


def run_sensitivity_manual_sampling(
    testcase: str,
    app_home: Path,
    tab_param_name: str,
    tmax: int,
    timeseries_files: list[str] | None = None
) -> None:

    if timeseries_files is None:
        timeseries_files = []

    # Resolve study path
    name = testcase
    study_path = app_home / (name if name.endswith(".json") else f"{name}.json")

    tab_param_path = app_home / tab_param_name

    cairn_instance = crn.CairnAPI()
    problem = cairn_instance.read_study(str(study_path))

    for ts_file in timeseries_files:
        problem.add_timeseries(ts_file)

    # Load CSV
    df_sens = pd.read_csv(tab_param_path, sep=";", decimal=".", header=[0, 1], index_col=0)

    # ---- Sampling ----
    columns = df_sens.columns.tolist()
    cases = df_sens.index.tolist()

    samplings = []
    for case in cases:
        sampling = [{"Case": case}]
        for col in columns:
            setting = {"model": col[0], "value": df_sens[col][case]}
            if "--" in col[1]:
                port, prop = col[1].split("--", 1)
                setting["port"] = port
                setting["property"] = prop
            else:
                setting["property"] = col[1]
            sampling.append(setting)
        samplings.append(sampling)

    problem.run_sensitivity(samplings, tmax, [])

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
        2. testcase           - Study name (without .json extension)
        3. tab_param_name     - Sampling CSV filename (default: tab_echantillonnage.csv)
        4. tmax               - Solver time limit in seconds (default: 10000)
        5+.                   - Timeseries files 
    """
    if len(argv) <= 5:
        raise ValueError(f"Expected at least 5 arguments, got {len(argv) - 1}.")

    app_home          = Path(argv[1])
    testcase          = argv[2]
    tab_param_name    = argv[3] or "tab_echantillonnage.csv"
    tmax              = int(argv[4]) if argv[4] else 10000

    timeseries_files: list[str] = []

    for arg in argv[6:]:
        if not arg:
            continue
        timeseries_files.append(arg)

    return {
        "app_home":          app_home,
        "testcase":          testcase,
        "tab_param_name":    tab_param_name,
        "tmax":              tmax,
        "timeseries_files":  timeseries_files
    }


if __name__ == "__main__":
    print("----------------------- Run Sensitivity Arguments -----------------------------")
    print(sys.argv)

    args = parse_args(sys.argv)

    app_home:          Path      = args["app_home"]
    testcase:          str       = args["testcase"]
    tab_param_name:    str       = args["tab_param_name"]
    tmax:              int       = args["tmax"]
    timeseries_files:  list[str] = args["timeseries_files"]

    print(f"App Home:               {app_home}")
    print(f"Test Case Name:         {testcase}")
    print(f"Sampling File:          {tab_param_name}")
    print(f"Time Limit (s):         {tmax}")

    # Fall back to the default dataseries CSV if no timeseries files were provided
    default_dataseries = app_home / f"{testcase}_dataseries.csv"
    if not timeseries_files and default_dataseries.exists():
        print(f"No timeseries loaded. Falling back to default: {default_dataseries.name}")
        timeseries_files.append(str(default_dataseries))

    print(f"Timeseries Files:       {timeseries_files}")
    print("-------------------------------------------------------------------------------")

    if not timeseries_files:
        print("Error: no timeseries loaded. Aborting.", flush=True)
        sys.exit(1)

    run_sensitivity_manual_sampling(
        testcase=testcase,
        app_home=app_home,
        tab_param_name=tab_param_name,
        tmax=tmax,
        timeseries_files=timeseries_files
    )
    print("Sensitivity analysis complete.")