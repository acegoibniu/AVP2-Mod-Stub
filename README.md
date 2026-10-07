# Aliens versus Predator 2 Mod Stub

[![Build](../../actions/workflows/build.yml/badge.svg)](../../actions/workflows/build.yml)

The smallest set of Aliens vs. Predator 2 game binaries that the last
shipped LithTech Talon engine (`lithtech.exe`, AvP2 1.0.9.6) will load and run.
The retail 1.0.9.6 binaries normally serve as the game logic and code that make AvP2 what
it is. This project removes all of that and only fills in what is minimally required
for the LithTech engine to be able to load these client and server shells without crashing.

When this AvP2 mod stub is launched it:

- Prints the message, `AvP2 Mod stub has been loaded!` to the LT console
- Draws the same message in the middle of the screen, with an **Exit** button
  underneath
- Shows a credit line
- Quits lithtech.exe on **Esc**

NO AvP2 game code and no LithTech SDK files are used. The LithTech engine interface
is described by this project's own headers in `include/`.

| File         | Role                                                                    |
|--------------|-------------------------------------------------------------------------|
| `cshell.dll` | Client shell: render mode, messages, Exit button     |
| `cres.dll`   | Client shell resources: string table and mouse cursor        |
| `object.lto` | Server shell + `BaseClass`. Only loaded when a local game is started    |
| `sres.dll`   | Server resources: version string                                        |

## How to play this bare mod?
Build or download the 3 .dll files and the .lto file (4 files total) from the latest release.
Ensure the 4 files are within the same folder inside your Aliens vs. Predator 2 game folder
(which is where you have your AvP2 game installed). Then launch the AVP2 game launcher,
click options and add -rez name_of_folder_with_the_dlls at the end of the line. The game
should launch quickly and show you a message indicating that the mod loaded.

If you know how to load AvP2 mods, this is no different: you just need to launch the game 
while loading the 4 files from this mod either in a .rez file or a folder. Both are loaded 
with the -rez FOLDER_NAME or -rez FILE_NAME.REZ command line parameter. It replaces
the normal AvP2 DLL files and LTO file with the mod's versions, just like loading a 
character skin.

In game: It is very barren. It just shows a message indicating that
CShell.dll had loaded, i.e. the mod loaded. There's an exit button 
and the game will close when the ESC key is pressed. That's it.
This is as bare bones as an AVP2 mod can get. The game should
not crash. The mod doesn't do anything else, it is a stub!

## Building this project

Requirements: Visual Studio 2022 with the C++ workload (its bundled CMake is
used). Nothing else is needed to build the code !

```powershell
.\build.ps1                      # Release
.\build.ps1 -Config Debug
```

Or with any CMake and MSVC: `cmake -S . -B build -A Win32` then
`cmake --build build --config Release`. The build is always Win32; AvP2 is a
32-bit game. GitHub Actions builds every push and keeps the binaries as a
downloadable artifact.

`build.ps1` places the four binaries in `out\`. Mount that folder (or a `.rez`
made from it) and launch the game, AvP2 with `-rez` followed by the name of your 
folder that contains the dll files. Be sure the -rez load appears last in the 
game's command line parameters. Only 1 cshell.dll, cres.dll, object.lto, and 
sres.dll can be loaded by the game.

## How it fits the LithTech engine

- **Exports.** The engine finds the DLLs' functions by name:
  - `cshell.dll`: `GetClientShellFunctions`, `GetClientShellVersion`,
    `SetInstanceHandle` (`SETUP_CLIENTSHELL()` in `include/lt_client.h`)
  - `object.lto`: `GetServerShellFunctions`, `GetServerShellVersion`,
    `SetInstanceHandle` (`SETUP_SERVERSHELL()`) and `ObjectDLLSetup`
    (`LT_DEFINE_CLASSES()`, both in `include/lt_server.h`)
- **Versions.** The engine refuses to load a DLL unless its interface
  version matches what this engine was built for: client shell 2, 
  server shell 2 and object DLL 1.
- **Interface headers.** LithTech hands the DLLs interface pointers
  (`g_pLTClient`, `g_pLTServer`) whose memory layout is fixed by
  `lithtech.exe`: a vtable (a table of function addresses for C++ virtual
  calls) followed by plain function pointers. `include/` declares only the
  entries this project calls, with placeholders for the rest so every entry
  sits at the right offset. `static_assert`s check each offset at compile
  time.
- **No engine library.** Nothing links against `lithtech.exe`; every engine
  service comes in through those interface pointers.
- **Static CRT.** Each DLL carries its own C runtime, so no VC++
  redistributable is needed.

## Legal

- This project's own code, headers, resources and cursor are MIT licensed
  (see `LICENSE`).
- It is an unofficial fan project. It is not made by, supported by or
  affiliated with Monolith Productions, Warner Bros. Games, Sierra, Fox
  Interactive or 20th Century Studios. Aliens vs. Predator 2, LithTech and
  related names are trademarks of their respective owners.
- It contains no game assets and no code or headers from the AvP2 source
  release. The headers in `include/` were written for this project and
  declare only interface facts (names, argument types, offsets) needed to
  talk to the engine.
- You need a legitimately owned copy of AvP2 (1.0.9.6) in order to run it.
