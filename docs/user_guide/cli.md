(chp:cli)=

# Command Line Parameters

The OpenNetlistView program can also be invoked with command line parameters.
This enables the use of the OpenNetlistView program in scripts.

```{note}
The the parameters are only available in the native version of the application.
Both for the standalone ELF file and the AppImage.
```

(sec:cli:switches)=

## Switches

```bash
OpenNetlistView [-h] [-v] [-s skin] [json-file]
```

The following switches are available:

| Switch     | Description                       |
| ---------- | --------------------------------- |
| -h, --help | Show help message and exit        |
| -v         | Show version information and exit |
| -s         | set the skin file to use          |

The `json-file` parameter is the Yosys JSON file to be loaded. If no file is given, the program will start with an empty workspace.
