(chp:prebuilt)=

# Prebuilt / AppImage

This chapter describes the steps necessary to install the prebuilt AppImage.

```{note}
If you want to build the OpenNetlistView application from source you need to look at
the chapter {ref}`chp:building_it_yourself`.
```

(sec:prebuilt:download)=

## Download the AppImage

Get the latest AppImage File from the release page of the GitHub repo. Which
can be found here [Release Page](https://github.com/hsa-ees/OpenNetlistView/releases)

(sec:prebuilt:mkexec)=

## Make it executable

In order to run the AppImage it needs to be executable. For this run
the following command:

```bash
$ chmod u+x OpenNetlistView-<Version>.AppImage
```

(sec:prebuilt:run)=

## Run it

Run OpenNetlistView with the following command

```bash
$ ./OpenNetlistView-<Version>.AppImage
```

(sec:prebuilt:first_diag)=

## Displaying the first diagram

Information on displaying the first diagram can be found in chapter
{ref}`chp:fist_diag`
