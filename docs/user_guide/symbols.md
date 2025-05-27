(chp:symbols)=

# Symbolfile format

This chapter describes the format of the symbol file. The symbol file is a SVG file
that contains the symbols that are used as graphical representation for the
nodes in the netlist. The file is based on the format used by netlistsvg.
A description of the format can be found in the
[netlistsvg documentation](https://github.com/nturley/netlistsvg?tab=readme-ov-file#skin-file).

The file is structured into svg groups. Each group represents a symbol.

The following is an example of a symbol file:

```xml

  <g s:type="and" transform="translate(150,50)" s:width="30" s:height="25">
    <s:alias val="$and" />
    <s:alias val="$logic_and" />
    <s:alias val="$_AND_" />

    <path d="M0,0 L0,25 L15,25 A15 12.5 0 0 0 15,0 Z" fill="#eebaff" class="$cell_id" />

    <g s:x="0" s:y="5" s:pid="A" />
    <g s:x="0" s:y="20" s:pid="B" />
    <g s:x="30" s:y="12.5" s:pid="Y" />
  </g>

```

Some values and tags are custom to the symbol file format. The following table
lists the custom tags and their meaning:

| Tag        | Description                                                                           |
| ---------- | ------------------------------------------------------------------------------------- |
| `s:type`   | The type of the symbol. This is used to match the symbol to the node in the netlist.  |
| `s:width`  | The width of the bounding box of the symbol.                                          |
| `s:height` | The height of the bounding box of the symbol.                                         |
| `s:alias`  | An alias for the symbol. This is used to match the symbol to the node in the netlist. |

Additional to the tags listed above, the group contains more groups each of them
representing a pin of the symbol. Each group contains the following tags:

| Tag     | Description                                                           |
| ------- | --------------------------------------------------------------------- |
| `s:x`   | The x position of the pin relative to the symbol.                     |
| `s:y`   | The y position of the pin relative to the symbol.                     |
| `s:pid` | The pin id. This is used to match the pin to the node in the netlist. |


(sec:symbol:differences)=
## Differences to netlistsvg

The format used in the OpenNetlistView is based on the format used by netlistsvg.
But there are some differences between the two formats.

The following list shows the differences between the two formats:

1. SVG text tags are not used for naming in the OpenNetlistView format. They are instead
generated directly inside the graphics view. Using SVG text tags for naming could
lead to overlapping text in the graphics view. Thus it is not recommended to use

2. Setting two ports at the exact same position is not supported, because it causes
problems with the routing of edges.

3. Defining colors for all elements inside the svg file via the `svg` tag is not supported.
Colors should be defined for each element individually. This is a limitation of
the QT framework.

(sec:symbol:default)=
## Default symbols

The default symbols that are used in OpenNetlistView can be found in the
repository in the `resources/symbols` directory.

These can be used as an example for the use in the following sections.

(sec:symbol:customizing)=

## Customizing an existing symbol

When a symbol is redefined not only the SVG representation of the symbol needs to be changed.
Additionally when the size of the symbol changes, the bounding box of the symbol needs to be updated.
If the ports are not in the correct position, the position of the ports needs to be updated
as well.

The Name and Alias should stay the same if the symbol will be used for the same
type of nodes as before. If the symbol should be used for more types of nodes
an alias can be added.

(sec:symbol:custom)=

## Defining a custom symbol

It is possible to define custom symbols for e.g specific submodules.

The following is an example of a custom symbol:

```xml

  <g s:type="counter" transform="translate(550,250)" s:width="30" s:height="40">

    <rect width="30" height="40" s:generic="body" fill="#ffae46" rx="0" class="$cell_id" />

    <g transform="translate(30, 30)" s:x="30" s:y="30" s:pid="count">
    </g>
    <g transform="translate(0, 10)" s:x="0" s:y="10" s:pid="clk">
    </g>
    <g transform="translate(0, 30)" s:x="0" s:y="30" s:pid="reset">
    </g>
  </g>
```

To define a custom symbol the following things need to be done:

1. Add a group to the symbol file.

2. The group needs to contain a `s:type` tag. The type must match
   the name of the module inside the netlist, this is generated form the
   module name in the source code (Verilog, VHDL, ...).

3. The ports need to be be named with the `s:pid` tag. The name must match
   the name of the port in the netlist, which are generated from the port
    names in the source code (Verilog, VHDL, ...).
