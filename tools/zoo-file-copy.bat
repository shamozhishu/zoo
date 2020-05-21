@echo off
set source_path=D:\Code\_svn\zoo
set target_path=D:\Code\_git\zoo
echo copy[%source_path%]to[%target_path%]:
pause

echo ----------------------------------------src catalog
xcopy /y /s /e %source_path%\src\*.* %target_path%\src\

echo ----------------------------------------plugins catalog
xcopy /y /s /e %source_path%\plugins\*.* %target_path%\plugins\

echo ----------------------------------------applications catalog
xcopy /y /s /e %source_path%\applications\*.* %target_path%\applications\

echo ----------------------------------------tools catalog
xcopy /y /s /e %source_path%\tools\*.* %target_path%\tools\

echo ----------------------------------------include catalog
xcopy /y /s /e %source_path%\include\*.* %target_path%\include\

echo ----------------------------------------build catalog
if not exist %target_path%\build\vs2017_x64 md %target_path%\build\vs2017_x64
copy %source_path%\build\vs2017_x64\zoo.sln %target_path%\build\vs2017_x64\zoo.sln

xcopy /y %source_path%\build\vs2017_x64\src\zoo\*.vcxproj   %target_path%\build\vs2017_x64\src\zoo\*.vcxproj
xcopy /y %source_path%\build\vs2017_x64\src\zoo\*.vcxproj.* %target_path%\build\vs2017_x64\src\zoo\*.vcxproj.*
xcopy /y %source_path%\build\vs2017_x64\src\zooCmd\*.vcxproj   %target_path%\build\vs2017_x64\src\zooCmd\*.vcxproj
xcopy /y %source_path%\build\vs2017_x64\src\zooCmd\*.vcxproj.* %target_path%\build\vs2017_x64\src\zooCmd\*.vcxproj.*
xcopy /y %source_path%\build\vs2017_x64\src\zooCmd_osg\*.vcxproj   %target_path%\build\vs2017_x64\src\zooCmd_osg\*.vcxproj
xcopy /y %source_path%\build\vs2017_x64\src\zooCmd_osg\*.vcxproj.* %target_path%\build\vs2017_x64\src\zooCmd_osg\*.vcxproj.*
xcopy /y %source_path%\build\vs2017_x64\src\zooCmdLoader\*.vcxproj   %target_path%\build\vs2017_x64\src\zooCmdLoader\*.vcxproj
xcopy /y %source_path%\build\vs2017_x64\src\zooCmdLoader\*.vcxproj.* %target_path%\build\vs2017_x64\src\zooCmdLoader\*.vcxproj.*

xcopy /y %source_path%\build\vs2017_x64\applications\zooCmdUI\*.vcxproj   %target_path%\build\vs2017_x64\applications\zooCmdUI\*.vcxproj
xcopy /y %source_path%\build\vs2017_x64\applications\zooCmdUI\*.vcxproj.* %target_path%\build\vs2017_x64\applications\zooCmdUI\*.vcxproj.*
xcopy /y %source_path%\build\vs2017_x64\applications\zooCmdShell\*.vcxproj   %target_path%\build\vs2017_x64\applications\zooCmdShell\*.vcxproj
xcopy /y %source_path%\build\vs2017_x64\applications\zooCmdShell\*.vcxproj.* %target_path%\build\vs2017_x64\applications\zooCmdShell\*.vcxproj.*

xcopy /y %source_path%\build\vs2017_x64\plugins\libctk_war\*.vcxproj   %target_path%\build\vs2017_x64\plugins\libctk_war\*.vcxproj
xcopy /y %source_path%\build\vs2017_x64\plugins\libctk_war\*.vcxproj.* %target_path%\build\vs2017_x64\plugins\libctk_war\*.vcxproj.*
xcopy /y %source_path%\build\vs2017_x64\plugins\libctk_zoocmd_ui\*.vcxproj   %target_path%\build\vs2017_x64\plugins\libctk_zoocmd_ui\*.vcxproj
xcopy /y %source_path%\build\vs2017_x64\plugins\libctk_zoocmd_ui\*.vcxproj.* %target_path%\build\vs2017_x64\plugins\libctk_zoocmd_ui\*.vcxproj.*
xcopy /y %source_path%\build\vs2017_x64\plugins\zooCmd_osg-world\*.vcxproj   %target_path%\build\vs2017_x64\plugins\zooCmd_osg-world\*.vcxproj
xcopy /y %source_path%\build\vs2017_x64\plugins\zooCmd_osg-world\*.vcxproj.* %target_path%\build\vs2017_x64\plugins\zooCmd_osg-world\*.vcxproj.*
xcopy /y %source_path%\build\vs2017_x64\plugins\libqt5_pluginfwadmin\*.vcxproj   %target_path%\build\vs2017_x64\plugins\libqt5_pluginfwadmin\*.vcxproj
xcopy /y %source_path%\build\vs2017_x64\plugins\libqt5_pluginfwadmin\*.vcxproj.* %target_path%\build\vs2017_x64\plugins\libqt5_pluginfwadmin\*.vcxproj.*

echo ----------------------------------------

pause