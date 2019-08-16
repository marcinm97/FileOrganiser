#include "FileOrganiser.h"

namespace FileManage{

    namespace TextValid{
        std::string createFilename(const std::string& file_name){
            auto dot = file_name.find_first_of(".");
            if(file_name[dot + 1] == ' ')
                dot += 2;
            else
                ++dot;

            std::string readyfname = file_name.substr(dot, file_name.size());

            std::replace(readyfname.begin(), readyfname.end(), ' ', '_');

            //std::transform(file_name.begin() + ++dot, file_name.end(), std::inserter(readyfname, readyfname.begin()), [](auto c){return std::tolower(c);});

            //readyfname.erase(std::remove(readyfname.begin(), readyfname.end(),' '));


            // tolower | remove space

            return readyfname;
        }
    }

    void FileOrganiser::stopForSec(int sec){
        std::this_thread::sleep_for(std::chrono::seconds(sec));
    }

    void FileOrganiser::run(){

        int temp;

        while(ifRun){
            update();

            std::cin>>temp;
            curr_option = static_cast<Options>(temp);

//                    "1. Show Files.",
//                    "2. Files number.",
//                    "3. Remove files.",
//                    "4. Change filenames (types).",
//                    "5. Save monit to file.",
//                    "6. Turn On/!Off file monitor.",
//                    "7. Exit."

            switch(curr_option){
                case Options::ShowFiles:  // 1.
                    showContainedFiles();
                    break;
                case Options::NumbFiles:  // 2.   ~
                    numberOfFiles();
                    break;
                case Options::DelFiles:   // 3.
                    deleteAllContentedFiles();
                    break;
                case Options::Filenames:  // 4.
                    fileChangesSubMenu();
                    break;
                case Options::SaveMonit:  // 5.    // (after launch) all changes available in cmake file
                    if(fileMonitor){
                        monit.launchSaveToFile();
                        std::cout<<"\nLaunch successed!\n";
                        stopForSec(2);
                    }else{
                        std::cout<<"\nfileMonitor hasn't run yet!\n";
                        stopForSec(2);
                    }
                    break;
                case Options::Monit:      // 6.
                    if(!fileMonitor){
                        runFileMonitor();
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
                    std::cout<<"INFO: Undefined action\n";
                    stopForSec(2);
                    break;
            }

        }
    }

    void FileOrganiser::showContainedFiles() {
        if(this->isEmptyDirectory()) {
            std::cout << "INFO: Empty directory!\n";
            stopForSec(3);
            return;
        }

        std::cout<<"\n";

        for(auto&& f: monit.getListOfCurrentFiles()){
            int idx = f.find_last_of('\\');
            //std::wcout<<f<<"\n";
            std::wcout << f.substr(idx++, f.size())<<"\n";
        }
        //stopForSec(15);
        std::cout<<"\n";
        system("pause");
    }

    void FileOrganiser::duplicateFile(const filesystem::path &fname, const unsigned int ncopy, std::function<std::string()> const& pred) {
        if(this->isEmptyDirectory()) {
            std::cout << "INFO: Empty directory!\n";
            stopForSec(3);
            return;
        }

        try {
            if (!filesystem::exists(fname))
                throw "EXCEPTION: File not exist";

            if(monit.getCurrentNumberOfFiles() > 1){
                bool state;

                std::cout << "Before making duplicate, you should remove other files.\n";

                std::cout << "Are You sure (1 - Yes | 0 - No) ?  ";
                std::cin >> state;

                if (state) {
                    for (const auto &rem: filesystem::directory_iterator(origin_directory)) {
                        if (rem.path() != fname)
                            filesystem::remove(rem.path());
                    }
                }
            }

            for(int i = 0; i < ncopy; ++i){
                filesystem::copy_file(fname, origin_directory/(pred() + fname.extension().string()));
            }

            filesystem::remove(fname);
        }
        catch(filesystem::filesystem_error& e){
            std::cout << e.what() << "\n";
        }
    }

    void FileOrganiser::setFileNameIf(std::function<std::string()> const& pred){  // pred should returns correct name
        if(this->isEmptyDirectory()) {
            std::cout << "INFO: Empty directory!\n";
            stopForSec(3);
            return;
        }
        // 1. Iterate by all files
        std::set<filesystem::path> _paths;   // set - ordering - protection by bad rename

        for(auto file: filesystem::directory_iterator(origin_directory)){
            _paths.insert(file.path());
        }

        try{
            for(const auto& currfile: _paths){
                auto s(origin_directory/(pred() + currfile.extension().string()));

                if(currfile != s)
                    filesystem::rename(currfile, s);
            }
        } catch(filesystem::filesystem_error& e){
            std::cout << e.what() << "\n";
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
            std::cout<<"\n";
            return monit.isEmptyPath();
        }else{
            std::cout<<"\nINFO: You don't use fileMonitor. Information may be incorrect!\n\n";
            system("pause");
            return monit.isEmptyPath();
        }
    }

    void FileOrganiser::fileChangesSubMenu() {
        if(this->isEmptyDirectory()) {
            std::cout << "INFO: Empty directory!\n";
            stopForSec(3);
            return;
        }

        bool check = true;
        enum class Action: unsigned int {FILE = 1, SEQUENCE, DUPLICATE, EXIT };
        Action type;

        auto subUpdate([&type]() -> void {
            system("cls");
            int temp;
            std::cout << "Change type of action: \n\n";
            std::cout << "1. According to file (if accesible).\n";
            std::cout << "2. Numbers sequence (1 .. 2 .. 3 ..)\n";
            std::cout << "3. Duplicate according to filename.\n";
            std::cout<<  "4. Exit.\n";
            std::cout<<"\n->  YOUR CHOICE: ";
            std::cin >> temp;

            type = static_cast<Action>(temp);
        });


        do{
            subUpdate();

            switch(type){
                case Action::FILE:

                    if(readDataFromFile()){
                        auto vect = fileNameBuffer;
                        setFileNameIf([&vect, count = 1]() mutable -> std::string {
                            std::string tmp;
                            if(!vect.empty()){
                                tmp = *vect.begin();  //vect.back();
                                vect.erase(vect.begin());
                                //vect.pop_back();
                            }else
                                tmp = "no_name" + std::to_string(count++);     // protection before name conflict

                            return tmp;});

                        std::cout<<"Successed!\n";
                        stopForSec(2);
                    }

                    std::cout<<"\n";
                    system("pause");
                    break;
                case Action::SEQUENCE:
                    setFileNameIf([count = 1]() mutable -> std::string { return std::to_string(count++); });
                    std::cout<<"Successed!\n";
                    stopForSec(2);
                    break;
                case Action::DUPLICATE:
                    system("cls");
                    if(readDataFromFile()){
                        std::string name;
                        int cpy = 1;

                        std::cout << "\nEnter copyable filename (not related with new filenames) example: filename.txt: ";
                        std::cin >> name;

                        std::cout << "\nEnter number of duplicates: ";
                        std::cin >> cpy;

                        system("cls");

                        auto vect = fileNameBuffer;
                        duplicateFile(origin_directory/name, cpy, [&vect, count = 1]() mutable -> std::string {

                            std::string tmp;
                            if(!vect.empty()){
                                tmp = *vect.begin();  //vect.back();
                                vect.erase(vect.begin());
                                //vect.pop_back();
                            }else
                                tmp = "no_name" + std::to_string(count++);     // protection before name conflict

                            return tmp;});
                    }

                    break;
                case Action::EXIT:
                    check = false;
                    break;
                default:
                    std::cout<<"INFO: Undefined action\n";
                    stopForSec(2);
                    break;
            }
        }while(check);
    }

    void FileOrganiser::drawMenu(){

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

    void FileOrganiser::displayAllContainedExtensions() {
        if(this->isEmptyDirectory()) {
            std::cout << "INFO: Empty directory!\n";
            stopForSec(3);
            return;
        }

        auto set = monit.getSetOfExtensions();
        std::cout<<"\nContented extensions:\n";
        std::copy(set.begin(), set.end(), std::ostream_iterator<std::string>(std::cout, "\n"));
        std::cout<<"\n";
        system("pause");
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

            if(curr_option != Options::Filenames)
                update();

            std::this_thread::sleep_for(std::chrono::seconds(1)); // wait 1 seconds before showing next changes
        });

        fileMonitor = true;
    }

