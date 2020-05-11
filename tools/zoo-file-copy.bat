@echo off
set source_path=D:\Code\#svn#\zoo
set target_path=D:\Code\zoo
echo copy[%source_path%]to[%target_path%]:
pause

echo ----------------------------------------

if not exist %target_path% md %target_path%
copy %source_path%\zoo.sln %target_path%\zoo.sln

echo ----------------------------------------src catalog

xcopy /y %source_path%\src\zoo\*.* %target_path%\src\zoo\*.*
xcopy /y %source_path%\src\zooCmd\*.* %target_path%\src\zooCmd\*.*
xcopy /y %source_path%\src\zooCmd_osg\*.* %target_path%\src\zooCmd_osg\*.*
xcopy /y %source_path%\src\zooCmdLoader\*.* %target_path%\src\zooCmdLoader\*.*

echo ----------------------------------------plugins catalog

xcopy /y %source_path%\plugins\zooCmd_osg-world\*.* %target_path%\plugins\zooCmd_osg-world\*.*
xcopy /y %source_path%\plugins\libqt5_pluginfwadmin\*.* %target_path%\plugins\libqt5_pluginfwadmin\*.*
xcopy /y %source_path%\plugins\libctk_war\*.* %target_path%\plugins\libctk_war\*.*
xcopy /y %source_path%\plugins\libctk_zoocmd_ui\*.* %target_path%\plugins\libctk_zoocmd_ui\*.*
xcopy /y /s /e %source_path%\plugins\libctk_zoocmd_ui\Resources\*.* %target_path%\plugins\libctk_zoocmd_ui\Resources\


echo ----------------------------------------applications catalog

xcopy /y %source_path%\applications\zooCmdShell\*.* %target_path%\applications\zooCmdShell\*.*
xcopy /y %source_path%\applications\zooCmdUI\*.* %target_path%\applications\zooCmdUI\*.*

echo ----------------------------------------tools catalog

xcopy /y /s /e %source_path%\tools\*.* %target_path%\tools\

echo ----------------------------------------include catalog

xcopy /y /s /e %source_path%\include\*.* %target_path%\include\

echo ----------------------------------------

pause