#include "FileOrganiser.h"

namespace FileManage{

    void FileOrganiser::stopForSec(int sec){
        std::this_thread::sleep_for(std::chrono::seconds(sec));
    }

    void FileOrganiser::run(){

        int temp;

        while(ifRun){
            update();

            std::cin>>temp;
            curr_option = static_cast<Options>(temp);

//                    "1. Create directory.",
//                    "2. Files number.",
//                    "3. Remove files.",
//                    "4. Change filenames (types).",
//                    "5. Save monit to file.",
//                    "6. Turn On/!Off file monitor.",
//                    "7. Exit."

            switch(curr_option){
                case Options::CreateDir:  // 1.
                    break;
                case Options::NumbFiles:  // 2.   ~
                    numberOfFiles();
                    break;
                case Options::DelFiles:   // 3.   ~
                    deleteAllContentedFiles();
                    break;
                case Options::Filenames:  // 4.   ~
                    fileChangesSubMenu();
//                    setFileNameIf([count = 1]() mutable {    // < ----- temporary lambda to change filename
//                        return std::to_string(count++);
//                    });
                    break;
                case Options::SaveMonit:  // 5.    // (after launch) all changes available in cmake file
                    if(fileMonitor)
                        monit.launchSaveToFile();  // TODO: add info about success
                    break;
                case Options::Monit:      // 6.
                    if(!fileMonitor){
                        runFileMonitor();
                        fileMonitor = true;
                    }else{
                        // TODO: turn off service
                    }
                    break;
                case Options::Exit:
                    if(fileMonitor){
                        fileMonitor = false;
                        monit.breakChecking();
                    }
                    ifRun = false;
                    break;
                default:
                    std::cout<<"Blad";
                    ifRun = false;
                    break;
            }

        }
    }

    void FileOrganiser::setFileNameIf(std::function<std::string()> const& pred){  // pred should returns correct name

        if(monit.isEmptyPath()){
            std::cout<<"Empty directory!\n";
            stopForSec(2);
            return;
        }

        // 1. Iterate by all files
        std::vector<filesystem::path> _paths;

        for(auto file: filesystem::directory_iterator(origin_directory)){
            //if(auto s = (pred() + file.path().extension().string()); !filesystem::exists(s))  // if not exist
            _paths.push_back(file.path());
        }

        for(const auto& f: _paths){
            filesystem::rename(f,origin_directory/(pred() + f.extension().string()));
        }
    }

    void FileOrganiser::update(){
        system("cls");
        drawMenu();
    }

    bool FileOrganiser::isEmptyDirectory() {

        // monit.isEmptyPatch() - all the time available but if user create file
        // during the program launching we don't have this file in monitor

        if (fileMonitor)
            return monit.isEmptyPath();

        std::cout << "Can I launch fileMonitor (1 - Yes | 0 - No) ? : ";
        bool state;

        std::cin >> state;

        if(state) {
            runFileMonitor();
            fileMonitor = true;
            std::cout<<"\n";
            return monit.isEmptyPath();
        }else{
            std::cout<<"\nYou don't use fileMonitor. Information may be incorrect!\n";
            return monit.isEmptyPath();
        }
    }

    void FileOrganiser::fileChangesSubMenu() {
        if(this->isEmptyDirectory()) {
            std::cout << "Empty directory!\n";
            stopForSec(3);
            return;
        }

        int temp;
        auto subUpdate([]() -> void {
            std::cout << "Change type of action: \n";
            std::cout << "1. According to file (if accesible).\n";
            std::cout << "2. Numbers sequence (1 .. 2 .. 3 ..)\n";

        });
        //while(std::cin>>temp)


    }

    void FileOrganiser::drawMenu(){
        // draw area
        // if update is accessible - do it


        auto mainMenu = menu;

        auto getMenuOption([&mainMenu]() {
            if(!mainMenu.empty()){
                auto tmp = mainMenu.front();
                mainMenu.pop_front();
                return tmp;
            }
        });


        std::string w(board.width, board.border);



        std::cout << w << "\n";

        for(unsigned int x = 0; x < board.height; ++x){
            if(x == board.sepBar)
                std::cout << w <<"\n"; // here
            else if(x == board.sepBar + 1 && notification && fileMonitor)
                std::cout << textLineCreator(*notification)<<"\n";
            else{
                for(unsigned int y = 0; y < board.width; ++y) {
                    if(x < board.menuCorner || x > board.menuCorner + menu.size() - 1){
                        if (y == 0 || y == board.width - 1) {
                            std::cout << board.border;
                        }else {
                            std::cout << " ";
                        }
                    }else{
                        std::cout << getMenuOption();
                        y=board.width;
                    }
                }
                std::cout<<"\n";
            }
        }

        std::cout << w;

        std::cout<<"\n->  YOUR CHOICE: ";

    }

