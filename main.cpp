#include "FileOrganiser.h"



int main() {

    // create directory (whether not exist)

    auto main_dir(filesystem::current_path().parent_path()/FileManage::dir_name);

    if(!filesystem::exists(main_dir)){
        filesystem::create_directory(filesystem::current_path().parent_path()/FileManage::dir_name);
    }

    FileManage::FileOrganiser manager(main_dir);

    manager.drawMenu();

//    enum class Menu{AddFile = 1, ChangeName, Exit};
//
//    unsigned int val = 2;
//    Menu one = Menu::ChangeName;
//    switch(one = Menu(val),one){
//        case Menu::AddFile:
//            std::cout<<"\nAddFile";
//            break;
//        case Menu::ChangeName:
//            std::cout<<"\nChangeName";
//            break;
//        case Menu::Exit:
//            std::cout<<"Exit";
//            break;
//        default:
//            std::cout<<"Default";
//    }




    //filesystem::rename(main_dir/"marcin.jpg",main_dir/"1.jpg");

    return 0;
}