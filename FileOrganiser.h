#ifndef FILEORGANISER_FILEORGANISER_H
#define FILEORGANISER_FILEORGANISER_H

#include "FileChecker.h"          // < -- own library to monit changes in files (given user-directory)
#include <experimental/filesystem>
#include <string>
#include <iostream>
#include <algorithm>
#include <list>
#include <optional>
#include <thread>
#include <functional>

using namespace std::experimental;

// 1. Changing filenames
// 2. Show extensions
// 3. Show current state of files (real time)
// 4. ...
namespace FileManage{

    namespace Window{

        struct Display{
            int width;
            int height;
            int sepBar = height <= 10 ? 0.9* height - 1 : 0.9 * height;
            char border = '#';
            int menuCorner = 3;
            unsigned int startText = 4;
        };

    }


    namespace fs = filesystem;

    static constexpr const char* dir_name = "MonitFiles";

    class FileOrganiser{
    public:

        FileOrganiser(const fs::path& main_dir): origin_directory(main_dir), notification(std::nullopt),
            monit(main_dir.string(), std::chrono::milliseconds(5000)), board{80,14} {
            createSmartMenu();

        }
        void run();

        void setFileNameIf(std::function<std::string()> const& pred); // or new menu to change options ...
        void changeSingleFileName();
        void numberOfFiles();
        void addFiles();        // only message "Add your files to given directory ... "
        void createNewDirectory(); // create and change for this dir
        void displayAllContainedExtensions();
        void runFileMonitor();  // new thread + notify to change string with message
        ~FileOrganiser();
    protected:
        void drawMenu();        // method use to draw all menu elements   1.
        void update();
    private:
        void createSmartMenu();
        std::string textLineCreator(const std::string& msg);
        void showMessage();

        enum class Options{
            CreateDir = 1,
            ChangeDir,
            DispFiles,
            AddFiles,
            Filenames,
            Monit,
            Exit
        };

        bool                       fileMonitor = false;
        bool                       ifRun = true;
        Options                    curr_option;
        std::optional<std::string> notification;
        std::list<std::string>     menu;
        filesystem::path           origin_directory;
        FileChecker                monit;      // only for notify about newest changes (alert border)
        std::thread                monitThread; // thread used to monit changes with FileChecker(launch only when fileMonitor is true)
        Window::Display            board;
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
