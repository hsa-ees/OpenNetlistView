(chp:fist_diag)=

# Viewing the first diagram

To view the first diagram you have two option:

- {ref}`sec:first_diag:example`
- {ref}`sec:first_diag:custom`

(sec:first_diag:example)=

## Viewing a example diagram

The OpenNetlistView application provides the possibility to load a
example design that is integrated into the program. To do this you need
to select one from the _Load Example_ menu which is located
inside the _File_ menu. The entries can be seen in {numref}`fig:load_example`

```{figure} figures/FileMenuLoad.png
:name: fig:load_example
:align: center

Load Example Menu

```

After selecting one of the entries the design is loaded.

Further information about the program can be found in the {ref}`sec:fist_diag:further` section

(sec:first_diag:custom)=

## Viewing a custom diagram

In order to create a custom diagram the program yosys is required.
If it is not installed follow the instructions in the next section, otherwise
the section can be skipped.

(sec:fist_diag:custom:installing_yosys)=

### Installing Yosys

It is recommended to use the oss-cad-suite for installing yosys.

This requires the following steps:

1. Download the oss-cad-suite, for your system, from the release page on github.:
   [oss-cad-suite](https://github.com/YosysHQ/oss-cad-suite-build/releases)

2. Extract the archive to a directory of your choice.

3. Add the `bin` directory to your `PATH` environment variable.

4. Run the following command to check if yosys is installed correctly:

```bash
yosys -V
```

(sec:fist_diag:custom:creating_json)=

### Creating the JSON file

```{important}
The following commands all have the `hierarchy -auto-top` command in them.
This is required in any command used to create a JSON file for OpenNetlistView.
Otherwise the program will have problems when determining the top module.
```

(sec:fist_diag:custom:creating_json:verilog)=

#### From a verilog file

To create a JSON file from a verilog file you need to run the following command:

For a register transfer level (RTL) file:

```bash
yosys -p "read_verilog <verilog_file>; hierarchy -auto-top; prep; write_json <json_file>"
```

For a gate level file of the ECP5 FPGA:

```bash
yosys -p "read_verilog <verilog_file>; hierarchy -auto-top; synth_ecp5; write_json <json_file>"
```

(sec:fist_diag:custom:creating_json:vhdl)=

#### From a VHDL file

To create a JSON file from a VHDL file you need to run the following command:

For a register transfer level (RTL) file:

```bash
yosys -m ghdl -p "ghdl <files> -e <top-module>; hierarchy -auto-top; prep; write_json <json_file>"
```

For a gate level file of the ECP5 FPGA:

```bash
yosys -m ghdl -p "ghdl <files> -e <top-module>; hierarchy -auto-top; synth_ecp5; write_json <json_file>"
```

(sec:fist_diag:custom:displaying)=

### Displaying the first Diagram

There are two ways to open the Diagram

1. Open the program and load the JSON file via the `open file` dialog.
   This works for both the native and web assembly version

2. Start the program with the JSON file as an argument via the command line.
   This only works on a native version of the application.
   Further information on this can be found in the chapter {ref}`chp:cli`

````{important}

When the module is being displayed that is bigger then a certain
threshold, a dialog will appear. This is because of the routing library. Which can take
a long time to calculate the position of paths for big modules.

The dialog looks like this:

```{figure} figures/BigModuleDialog.png
:name: fig:big_module_dialog
:align: center

Big Module Dialog

````

(sec:fist_diag:further)=

## Further Reading

- When you want to learn more about the symbol file format or create custom symbols
  you can look at the chapter {ref}`chp:symbols`

- To learn about the functions of the Graphical User Interface (GUI) look at the
  chapter {ref}`chp:gui`

- To learn about the command line parameters look at the chapter {ref}`chp:CLI`
