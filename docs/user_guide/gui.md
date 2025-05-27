(chp:gui)=

# GUI

The following chapter describes the graphical user interface (GUI) of the application.

```{contents}
:depth: 2
:local:
```

(sec:gui:MainWindow)=

## MainWindow

```{figure} figures/MainWindowHierarchy.png
:name: gui:MainWindow
:align: center

main window

```

The MainWindow of the application consists of two parts:

1. The menu bar and the buttons to control the displayed the
   graphical view of the netlist.

2. The viewing area shows the graphical representation of the netlist.

3. The hierarcy section shows a tree that models the hierarchy of the submodules in the netlist.
   This is enabled by the `Toggle Hierarchical View` button in the menu bar. (See {ref}`sec:gui:MainWindow:MenuBar:Buttons`)

(sec:gui:MainWindow:MenuBar)=

### Menu Bar

(sec:gui:MainWindow:MenuBar:Buttons)=

#### Buttons

The following [figure](gui:MenuBar) shows all the available buttons in the menu bar.

```{figure} figures/MenuBar.png
:name: gui:MenuBar
:align: center

menu bar

```

The following section describes the functionality of each button in the menu bar.

1. **Open File:** opens the file dialog to select the JSON file to view.
2. **Zoom In:** zooms in the graphical view.
3. **Zoom Out:** zooms out the graphical view.
4. **Zoom to Fit:** fits the whole diagram into the viewing area.
5. **Toggle Names:** toggles between hiding and showing the names of the signals
6. **Search Node:** opens a search dialog to search for a node in the netlist.
7. **Toggle Hierarchical View:** shows or hides a tree that models the hierarchy
   of the submodules in the netlist.

(sec:gui:MainWindow:MenuBar:Menus)=

#### Menus

The following [figure](gui:Menu) shows all the available menus in the menu bar.

```{figure} figures/Menu.png
:name: gui:Menu
:align: center

menus

```

The following section describes the functionality of each menu in the menu bar.

1. **File:** contains options to open or save files, the settings menu, and the exit button.
2. **View:** contains functions to manipulate the view of the netlist.
3. **Info:** contains information about the application

(sec:gui:MainWindow:MenuBar:FileMenu)=

#### File Menu

The following [figure](gui:FileMenu) shows all the available options in the file menu.

```{figure} figures/FileMenu.png
:name: gui:FileMenu
:align: center

file menu

```

The following section describes the functionality of each option in the file menu.

1. **Open File...:** opens the file dialog to select the JSON file to view.
2. **Load Example:** contains example designs that can be loaded
   - **adder.rtl.json**
   - **addersystem.rtl.json**
   - **adder.tech.json**
3. **Export .svg from:**
   - **Schematic:** exports the whole diagram shown in the view as an svg file.
   - **Selected::** exports the selected part of the diagram as an svg file.
4. **Settings...:** opens the settings dialog to change the settings of the application.
5. **Exit:** closes the application. _Note: this is not available in the web version._

(sec:gui:MainWindow:MenuBar:ViewMenu)=

#### View Menu

The following [figure](gui:ViewMenu) shows all the available options in the view menu.

```{figure} figures/MenuView.png
:name: gui:ViewMenu
:align: center

view menu

```

The following section describes the functionality of each option in the view menu.

All options except for the **Clear Highlight** option are explained in
{ref}`sec:gui:MainWindow:MenuBar`.

1. **Clear Highlight:** clears the highlighted nodes and edges in the diagram.

(sec:gui:MainWindow:MenuBar:InfoMenu)=

#### Info Menu

The following [figure](gui:InfoMenu) shows all the available options in the info menu.

```{figure} figures/MenuInfo.png
:name: gui:InfoMenu
:align: center

info menu

```

The following section describes the functionality of each option in the info menu.

1. **Documentation:** opens the documentation in the default browser. **Currently not implemented**
2. **About:** shows information about the application.

(sec:gui:MainWindow:ViewingArea)=

### Viewing Area

The viewing area shows the graphical representation of the netlist.
When first opening the application without any command-line arguments **(see)**
the viewing area is empty. As shown in the following [figure](gui:MainWindow).

```{figure} figures/ViewArea.png
:name: gui:ViewArea
:align: center

viewing area

```

When a JSON file is opened, the viewing area shows the graphical representation of the netlist.
Additional the `Modulepath:` appears in the top left corner of the viewing area.
This shows which submodule is currently displayed in the viewing area. A `/` means
that the top module is displayed. An example can be seen in the following [figure](gui:ViewAreaExample).

```{figure} figures/ViewAreaExample.png
:name: gui:ViewAreaExample
:align: center

viewing area with diagram

```

(sec:gui:MainWindow:ViewingArrea:NodeContextMenu)=

#### Node Context Menu

When right-clicking on nodes or ports in the viewing area, the following context menu appears.

```{figure} figures/NodeContext.png
:name: gui:NodeContext
:align: center

Node Context Menu
```

It has the following options:

1. **Highlight:** colors the node or ports border in the color that is selected in the
   color [submenu](gui:HighlightColor) that appears when hovering over the highlight option.
2. **Clear Highlight:** clears the highligting on the node or port
3. **Select Connected:** selects all the paths that are connected to the node or port
4. **Highlight Connectivity:** highlights all the paths that are connected to the node or port. The color is selected in the color [submenu](gui:HighlightColor) that appears when hovering over the highlight option.
5. **Zoom To:** zooms to the node or port
6. **Properties:** opens a window that shows detailed information about the node or port

