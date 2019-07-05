#ifndef FILECHECKER_FILECHECKER_H
#define FILECHECKER_FILECHECKER_H

#include <experimental/filesystem>
#include <thread>
#include <unordered_map>
#include <chrono>
#include <string>
#include <functional>
#include <fstream>
#include <optional>
#include <set>
#include <algorithm>
#include <iterator>
#include <iostream>

enum class State {Created, Modified, Removed};
using namespace std::experimental;

inline std::string stateToString(State type){
    using namespace std::string_literals;
    switch(type){
        case State::Created:
            return "File created/added: "s;
        case State::Modified:
            return "File modified: "s;
        case State::Removed:
            return "File removed: "s;
        default:
            return "Unknown file status.\n"s;
    }
}

class FileChecker{
private:
    std::unordered_map<std::string, filesystem::file_time_type> paths_;
    std::chrono::duration<int, std::milli> delay_;
    std::string main_path;
    std::optional<std::ofstream> alert_file;
    std::string fileToSave;
    bool run = true;
    bool ifSave;

    bool contains(const std::string& file_name){
        // return std::find_if(paths_.begin(), paths_.end(), [file_name](const auto& pair){
        // return pair.first == file_name;}) != paths_.end();
        return paths_.find(file_name) != paths_.end();
    }

    void saveToFile(const std::string& file_name, const std::tuple<filesystem::path, time_t, std::string>& info){

        try{
            if(!alert_file) { // if alert_file is std::nullopt
                alert_file.emplace(file_name, std::ios::app);    // default is open
            }else
                alert_file->open(file_name, std::ios::app);

            if (alert_file->is_open()) {
                *alert_file <<" +  " << std::get<2>(info) << "\t" << std::get<0>(info) << "\t"
                        << std::asctime(std::localtime(&std::get<1>(info))) <<"\n";
            }

            alert_file->close();
        }
        catch(std::ostream::failure& e){
            std::cout << e.what();
            alert_file->close();
            alert_file.reset();

        }
    }
public:
    using time = std::chrono::system_clock;
    using extens = std::set<std::string>;

    FileChecker(const std::string& file_path, std::chrono::duration<int, std::milli> delay, bool saveToFile = false): main_path(file_path),
        delay_(delay), alert_file(std::nullopt), ifSave(saveToFile){

        fileToSave = ifSave ? "changes.txt" : "";

        for(auto& f: filesystem::recursive_directory_iterator(main_path))
            paths_[f.path().string()] = filesystem::last_write_time(f);     // add path as a key, and time data as a value
    }

    unsigned int getCurrentNumberOfFiles(){
        return paths_.size();
    }

    bool isEmptyPath(){
        if(!paths_.empty())
            return false;

        return true;
    }
    void startChecking(const std::function<void(const std::string&, State)>& validate){

        while(run){                                                 // infinity loop
            std::this_thread::sleep_for(delay_);                     // set refresh every "delay_" seconds

            for(auto it = paths_.begin(); it != paths_.end();){      // checking if one of the files was erased
                auto [file, mod_data] = *it;
                // TODO: if run is true
                if(!filesystem::exists(file)){
                    validate(file, State::Removed);
                    if(ifSave)
                        saveToFile(fileToSave, std::make_tuple(file, time::to_time_t(mod_data), stateToString(State::Removed)));

                    paths_.erase(it++);
                } else it++;
            }

            for(auto& file: filesystem::recursive_directory_iterator(main_path)){
                auto lastWriteTime(filesystem::last_write_time(file));

                if(auto s(file.path().string()); !contains(s)){     // check if file was created
                    paths_.insert({s,lastWriteTime});

                    if(ifSave)
                        saveToFile(fileToSave, std::make_tuple(file.path().string(),time::to_time_t(lastWriteTime), stateToString(State::Created)));

                    validate(s,State::Created);
                }else{                                              // check if file was modificated
                    if(paths_[s] != lastWriteTime){
                        paths_[s] = lastWriteTime;

                        if(auto p(filesystem::current_path()/fileToSave); ifSave && s != p.string())
                            saveToFile(fileToSave, std::make_tuple(file.path().string(),time::to_time_t(lastWriteTime), stateToString(State::Modified)));

                        validate(s,State::Modified);
                    }
                }
            }
        }
    }

    void breakChecking(){
        if(run)
            run = false;   // break success
    }

    extens getSetOfExtensions(){                 // return current available extensions
        std::set<std::string> ext;

        for(const auto& file: filesystem::directory_iterator(main_path)){
            if(!filesystem::is_directory(file))
                ext.insert(file.path().extension().string());
        }
        return ext;
    }


};

#endif //FILECHECKER_FILECHECKER_H
