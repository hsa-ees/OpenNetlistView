# Configuration file for the Sphinx documentation builder.
#
# For the full list of built-in configuration values, see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Project information -----------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#project-information

import re
import os

project = 'OpenNetlistView'
copyright = '@SPHINX_TARGET_YEAR@, Lukas Bauer'
author = 'Lukas Bauer'

release = re.sub('^v', '', os.popen("git describe --tags").read().strip())

# check if no tag is set
if release == "":
    release = "N/A"

version = release.split("-")[0]

# -- General configuration ---------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#general-configuration

extensions = ["myst_parser",                # Markdown support
              "breathe",                               # doxygen parser
              "sphinx.ext.graphviz",                   # graphviz support
              "sphinxcontrib.inkscapeconverter"]       # convert svg to pdf in latex

numfig = True

templates_path = ['_templates']
exclude_patterns = []

master_doc = 'index'

source_suffix = {
    ".rst": "restructuredtext",
    ":md": "markdown"
}

# -- Options for Myst-Parser -------------------------------------------------

myst_enable_extensions = [
    "amsmath",
    "colon_fence",
    "deflist",
    "dollarmath",
    "fieldlist",
    "html_admonition",
    "html_image",
    "replacements",
    "smartquotes",
    "strikethrough",
    "substitution",
    "tasklist"
]

# -- Options for Breathe -----------------------------------------------------
# https://breathe.readthedocs.io/en/latest/

# breathe_projects = {
#     "OpenNetlistView": "../xml"
# }

# breathe_projects_source = {

#     "OpenNetlistView": ("../include", ["OpenNetlistView.h"])

# }


# -- Options for HTML output -------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#options-for-html-output

html_theme = 'sphinx_rtd_theme'
html_static_path = ['_static']

# -- Options for PDF output --------------------------------------------------

latex_documents = [
    (master_doc, '@SPHINX_TARGET_NAME@.tex',
     'OpenNetlistView Documentation', author.replace(", ", "\\and").replace(" and ", "\\and and "), 'manual')
]

latex_elements = {
    'preamble': r'''\usepackage{float}''',
    'figure_align': r'''H'''
}
