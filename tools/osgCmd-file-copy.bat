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

echo ----------------------------------------plugins catalog

xcopy /y %source_path%\plugins\osgCmd-World\*.* %target_path%\plugins\osgCmd-World\*.*
xcopy /y %source_path%\plugins\libqt5_pluginfw_admin\*.* %target_path%\plugins\libqt5_pluginfw_admin\*.*
xcopy /y %source_path%\plugins\libctk_war_scene\*.* %target_path%\plugins\libctk_war_scene\*.*
xcopy /y %source_path%\plugins\libctk_osgcmd_ui\*.* %target_path%\plugins\libctk_osgcmd_ui\*.*
xcopy /y /s /e %source_path%\plugins\libctk_osgcmd_ui\Resources\*.* %target_path%\plugins\libctk_osgcmd_ui\Resources\


echo ----------------------------------------applications catalog

xcopy /y %source_path%\applications\osgCmdShell\*.* %target_path%\applications\osgCmdShell\*.*
xcopy /y %source_path%\applications\osgCmdUI\*.* %target_path%\applications\osgCmdUI\*.*

echo ----------------------------------------tools catalog

xcopy /y /s /e %source_path%\tools\*.* %target_path%\tools\

echo ----------------------------------------data catalog

xcopy /y %source_path%\data\*.* %target_path%\data\*.*

echo ----------------------------------------

pause