    std::string FileOrganiser::textLineCreator(const std::string& msg){
        std::string line(board.width,' ');
        line[0] = line[board.width - 1] = board.border;

        std::copy(msg.begin(), msg.end(), line.begin() + board.startText);
        return line;
    }

    void FileOrganiser::createSmartMenu(){ // bool / menu depends from height and width

        std::initializer_list<std::string> init{
            "1. Show all files.",
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
            std::cout<<"INFO: Empty directory!\n";
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
            std::cout<<"INFO: Empty directory!\n";
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

    bool FileOrganiser::readDataFromFile() {  // involve only if lastModification was changed

        using namespace std::string_literals;

        std::string name = (filesystem::current_path()/file_name).string();    // file should be given in cmaake directory

        try{
            if(!filesystem::exists(name))
                throw "\nEXCEPTION: File not exist"s;

            if(filesystem::last_write_time(name) == lastFileMod){
                std::cout<<"\nINFO: You have the newest version of data storage!\n";
                stopForSec(2);
                return true;
            }

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
                    fileNameBuffer.insert(TextValid::createFilename(temp));
                }

                for(const auto& f: fileNameBuffer)
                    std::cout<<f<<"\n";


                std::cout<<"\n\nSuccess Processing ... \n";
                stopForSec(3);
            }
            lastFileMod = filesystem::last_write_time(name);

            data_flow->close();
        }
        catch(std::ifstream::failure& f){
            std::cout << f.what() << "\n";
            return false;
        }
        catch(filesystem::filesystem_error& e){
            std::cout << e.what() << "\n";
            return false;
        }
        catch(const std::string& s){
            std::cout << s << "\n";
            return false;
        }

        std::cout<<"Update storage of filenames!\n";
        return true;
    }

    FileOrganiser::~FileOrganiser() {
        if(monitThread.joinable())
            monitThread.join();
    }
}