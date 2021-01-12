import os, shutil

mark = 1
CompilerVersion = 'vs2017_x64'
if mark == 1:
    CompilerVersion = 'vs2015_x64'

src_dir_debug = ["D:/Build/" + CompilerVersion + "/zoo/bin/Debug", "D:/Build/" + CompilerVersion + "/zoo/bin/Debug/plugins"]
src_dir_release = ["D:/Build/" + CompilerVersion + "/zoo/bin/Release", "D:/Build/" + CompilerVersion + "/zoo/bin/Release/plugins"]

target_dir_debug = ["D:/Build/" + CompilerVersion + "/zoo-demo/bin/Debug", "D:/Build/" + CompilerVersion + "/zoo-demo/bin/Debug/plugins"]
target_dir_release = ["D:/Build/" + CompilerVersion + "/zoo-demo/bin/Release", "D:/Build/" + CompilerVersion + "/zoo-demo/bin/Release/plugins"]

def copy_files(src_dir, target_dir, idx):
    for fimename in os.listdir(src_dir[idx]):
        if fimename.endswith('.exe') or fimename.endswith('.dll'):
            shutil.copy(os.path.join(src_dir[idx], fimename), target_dir[idx])

copy_files(src_dir_debug, target_dir_debug, 0)
copy_files(src_dir_debug, target_dir_debug, 1)
copy_files(src_dir_release, target_dir_release, 0)
copy_files(src_dir_release, target_dir_release, 1)
