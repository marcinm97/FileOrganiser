#include <iostream>
#include "FileOrganiser.h"

int main() {

    // create directory (whether not exist)

    auto main_dir(filesystem::current_path().parent_path()/FileManage::dir_name);

    if(!filesystem::exists(main_dir)){
        filesystem::create_directory(filesystem::current_path().parent_path()/FileManage::dir_name);
    }

    FileManage::FileOrganiser manager(main_dir);

    manager.drawMenu();

    //filesystem::rename(main_dir/"marcin.jpg",main_dir/"1.jpg");

    return 0;
}