#ifndef FILEORGANISER_FILEORGANISER_H
#define FILEORGANISER_FILEORGANISER_H

#include "FileChecker.h"          // < -- own library to monit changes in files (given user-directory)
#include <experimental/filesystem>
#include <string>
#include <iostream>
#include <algorithm>

using namespace std::experimental;

// 1. Changing filenames
// 2. Show extensions
// 3. Show current state of files (real time)
// 4.
namespace FileManage{


    class FileOrganiser{
    public:
        FileOrganiser(const std::string& main_dir): origin_directory(main_dir), monit(main_dir, std::chrono::milliseconds(5000)){
            if(!filesystem::exists(origin_directory))
                filesystem::create_directory(origin_directory);
        }

        template<typename T>
        void setFileNameIf(std::function<T()> const& pred); // or new menu to change options ...
        void changeSingleFileName();
        void changeDirectory();
        void addFiles();        // only message "Add your files to given directory ... "
        void createDirectory(); // create and change for this dir
        void displayAllContainedExtensions();
        void startManager();    // main user-output
    protected:
        void drawMenu();        // method use to draw all menu elements
    private:
        std::string origin_directory;
        FileChecker monit;      // only for notify about newest changes (alert border)
    };
}




//void testOrganiser(const std::string& file_name){
////    auto w = filesystem::recursive_directory_iterator(file_name);
////    w++;
////
////    std::cout<<w->path();
//
//    for(const auto& file: filesystem::directory_iterator(file_name,filesystem::directory_options::skip_permission_denied)){
//        std::cout<<file.path()<<"\n";
//        if(filesystem::is_directory(file.path())){
//            for(const auto& sub: filesystem::directory_iterator(file,filesystem::directory_options::skip_permission_denied))
//                std::cout<<"   "<<sub.path()<<"\n";
//            }
//    }
//}




#endif //FILEORGANISER_FILEORGANISER_H
