To generate automatically the Cairn Python API documentation:

1) uninstall cairn package from envDocCairn

2) Modify PerseeBind.cpp to include additional comments for instance

3) Build the wheel with deployWheel.bat

4) install cairn package in envDocCairn

5) run batch script buildDoc.bat by providing the python environment in which Cairn is installed

In order to transform a jupyter notebook into a html:
jupyter nbconvert [path to notebook] --to html --embed-images --output-dir [directory]

In order to insert the html in a .rst file:
.. raw:: html
    :file: Draft_APIPython_Doc_Notebook.html
