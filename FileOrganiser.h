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
#include <fstream>

using namespace std::experimental;

namespace FileManage{

    static constexpr const char* dir_name  = "MonitFiles";
    static constexpr const char* file_name = "filenamesList.txt";

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

    namespace TextValid{
        std::string createFilename(const std::string& file_name);
    }

    namespace fs = filesystem;

    class FileOrganiser{
    public:
        FileOrganiser(const fs::path& main_dir): origin_directory(main_dir), notification(std::nullopt),
            monit(main_dir.string(), std::chrono::milliseconds(5000)), data_flow(std::nullopt), board{80,14} {
            createSmartMenu();

        }

        // TODO: ^ more options with predicate (maybe vector with functional?)
        // TODO: action to break thread with fileMonitor

        void run();             // MAIN METHOD TO LAUNCH ALL APP
        bool isEmptyDirectory();
        void setFileNameIf(std::function<std::string()> const& pred); // or new menu to change options ...
        void displayAllContainedExtensions();
        void numberOfFiles();
        void deleteAllContentedFiles();
        void showContainedFiles();
        ~FileOrganiser();
    protected:
        void runFileMonitor();  // new thread + notify to change string with message
        bool readDataFromFile();
        void fileChangesSubMenu();
        void drawMenu();        // method use to draw all menu elements
        void update();
    private:
        void stopForSec(int sec);
        void createSmartMenu();
        std::string textLineCreator(const std::string& msg);


        enum class Options{
            ShowFiles = 1,
            NumbFiles,
            DelFiles,
            Filenames,
            SaveMonit,
            Monit,
            Exit
        };

        bool                         fileMonitor = false;
        bool                         ifRun = true;
        Options                      curr_option;
        filesystem::file_time_type   lastFileMod;
        std::optional<std::string>   notification;
        std::optional<std::ifstream> data_flow;
        std::list<std::string>       menu;
        std::vector<std::string>     fileNameBuffer;
        filesystem::path             origin_directory;
        FileChecker                  monit;      // only for notify about newest changes (alert border)
        std::thread                  monitThread; // thread used to monit changes with FileChecker(launch only when fileMonitor is true)
        Window::Display              board;
    };
}


//void testOrganiser(const std::string& file_name){
//    for(const auto& file: filesystem::directory_iterator(file_name,filesystem::directory_options::skip_permission_denied)){
//        std::cout<<file.path()<<"\n";
//        if(filesystem::is_directory(file.path())){
//            for(const auto& sub: filesystem::directory_iterator(file,filesystem::directory_options::skip_permission_denied))
//                std::cout<<"   "<<sub.path()<<"\n";
//            }
//    }
//}

#endif //FILEORGANISER_FILEORGANISER_H