    void FileOrganiser::runFileMonitor(){         // FILE MONITOR (OTHER THREAD)

        if(!notification){
            notification.emplace("START CHECKING...");
            update();
        }

        //auto& opt = notification;
        //std::function<void()> refresh = std::bind(&FileOrganiser::update, this);
        // avoid 'this' pointer in this lambda
        monitThread = std::thread(&FileChecker::startChecking,&monit,[this](const std::wstring& path_, State type_of_change){
            filesystem::path pth(path_);
            auto msg = stateToString(type_of_change) + pth.filename().string();
            notification.emplace(msg);
            update();
            std::this_thread::sleep_for(std::chrono::seconds(1)); // wait 1 seconds before showing next changes
        });

    }

    std::string FileOrganiser::textLineCreator(const std::string& msg){
        std::string line(board.width,' ');
        line[0] = line[board.width - 1] = board.border;

        std::copy(msg.begin(), msg.end(), line.begin() + board.startText);
        return line;
    }

    void FileOrganiser::createSmartMenu(){ // bool / menu depends from height and width

        std::initializer_list<std::string> init{
            "1. Create directory.",
            "2. Files number.",
            "3. Remove files.",
            "4. Change filenames (types).",
            "5. Save monit log to file.",
            "6. Turn On/!Off file monitor.",
            "7. Exit."
        };

        auto makeOption([this](std::string opt) -> std::optional<std::string>{
            if(board.startText + opt.length() > board.width)
                return std::nullopt;

            auto line = textLineCreator(opt);

//            for (int i = startText, j = 0; i < startText + opt.length(); ++i, ++j) {
//                line[i] = opt[j];
//            }
            return std::make_optional(line);
        });

        for(const auto& v: init){
            if(auto o(makeOption(v)); o){    // if it's not nullopt
                menu.push_back(o.value());
            }
        }
    }

    void FileOrganiser::numberOfFiles() {

        if(this->isEmptyDirectory()){
            std::cout<<"Empty directory!\n";
            stopForSec(2);
            return;
        }

        using namespace std::string_literals;

        auto curr_size = monit.getCurrentNumberOfFiles();

        auto num_it = menu.begin();
        std::advance(num_it, 1);

        std::string copy = *num_it;
        std::string& menu_option  = *num_it;


        const std::string counter = "DIR/FILES - "s + std::to_string(curr_size);

        int dot_idx = menu_option.find_last_of(".:");

        menu_option[dot_idx] = ':';
        std::copy(counter.begin(), counter.end(), menu_option.begin() + dot_idx + 3);

        update();

        stopForSec(3);
        *num_it = copy;
    }

    void FileOrganiser::deleteAllContentedFiles(){

        if(this->isEmptyDirectory()){
            std::cout<<"Empty directory!\n";
            stopForSec(2);
            return;
        }

        bool state;

        std::cout << "Are You sure (1 - Yes | 0 - No) ?  ";
        std::cin>>state;

        try {
            if (state) {
                for (const auto &rem: filesystem::directory_iterator(origin_directory)) {
                    filesystem::remove(rem.path());
                }
            }
        }
        catch(filesystem::filesystem_error& e){
            std::cout << e.what() << "\n";
        }
    }

    void FileOrganiser::readDataFromFile() {  // involve only if lastModification was changed

        using namespace std::string_literals;

        std::string name = (origin_directory/file_name).string();

        try{
            if(!filesystem::exists(name))
                throw "EXCEPTION: File not exist"s;

            if(!data_flow){
                data_flow.emplace(name);
            }else{
                data_flow->open(name);
            }

            std::string temp;

            if(!fileNameBuffer.empty())
                fileNameBuffer.clear();

            if(data_flow->is_open()){
                while(!data_flow->eof()){
                    std::getline(*data_flow, temp);
                    fileNameBuffer.emplace_back(temp);
                }

                for(const auto& f: fileNameBuffer)
                    std::cout<<f<<"\n";
            }

            data_flow->close();
        }
        catch(std::ifstream::failure& f){
            std::cout << f.what() << "\n";
        }
        catch(filesystem::filesystem_error& e){
            std::cout << e.what() << "\n";
        }
        catch(const std::string& s){
            std::cout << s << "\n";
        }
    }

    FileOrganiser::~FileOrganiser() {
        if(monitThread.joinable())
            monitThread.join();
    }
}