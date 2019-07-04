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


    //filesystem::space_info info = filesystem::space(main_dir);



    //filesystem::rename(main_dir/"marcin.txt", main_dir/"1.txt");

    return 0;
}