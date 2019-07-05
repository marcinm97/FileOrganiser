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

            switch(curr_option){
                case Options::CreateDir:  // 1.
                    break;
                case Options::ChangeDir:  // 2.
                    break;
                case Options::NumbFiles:  // 3.
                    numberOfFiles();
                    break;
                case Options::DelFiles:  // 4.
                    deleteAllContentedFiles();
                    break;
                case Options::Filenames:  // 5.  // add vector with functionals or map
                    setFileNameIf([count = 1]() mutable {    // < ----- temporary lambda to change filename
                       return std::to_string(count++);
                    });
                    break;
                case Options::Monit:      // 6.
                    if(!fileMonitor){
                        runFileMonitor();
                        fileMonitor = true;
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
        monitThread = std::thread(&FileChecker::startChecking,&monit,[this](const std::string& path_, State type_of_change){
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
            "2. Change directory.",
            "3. Files number.",
            "4. Remove files.",
            "5. Change filenames (types).",
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

    void FileOrganiser::numberOfFiles() { //return size from map FileChecker(lib) / add method
        // find to search option in menu
        // for to check how mant files are there
        using namespace std::string_literals;

        auto curr_size = monit.getCurrentNumberOfFiles();

        auto num_it = menu.begin();
        std::advance(num_it, 2);

        std::string& menu_option  = *num_it;

        const std::string counter = "DIR/FILES -> "s + std::to_string(curr_size);

        int dot_idx = menu_option.find_last_of(".:");

        menu_option[dot_idx] = ':';

        std::copy(counter.begin(), counter.end(), menu_option.begin() + dot_idx + 3);
    }

    void FileOrganiser::deleteAllContentedFiles(){
        if(monit.isEmptyPath()){
            std::cout<<"Empty directory!\n";
            stopForSec(2);
            return;
        }

        bool state;

        std::cout << "Are You sure (1 - Yes | 0 - No) ? ";
        std::cin>>state;

        if(state)
            filesystem::remove_all(origin_directory);
    }

    FileOrganiser::~FileOrganiser() {
        if(monitThread.joinable())
            monitThread.join();
    }
}