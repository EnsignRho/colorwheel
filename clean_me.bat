@echo off

REM remove all the debug subdirs
echo Removing debug and release folders
for /d /r . %%d in (Debug Release) do @if exist "%%d" echo "%%d" && rd /s/q "%%d"

REM remove all NCB intellisense databases
echo Removing intellisense databases
del /s *.ncb

REM remove build files no longer needed
del colorwheel.exp
del colorwheel.ilk
del colorwheel.lib
del colorwheel.pdb
del *.fxp
del *.bak

REM All done
pause
