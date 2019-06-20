#include "FileOrganiser.h"

namespace FileManage{

    void FileOrganiser::drawMenu(){
        // draw area
        std::string w(board.width, board.border);

        std::cout << w << "\n";

        for(unsigned int x = 0; x < board.height; ++x){
            if(x == board.sepBar)
                std::cout << w<<"\n";
            else{
                for(unsigned int y = 0; y < board.width; ++y) {

                    if (y == 0 || y == board.width - 1) {
                        std::cout << board.border;
                    }else {
                        std::cout << " ";
                    }
                }
                std::cout<<"\n";
            }
        }
        std::cout << w;
    }


}