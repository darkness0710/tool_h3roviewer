## ABOUT THIS FORK

This is a **modified fork** of `h3roviewer`, originally written by **Grek** <gitgrek@gmail.com>.

- Upstream: <https://gitlab.com/Grekern/h3roviewer>
- Modified by darkness0710, 2026-09-07
- Licensed under **GNU GPL v3** — see [LICENSE.md](LICENSE.md)

Published history here is squashed into a single release commit.
The full upstream commit history lives in the GitLab repository linked above.

----------

## USAGE
Simply start this application and it should automatically try to attach to Heroes 3 HotA.  
It will look for a process named "h3hota.exe" or "h3hota HD.exe".   
If the process is named differently, it will not work.  
As soon as a game is active, the selected hero should appear in the main window.  
Right click in the GUI to bring up the menu.



## BUILDING
This was compiled using QtCreator, Qt 6.6.0, MinGW 11.2.0, 64 bit.  

Get Qt here: https://www.qt.io/download-open-source.  
The default installation should include MinGW.  
Open QtCreator and load the project file "h3roviewer.pro"  
Compile.

To make a standalone package, the binary need some libraries in the same directory as the exe.  
This can be done with the "windeployqt.exe" which is in the \<Qt installation>/mingw_64/bin/  
```
  $ windeployqt.exe <path to the directory where the compiled exe is>
```
It must have the same build environment as when compiling.  
Can be added as a custom build step in QtCreator.
```
Command: %{ActiveProject:QT_HOST_BINS}\windeployqt.exe
Argument: %{buildDir}\release
Working directory: %{buildDir}
```


## CONTACT
email: gitgrek@gmail.com