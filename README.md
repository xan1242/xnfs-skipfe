# XNFS-SkipFE
SkipFE and JoyLog configurator for Need for Speed games by EA Black Box

## Game compatibility
Current titles that are supported
- Need for Speed: Most Wanted
- Need for Speed: Carbon (broken JoyLog)

Support for other games coming soon.

## Building
The included solution is made from VS2015, however, you should be able to build this with any Visual Studio or Dev C++.

- Open XNFS-SkipFE.sln
- Select the configuration of the game that you desire.
- Output file will be in the Release(-GameExt).

## Running
To run this, you need ThirteenAG's Ultimate ASI loader, which you can get from the widescreen packs here: https://thirteenag.github.io/wfp#nfs

After installing the ASI loader (by putting dinput8.dll in game's root folder), simply copy the compiled .asi and the .ini into the scripts folder which is also located in the game's root directory.