The Properties window looks like the following [figure](gui:PropertiesWindow).

```{figure} figures/NodePorperties.png
:name: gui:PropertiesWindow
:align: center

Properties Window
```


```{figure} figures/HighlightColorContext.png
:name: gui:HighlightColor
:align: center

Highlight Color Submenu
```



(sec:gui:MainWindow:ViewingArrea:PathContextMenu)=

#### Path Context Menu

When right-clicking on paths in the viewing area, the following context menu appears.


```{figure} figures/PathContext.png
:name: gui:PathContext
:align: center

Path Context Menu
```

1. **Highlight:** colors the path in the color that is selected in the
   color [submenu](gui:HighlightColor) that appears when hovering over the highlight option.
2. **Clear Highlight:** clears the highligting on the path
3. **Go to Source:** zooms to the source node or port of the path
4. **Select Source:** selects the source node or port of the path
5. **Select Destinations:** selects all the destination nodes or ports of the path
6. **Zoom To:** zooms to the path
7. **Properties:** opens a window that shows detailed information about the path

A example of the Properties window can be seen in the following [figure](gui:PathProperties).

```{figure} figures/PathProperties.png
:name: gui:PathProperties
:align: center

Path Properties Window
```


(sec:gui:MainWindow:HierarchicalView)=

### Hierarchical View

The hierarchical view shows a tree that models the hierarchy of the submodules in the netlist.
This is enabled by the `Toggle Hierarchical View` button in the menu bar (see {ref}`sec:gui:MainWindow:MenuBar:Buttons`).
When no JSON file is opened, the hierarchical view is empty. As shown in the following [figure](gui:HierarchicalView).

```{figure} figures/HierarchyTreeEmpty.png
:name: gui:HierarchicalView
:align: center

hierarchical view empty

```

When a JSON file is opened, the hierarchical view shows the hierarchy of the submodules in the netlist.
The top module is displayed at the top of the tree. The submodules are displayed below the top module.
The submodule entries consist of two parts separated by a `:`. The first part is the name of the module defined
in the source code. The second part is the name of the instance inside of the parent module.
For example `counter:U0` means that the submodule `counter` is instantiated in the parent module as `U0`.
An example can be seen in the following [figure](gui:HierarchicalViewExample).
Double-clicking on an entry in the hierarchical view opens the graphical view of the submodule in the viewing area.
If the submodule is already opened, the view switches to the tab of the submodule.

```{figure} figures/HierarchyTree.png
:name: gui:HierarchicalViewExample
:align: center

hierarchical view

```

(sec:gui:MainWindow:HierarchicalView:Settings)=
## Settings Dialog

The settings dialog can be opened by clicking on the `Settings...` option in the file menu.
(See {ref}`sec:gui:MainWindow:MenuBar:FileMenu`)

At the moment it allows the user to change the Symbol file used to display the graphical view of the netlist.
More information about the Symbol file can be found in {ref}`chp:symbols`.

The following [figure](gui:SettingsDialog) shows the settings dialog.

```{figure} figures/SettingsDialog.png
:name: gui:SettingsDialog
:align: center

settings dialog

```

The button `Change` opens a file dialog to select a new symbol file.
When pressing on `Apply` the new symbol file is loaded and the graphical view is updated.
The button `Reset` resets the symbols to the default ones, it is only active when different symbols are loaded.

Additionally, the settings dialog contains the parameters of the rouiting algorithm for
the current module. The parameters influence the resulting diagram that is shown in the viewing area.

A more detailed description of the parameters can be found in the following section.

### Routing Parameters

| Parameter | Description |
|-----------|-------------|
| `X Constraint` | The X constraint is the distance between two nodes in the X direction.  The distance can be smaller than the given value.|
| `Y Constraint` | The Y constraint is the distance between two nodes in the Y direction.  The distance can be smaller than the given value.|
| `Test Tolerance` | The test tolerance is the used to determine when the routing algorithm has found a solution. |
| `Max Iterations` | The maximum number of iterations that the routing algorithm will run. If the maximum number of iterations is reached, the routing algorithm stops and returns the current solution. |
| `Default Edge Length` | Length of edges at the beginning of the calculations|

(sec:gui::SearchDialog)=
## Search Dialog

The search dialog can be opened by clicking on the `Search Node...` option in the menu bar, using the `View->Search Node...` option
or by pressing `Ctrl+F`.

The following [figure](gui:SearchDialog) shows the search dialog.

```{figure} figures/SearchDialog.png
:name: gui:SearchDialog
:align: center

search dialog

```

When a name of a object in the netlist is entered in the search field and the `OK` button is pressed,
if a node with then name exists in the netlist, the view is centered on the object and zoomed in.

## About Dialog

The about dialog can be opened by clicking on the `About` option in the info menu.
It shows information about the application. There is also a button `About Qt` which
shows information about the Qt library.


## Routing Dialog

Because the line routing library can take a long time to calculate the position of paths for big modules, a dialog will appear when the module is bigger than a certain threshold.
The dialog looks like this:

```{figure} figures/BigModuleDialog.png
:name: gui:BigModuleDialog
:align: center

Big Module Dialog
```

## Reload Dialog

When a second JSON file is opened, after the first one was already loaded, a dialog will appear.
This dialog asks if the user if the current diagram should be closed and the new one loaded.

```{figure} figures/ReloadDialog.png
:name: gui:ReloadDialog
:align: center

Reload Dialog
```