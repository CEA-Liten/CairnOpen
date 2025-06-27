# Cairn 

![logo_cairn](resources/images/cairnopen.png)

## 🏔️ Description
Cairn is a simulation and optimization software for energy and environmental systems. It allows for the modeling of technical components and the simulation of their behavior in various scenarios. Cairn is designed to be used in research and engineering environments to evaluate the environmental and economic impacts of energy systems. 🌍🔍
Cairn is developped by the CEA-Liten in Grenoble. 

## 🌟Features
- 🛠️ Modeling of technical components. 
- ⚡ Simulation of energy scenarios. 
- 🌳 Evaluation of environmental impacts. 
- 📈 Optimization of system configurations, sizing and management. 
- 🖥️ Graphical user interface for configuration and visualization of results.

## 🛠️ Prerequisites
Before installing Cairn, ensure your system meets the following prerequisites:

- Operating System: Windows, Linux, macOS. 
- C++ compiler compatible with C++17 or higher (for instance CMake)
- Python 3.10 or higher

## ⚙️ Dependances 
- Qt libraries (version 5.12 or higher). 
- Eigen libraries (version 3.3 or higher).
- Highs solver
- [MIPModeler](https://github.com/CEA-Liten/MIPModeler)
- [LSET CMakeTools](https://github.com/CEA-Liten/LSET_CMakeTools)

## 📦 Installation
They are have several options to use Cairn:
- **Cairn Viewer**, a graphical interface, can be installed on Windows PC : see releases.
- **Cairn Python API** wheels are available on releases. 

## 🚀 Usage
To use Cairn, follow these steps:

- Insall Cairn. 
- Build and configure your model parameters using the graphical user interface or the API.
- Run the simulation by clicking the "Run" button.
- View the results in the visualization tabs.

## 📚 Documentation
For more information on using Cairn, refer to the online [documentation](https://cea-liten.github.io/CairnOpen/).

## 📞 Support
If you encounter any issues or have questions, you can consult the contact technical support at pimprenelle.parmentier at cea point fr. 

## 🤝 Contribution
If you wish to contribute to the development of Cairn, please refer to the CONTRIBUTING.md file for more information on how to submit pull requests and report bugs. 

## 📜 License
Cairn is distributed under the Eclipse Public Licence V2. For more information, see the LICENSE file. 

# 📖 Cite Cairn

Cairn has been presented in ECOS converence in 2024. The paper is available [here](https://cea.hal.science/cea-04681216).

Please cite as follows:

```
@inproceedings{ruby:cea-04681216,
  TITLE = {{PERSEE, a single tool for various optimizations of multi-carrier energy system sizing and operation}},
  AUTHOR = {Ruby, Alain and Parmentier, Pimprenelle and Crevon, St{\'e}phanie and Gaoua, Yacine and Piguet, Antoine and Wissocq, Thibaut and Leoncini, Gabriele and Lavialle, Gilles},
  URL = {https://cea.hal.science/cea-04681216},
  BOOKTITLE = {{ECOS 2024 - 37th International Conference on Efficiency, Cost, Optimization, Simulation and Environmental Impact of Energy Systems}},
  ADDRESS = {Rhodes, Greece},
  YEAR = {2024},
  MONTH = Jun,
  PDF = {https://cea.hal.science/cea-04681216v1/file/ECOS2024_Paper_112.pdf},
  HAL_ID = {cea-04681216},
  HAL_VERSION = {v1},
}
```
