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

echo ----------------------------------------build\vs2017_x64 catalog
if not exist %target_path%\build\vs2017_x64 md %target_path%\build\vs2017_x64
copy %source_path%\build\vs2017_x64\zoo.sln %target_path%\build\vs2017_x64\zoo.sln
copy %source_path%\build\vs2017_x64\ALL_BUILD.vcxproj %target_path%\build\vs2017_x64\ALL_BUILD.vcxproj
copy %source_path%\build\vs2017_x64\ALL_BUILD.vcxproj.filters %target_path%\build\vs2017_x64\ALL_BUILD.vcxproj.filters
copy %source_path%\build\vs2017_x64\ALL_BUILD.vcxproj.user %target_path%\build\vs2017_x64\ALL_BUILD.vcxproj.user

xcopy /y %source_path%\build\vs2017_x64\src\zoo\*.vcxproj   %target_path%\build\vs2017_x64\src\zoo\*.vcxproj
xcopy /y %source_path%\build\vs2017_x64\src\zoo\*.vcxproj.filters %target_path%\build\vs2017_x64\src\zoo\*.vcxproj.filters
xcopy /y %source_path%\build\vs2017_x64\src\zooCmd\*.vcxproj   %target_path%\build\vs2017_x64\src\zooCmd\*.vcxproj
xcopy /y %source_path%\build\vs2017_x64\src\zooCmd\*.vcxproj.filters %target_path%\build\vs2017_x64\src\zooCmd\*.vcxproj.filters
xcopy /y %source_path%\build\vs2017_x64\src\zooCmd_osg\*.vcxproj   %target_path%\build\vs2017_x64\src\zooCmd_osg\*.vcxproj
xcopy /y %source_path%\build\vs2017_x64\src\zooCmd_osg\*.vcxproj.filters %target_path%\build\vs2017_x64\src\zooCmd_osg\*.vcxproj.filters
xcopy /y %source_path%\build\vs2017_x64\src\zooCmdLoader\*.vcxproj   %target_path%\build\vs2017_x64\src\zooCmdLoader\*.vcxproj
xcopy /y %source_path%\build\vs2017_x64\src\zooCmdLoader\*.vcxproj.filters %target_path%\build\vs2017_x64\src\zooCmdLoader\*.vcxproj.filters

xcopy /y %source_path%\build\vs2017_x64\applications\zooCmdUI\*.vcxproj   %target_path%\build\vs2017_x64\applications\zooCmdUI\*.vcxproj
xcopy /y %source_path%\build\vs2017_x64\applications\zooCmdUI\*.vcxproj.filters %target_path%\build\vs2017_x64\applications\zooCmdUI\*.vcxproj.filters
xcopy /y %source_path%\build\vs2017_x64\applications\zooCmdShell\*.vcxproj   %target_path%\build\vs2017_x64\applications\zooCmdShell\*.vcxproj
xcopy /y %source_path%\build\vs2017_x64\applications\zooCmdShell\*.vcxproj.filters %target_path%\build\vs2017_x64\applications\zooCmdShell\*.vcxproj.filters

xcopy /y %source_path%\build\vs2017_x64\plugins\libctk_war\*.vcxproj   %target_path%\build\vs2017_x64\plugins\libctk_war\*.vcxproj
xcopy /y %source_path%\build\vs2017_x64\plugins\libctk_war\*.vcxproj.filters %target_path%\build\vs2017_x64\plugins\libctk_war\*.vcxproj.filters
xcopy /y %source_path%\build\vs2017_x64\plugins\libctk_zoocmd_ui\*.vcxproj   %target_path%\build\vs2017_x64\plugins\libctk_zoocmd_ui\*.vcxproj
xcopy /y %source_path%\build\vs2017_x64\plugins\libctk_zoocmd_ui\*.vcxproj.filters %target_path%\build\vs2017_x64\plugins\libctk_zoocmd_ui\*.vcxproj.filters
xcopy /y %source_path%\build\vs2017_x64\plugins\zooCmd_osg-world\*.vcxproj   %target_path%\build\vs2017_x64\plugins\zooCmd_osg-world\*.vcxproj
xcopy /y %source_path%\build\vs2017_x64\plugins\zooCmd_osg-world\*.vcxproj.filters %target_path%\build\vs2017_x64\plugins\zooCmd_osg-world\*.vcxproj.filters
xcopy /y %source_path%\build\vs2017_x64\plugins\libqt5_pluginfwadmin\*.vcxproj   %target_path%\build\vs2017_x64\plugins\libqt5_pluginfwadmin\*.vcxproj
xcopy /y %source_path%\build\vs2017_x64\plugins\libqt5_pluginfwadmin\*.vcxproj.filters %target_path%\build\vs2017_x64\plugins\libqt5_pluginfwadmin\*.vcxproj.filters

echo ----------------------------------------build\vs2015_x64 catalog
if not exist %target_path%\build\vs2015_x64 md %target_path%\build\vs2015_x64
copy %source_path%\build\vs2015_x64\zoo.sln %target_path%\build\vs2015_x64\zoo.sln
copy %source_path%\build\vs2015_x64\ALL_BUILD.vcxproj %target_path%\build\vs2015_x64\ALL_BUILD.vcxproj
copy %source_path%\build\vs2015_x64\ALL_BUILD.vcxproj.filters %target_path%\build\vs2015_x64\ALL_BUILD.vcxproj.filters
copy %source_path%\build\vs2015_x64\ALL_BUILD.vcxproj.user %target_path%\build\vs2015_x64\ALL_BUILD.vcxproj.user

