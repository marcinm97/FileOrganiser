#include "FileOrganiser.h"

namespace FileManage{

    void FileOrganiser::drawMenu(){
        // draw area
        createSmartMenu();
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
                    if(x < board.menuCorner || x > board.height - board.menuCorner - menu.size() - 3){
                        if (y == 0 || y == board.width - 1) {   // ++ else if (MENU)
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
    }


    void FileOrganiser::createSmartMenu(){ // bool / menu depends from height and width

        unsigned int startText = 4;  // index of first menu option (X axis)

        std::initializer_list<std::string> init{
            "1. Create directory.",
            "2. Change directory.",
            "3. Show files.",
            "4. Add files.",
            "5. Change filenames (types).",
            "6. Exit."
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