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

    // TEST

//    filesystem::file_time_type time_type;


//    while(true){
//        if(filesystem::last_write_time(main_dir/"test.txt") == time_type){
//            std::cout<<"Rowna!\n";
//        }else {
//            std::cout << "Rozne!\n";
//            time_type = filesystem::last_write_time(main_dir/"test.txt");
//        }
//        std::this_thread::sleep_for(std::chrono::seconds(2));
//    }

//    manager.readDataFromFile();

    return 0;
}