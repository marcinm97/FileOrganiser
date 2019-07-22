#include "FileOrganiser.h"
#include <thread>
#include <cstdlib>

int main() {

    // create directory (whether not exist)

    auto main_dir(filesystem::current_path().parent_path()/FileManage::dir_name);

    if(!filesystem::exists(main_dir)){
        filesystem::create_directory(filesystem::current_path().parent_path()/FileManage::dir_name);
    }

    FileManage::FileOrganiser manager(main_dir);

    manager.run();


    // 1. If You want to change file name according to your own .txt file, you should add
    // "filenamesList.txt" to cmake-build-debug directory. File should content list of names and surnames
    // such like this:

    // 1. Andrey Martin    < -
    // 2. Kate Brown       < -
    // 3. John Drows       < -
    // ...

    //manager.showContentedFiles();

    return 0;
}