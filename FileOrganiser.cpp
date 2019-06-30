#include "FileOrganiser.h"

namespace FileManage{


    void FileOrganiser::run(){

        int temp;

        while(ifRun){
            drawMenu();
            std::cin>>temp;
            curr_option = static_cast<Options>(temp);

            switch(curr_option){
                case Options::CreateDir:
                    break;
                case Options::ChangeDir:
                    break;
                case Options::DispFiles:
                    break;
                case Options::AddFiles:
                    break;
                case Options::Filenames:
                    break;
                case Options::Monit:
                    break;
                case Options::Exit:
                    ifRun = false;
                    break;
                default:
                    std::cout<<"Blad";
                    ifRun = false;
                    break;
            }
            system("cls");
        }
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
                std::cout << w <<"\n";
            else{
                for(unsigned int y = 0; y < board.width; ++y) {
                    if(x < board.menuCorner || x > board.menuCorner + menu.size() - 1){
                        if (y == 0 || y == board.width - 1) {
                            std::cout << board.border;
                        }else {
                            std::cout << " ";
                        }
                    }else{
                        std::cout << getMenuOption(); //mainMenu.front();
                        //mainMenu.pop_front();
                        y=board.width;
                    }
                }
                std::cout<<"\n";
            }
        }
        std::cout << w;

        std::cout<<"\n-> YOUR CHOICE: ";

    }


    void FileOrganiser::createSmartMenu(){ // bool / menu depends from height and width

        unsigned int startText = 4;  // index of first menu option (X axis)

        std::initializer_list<std::string> init{
            "1. Create directory.",
            "2. Change directory.",
            "3. Show files.",
            "4. Add files.",
            "5. Change filenames (types).",
            "6. Turn On/Off file monitor.",
            "7. Exit."
        };

        auto makeOption([this, startText](std::string opt) -> std::optional<std::string>{
            if(startText + opt.length() > board.width)
                return std::nullopt;

            std::string line(board.width,' ');
            line[0] = line[board.width - 1] = board.border;

            std::copy(opt.begin(), opt.end(), line.begin() + startText);

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

}