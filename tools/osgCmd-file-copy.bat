@echo off
set source_path=D:\Code\#svn#\osgCmd
set target_path=D:\Code\osgCmd
echo copy[%source_path%]to[%target_path%]:
pause

echo ----------------------------------------

if not exist %target_path% md %target_path%
copy %source_path%\osgCmd.sln %target_path%\osgCmd.sln

echo ----------------------------------------src catalog

xcopy /y %source_path%\src\*.* %target_path%\src\*.*
xcopy /y /s /e %source_path%\src\osgCmd\*.* %target_path%\src\osgCmd\

echo ----------------------------------------resource catalog

xcopy /y %source_path%\resource\*.* %target_path%\resource\*.*

echo ----------------------------------------plugins catalog

xcopy /y %source_path%\plugins\osgCmd-World\*.* %target_path%\plugins\osgCmd-World\*.*

echo ----------------------------------------applications catalog

xcopy /y %source_path%\applications\osgCmdShell\*.* %target_path%\applications\osgCmdShell\*.*
xcopy /y %source_path%\applications\osgCmdUI\*.* %target_path%\applications\osgCmdUI\*.*

echo ----------------------------------------tools catalog

xcopy /y /s /e %source_path%\tools\*.* %target_path%\tools\

echo ----------------------------------------

pause