# Configuration file for the Sphinx documentation builder.
#
# For the full list of built-in configuration values, see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Project information -----------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#project-information

project = 'Cairn'
copyright = '2024, CEA'
author = 'CEA'
release = '4.5.0'

# -- General configuration ---------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#general-configuration
# for autosectionlabels: 
#     see https://docs.readthedocs.io/en/stable/guides/cross-referencing-with-sphinx.html#explicit-targets

import os
import sys

def readConfig(param):
    with open("./config.ini", 'r', encoding='utf-8') as infile:
        for line in infile:
            if line.startswith(param):
                return line.split("=")[1]

CAIRN_PATH = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '../../../lib/export/Persee/core/'))
sys.path.insert(0, os.path.join(CAIRN_PATH, "api"))
sys.path.append(os.path.join(CAIRN_PATH, "api"))
PYBIND11_PATH="C:\\PythonPegase\\3_10_9\\envDocPersee\\Lib\\site-packages\\pybind11\\include"
sys.path.insert(0,PYBIND11_PATH)
sys.path.append(PYBIND11_PATH)

cea_content_str = readConfig("CEA")
open_source_str = readConfig("OPEN")

print("cea_content_str="+str(cea_content_str))
print("open_source_str="+str(open_source_str))

if "True" in cea_content_str:
    cea_content = True
else:
    cea_content = False

if "True" in open_source_str:
    open_source = True
else:
    open_source = False

print("cea_content="+str(cea_content))
print("open_source="+str(open_source))

extensions = ["breathe", 
			"sphinx.ext.mathjax",
			"sphinx.ext.autosectionlabel",
			"sphinx.ext.ifconfig",
			'sphinx.ext.autodoc',
			'sphinx.ext.napoleon',
			'sphinx.ext.todo'
			]

autodoc_default_options = {
    'members': None,
    'imported-members': True,
    'undoc-members': True,
    'show-inheritance': True,
	'member-order': 'groupwise'
}

def setup(app):
    app.add_config_value('cea_content',cea_content,'html',bool,"Whether the documentation is for CEA")
    app.add_config_value('open_source',open_source,'html',bool,"Whether the documentation is open")

templates_path = ['_templates']
exclude_patterns = []

# Display todos by setting to True
todo_include_todos = True

# -- Options for HTML output -------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#options-for-html-output

html_theme = 'sphinx_rtd_theme'
html_static_path = ['_static']
html_logo = 'images/cairn.png'
html_favicon = '_static/cairn_logo.png'
html_show_search_summary = True
html_copy_source = False
html_theme_options = {
    'logo_only': True,
    'display_version': False,
}
sticky_navigation = True

html_css_files = ['custom.css']

#Breathe configuration for Doxygen
#breathe_default_project = "CatCutifier"

#Figures configuration
numfig = True

numfig_format = {
    'code-block': 'Listing %s',
    'figure': 'Fig. %s',
    'section': 'Section',
    'table': 'Table. %s',
}

rst_prolog = """
.. |cairn| replace:: :term:`Cairn`
.. |milp|   replace:: :term:`MILP`
.. |gui|   replace:: :term:`GUI`
.. |api|   replace:: :term:`API`
.. |hmi|   replace:: :term:`HMI`
.. |lca|   replace:: :term:`LCA`
.. |gams|   replace:: :term:`GAMS`
.. |cplex|   replace:: :term:`CPLEX`
.. |fmi|   replace:: :term:`FMI`
.. |fmu|   replace:: :term:`FMU`
.. |json|   replace:: :term:`JSON`
.. |modelica|   replace:: :term:`MODELICA`
.. |pegase|   replace:: :term:`PEGASE`
.. |csv|   replace:: :term:`CSV`
.. |cop|   replace:: :term:`COP`
.. |python|   replace:: :term:`Python`
.. |capex|   replace:: :term:`CAPEX`
.. |opex|   replace:: :term:`OPEX`
.. |npv|   replace:: :term:`NPV`
.. |irr|   replace:: :term:`IRR`
.. |lcoe|   replace:: :term:`LCOE`
.. |udi|   replace:: :term:`UDI`
.. |co2|   replace:: CO\ :sub:`2`\

.. |turpe|   replace:: :term:`TURPE`
.. |ancillary|   replace:: :term:`Ancillary services`
.. |lhv|   replace:: :term:`LHV`

"""