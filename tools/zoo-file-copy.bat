@echo off
set source_path=D:\Code\_svn\zoo
set target_path=D:\Code\_git\zoo
echo copy[%source_path%]to[%target_path%]:
pause

echo ----------------------------------------libs catalog
xcopy /y /s /e %source_path%\libs\*.* %target_path%\libs\

echo ----------------------------------------plugins catalog
xcopy /y /s /e %source_path%\plugins\*.* %target_path%\plugins\

echo ----------------------------------------applications catalog
xcopy /y /s /e %source_path%\applications\*.* %target_path%\applications\

echo ----------------------------------------tools catalog
xcopy /y /s /e %source_path%\tools\*.* %target_path%\tools\

echo ----------------------------------------include catalog
xcopy /y /s /e %source_path%\include\*.* %target_path%\include\

echo ----------------------------------------CMakeModules catalog
xcopy /y /s /e %source_path%\CMakeModules\*.* %target_path%\CMakeModules\

copy %source_path%\CMakeLists.txt %target_path%\CMakeLists.txt

pause
