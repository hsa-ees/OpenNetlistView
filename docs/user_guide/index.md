# OpenNetlistView user guide

**Introduction**

OpenNetlistView is a tool for viewing netlists in the [Yosys](https://yosyshq.readthedocs.io/en/latest/)
JSON format. It is designed to run as an native application and a WASM application in a web browser.

The project was developed as part of a master thesis. The finished thesis can be found
[here](../../../../docs/technical_report/masterthesis.pdf).

**Note:** The current version is a prototype. The generation of schematics for bigger and more
complex designs can be very slow.

Information on how to install and use OpenNetlistView can be found in the following sections.

A good starting point is the {ref}`chp:prebuilt` section.

```{figure} figures/Example.png
:name: fig:Example
:align: center
```

```{toctree}
:maxdepth: 2
:caption: "Getting Started:"

prebuilt_app.md
building_app.md
first_diag.md


```

```{toctree}
:maxdepth: 2
:caption: "Using the OpenNetlistView application"

cli.md
gui.md
```

```{toctree}
:maxdepth: 2
:caption: "Appendix"

symbols.md

```