xcopy /y %source_path%\build\vs2015_x64\src\zoo\*.vcxproj   %target_path%\build\vs2015_x64\src\zoo\*.vcxproj
xcopy /y %source_path%\build\vs2015_x64\src\zoo\*.vcxproj.filters %target_path%\build\vs2015_x64\src\zoo\*.vcxproj.filters
xcopy /y %source_path%\build\vs2015_x64\src\zooCmd\*.vcxproj   %target_path%\build\vs2015_x64\src\zooCmd\*.vcxproj
xcopy /y %source_path%\build\vs2015_x64\src\zooCmd\*.vcxproj.filters %target_path%\build\vs2015_x64\src\zooCmd\*.vcxproj.filters
xcopy /y %source_path%\build\vs2015_x64\src\zooCmd_osg\*.vcxproj   %target_path%\build\vs2015_x64\src\zooCmd_osg\*.vcxproj
xcopy /y %source_path%\build\vs2015_x64\src\zooCmd_osg\*.vcxproj.filters %target_path%\build\vs2015_x64\src\zooCmd_osg\*.vcxproj.filters
xcopy /y %source_path%\build\vs2015_x64\src\zooCmdLoader\*.vcxproj   %target_path%\build\vs2015_x64\src\zooCmdLoader\*.vcxproj
xcopy /y %source_path%\build\vs2015_x64\src\zooCmdLoader\*.vcxproj.filters %target_path%\build\vs2015_x64\src\zooCmdLoader\*.vcxproj.filters

xcopy /y %source_path%\build\vs2015_x64\applications\zooCmdUI\*.pro   %target_path%\build\vs2015_x64\applications\zooCmdUI\*.pro
xcopy /y %source_path%\build\vs2015_x64\applications\zooCmdUI\*.pro.user %target_path%\build\vs2015_x64\applications\zooCmdUI\*.pro.user
xcopy /y %source_path%\build\vs2015_x64\applications\zooCmdUI\*.vcxproj   %target_path%\build\vs2015_x64\applications\zooCmdUI\*.vcxproj
xcopy /y %source_path%\build\vs2015_x64\applications\zooCmdUI\*.vcxproj.filters %target_path%\build\vs2015_x64\applications\zooCmdUI\*.vcxproj.filters
xcopy /y %source_path%\build\vs2015_x64\applications\zooCmdShell\*.vcxproj   %target_path%\build\vs2015_x64\applications\zooCmdShell\*.vcxproj
xcopy /y %source_path%\build\vs2015_x64\applications\zooCmdShell\*.vcxproj.filters %target_path%\build\vs2015_x64\applications\zooCmdShell\*.vcxproj.filters

xcopy /y %source_path%\build\vs2015_x64\plugins\libctk_war\*.pro   %target_path%\build\vs2015_x64\plugins\libctk_war\*.pro
xcopy /y %source_path%\build\vs2015_x64\plugins\libctk_war\*.pro.user %target_path%\build\vs2015_x64\plugins\libctk_war\*.pro.user
xcopy /y %source_path%\build\vs2015_x64\plugins\libctk_war\*.vcxproj   %target_path%\build\vs2015_x64\plugins\libctk_war\*.vcxproj
xcopy /y %source_path%\build\vs2015_x64\plugins\libctk_war\*.vcxproj.filters %target_path%\build\vs2015_x64\plugins\libctk_war\*.vcxproj.filters
xcopy /y %source_path%\build\vs2015_x64\plugins\libctk_zoocmd_ui\*.pro   %target_path%\build\vs2015_x64\plugins\libctk_zoocmd_ui\*.pro
xcopy /y %source_path%\build\vs2015_x64\plugins\libctk_zoocmd_ui\*.pro.user %target_path%\build\vs2015_x64\plugins\libctk_zoocmd_ui\*.pro.user
xcopy /y %source_path%\build\vs2015_x64\plugins\libctk_zoocmd_ui\*.vcxproj   %target_path%\build\vs2015_x64\plugins\libctk_zoocmd_ui\*.vcxproj
xcopy /y %source_path%\build\vs2015_x64\plugins\libctk_zoocmd_ui\*.vcxproj.filters %target_path%\build\vs2015_x64\plugins\libctk_zoocmd_ui\*.vcxproj.filters
xcopy /y %source_path%\build\vs2015_x64\plugins\zooCmd_osg-world\*.vcxproj   %target_path%\build\vs2015_x64\plugins\zooCmd_osg-world\*.vcxproj
xcopy /y %source_path%\build\vs2015_x64\plugins\zooCmd_osg-world\*.vcxproj.filters %target_path%\build\vs2015_x64\plugins\zooCmd_osg-world\*.vcxproj.filters
xcopy /y %source_path%\build\vs2015_x64\plugins\libqt5_pluginfwadmin\*.pro   %target_path%\build\vs2015_x64\plugins\libqt5_pluginfwadmin\*.pro
xcopy /y %source_path%\build\vs2015_x64\plugins\libqt5_pluginfwadmin\*.pro.user %target_path%\build\vs2015_x64\plugins\libqt5_pluginfwadmin\*.pro.user
xcopy /y %source_path%\build\vs2015_x64\plugins\libqt5_pluginfwadmin\*.vcxproj   %target_path%\build\vs2015_x64\plugins\libqt5_pluginfwadmin\*.vcxproj
xcopy /y %source_path%\build\vs2015_x64\plugins\libqt5_pluginfwadmin\*.vcxproj.filters %target_path%\build\vs2015_x64\plugins\libqt5_pluginfwadmin\*.vcxproj.filters

echo ----------------------------------------

pause