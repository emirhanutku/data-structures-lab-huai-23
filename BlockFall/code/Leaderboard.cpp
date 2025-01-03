#include "Leaderboard.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <vector>

void Leaderboard::insert_new_entry(LeaderboardEntry* new_entry) {
    int index{0};

    if (head_leaderboard_entry== nullptr){
        head_leaderboard_entry=new_entry;

    }
    else{
        LeaderboardEntry* afterEntry=head_leaderboard_entry;
        LeaderboardEntry* beforeEntry{nullptr};

        while (afterEntry!= nullptr){

            if (new_entry->score>afterEntry->score){
                break;
            }
            else if (new_entry->score==afterEntry->score&&new_entry->last_played<afterEntry->last_played){
                break;
            }
            index++;
            beforeEntry=afterEntry;
            afterEntry=afterEntry->next_leaderboard_entry;
        }
        if (beforeEntry== nullptr){
            head_leaderboard_entry=new_entry;
            new_entry->next_leaderboard_entry=afterEntry;
        }
        else{
            beforeEntry->next_leaderboard_entry=new_entry;
            new_entry->next_leaderboard_entry=afterEntry;
        }
        while (new_entry!= nullptr){
            if (index==MAX_LEADERBOARD_SIZE-1){
                delete new_entry->next_leaderboard_entry;
                new_entry->next_leaderboard_entry= nullptr;
                break;
            }
            if (index==MAX_LEADERBOARD_SIZE){
                delete beforeEntry->next_leaderboard_entry;
                beforeEntry->next_leaderboard_entry= nullptr;
                break;
            }
            new_entry=new_entry->next_leaderboard_entry;
            index++;

        }
    }

}

void Leaderboard::write_to_file(const string& filename) {

    ofstream file(filename);

    if (file.is_open() && head_leaderboard_entry != nullptr) {
        LeaderboardEntry *leaderboardEntry = head_leaderboard_entry;


        while (leaderboardEntry != nullptr) {
            file << leaderboardEntry->score << " " << leaderboardEntry->last_played << " " << leaderboardEntry->player_name << "\n";
            leaderboardEntry = leaderboardEntry->next_leaderboard_entry;
        }
    }
}

void Leaderboard::read_from_file(const string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return;
    }
    std::string line;

    int i{0};
    LeaderboardEntry* leaderboardEntry=head_leaderboard_entry;
    while (std::getline(file, line)&&i<MAX_LEADERBOARD_SIZE) {


        std::istringstream stream(line);
        std::vector<std::string> infos;

        std::string info;
        while (stream >> info) {
            infos.push_back(info);
        }
        auto* entry=new LeaderboardEntry(stoul(infos[0]), stol(infos[1]), infos[2]);
        if (head_leaderboard_entry== nullptr){
            head_leaderboard_entry=entry;
            leaderboardEntry=entry;
        }
        else{
            leaderboardEntry->next_leaderboard_entry=entry;
            leaderboardEntry=leaderboardEntry->next_leaderboard_entry;
        }
        ++i;
    }
}



void Leaderboard::print_leaderboard() {
    std::cout<<"Leaderboard\n"
               "-----------\n";
    int i{1};
    LeaderboardEntry* leaderboardEntry=head_leaderboard_entry;
    while (leaderboardEntry!= nullptr){
        std::cout<<i<<". "<<leaderboardEntry->player_name<<" "<<leaderboardEntry->score<<" "<<std::put_time(std::localtime(&leaderboardEntry->last_played), "%H:%M:%S/%d.%m.%Y") << std::endl;
        leaderboardEntry=leaderboardEntry->next_leaderboard_entry;
        i++;
    }


}

Leaderboard::~Leaderboard() {
   LeaderboardEntry* currentLeaderboard=head_leaderboard_entry;
    while (currentLeaderboard!= nullptr){
        LeaderboardEntry* nextLeaderboard=currentLeaderboard->next_leaderboard_entry;
        delete currentLeaderboard;
        currentLeaderboard=nextLeaderboard;
    }
}


