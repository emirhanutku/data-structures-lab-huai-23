#include "GameController.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <iomanip>

bool GameController::play(BlockFall& game, const string& commands_file){

    game.active_rotation=game.initial_block;
    if (game.leaderboard.head_leaderboard_entry== nullptr){
        highScore=0;
    }
    else{
        highScore=(int)game.leaderboard.head_leaderboard_entry->score;
    }


    std::ifstream file(commands_file);
    if (!file.is_open()) {
        std::cerr << "Cannot open file: " << commands_file << std::endl;

    }

    std::string line;
    while (std::getline(file, line)) {
        removeR(line);
        if (line=="PRINT_GRID"){
            highScore = (highScore > game.current_score) ? highScore : (int)game.current_score;
            std::cout<<"Score: "<<game.current_score<<std::endl;
            std::cout<<"High Score: "<<highScore<<std::endl;
            printGrid(game);
        }
        else if (line=="ROTATE_RIGHT"){
            rightRotation(game);

        }
        else if (line=="ROTATE_LEFT"){
            leftRotation(game);

        }
        else if (line=="MOVE_RIGHT"){
            rightMove(game);

        }
        else if (line=="MOVE_LEFT"){
            leftMove(game);

        }
        else if (line=="DROP"){
            if (!game.gravity_mode_on){
                drop(game);
            }
            else{
                dropGravity(game);
            }



            if (game.active_rotation->next_block!= nullptr){
                //game.initial_block=game.initial_block->next_block;
                game.active_rotation=game.active_rotation->next_block;
                newBlockColumn=0;
                newBlockRow=0;
            }
            else{
                highScore = (highScore > game.current_score) ? highScore : (int)game.current_score;
                std::cout<<"YOU WIN!\n"
                           "No more blocks.\n"
                           "Final grid and score:\n"<<std::endl<<"Score: "<<game.current_score<<std::endl<<
                         "High Score: "<<highScore<<"\n";
                onlyPrintGrid(game);
                newBlockColumn=0;
                newBlockRow=0;
                insertToLeaderBoardAndFinishGame(game);

                return true;
            }



            if (!isGameStart(game)){
                highScore = (highScore > game.current_score) ? highScore : (int)game.current_score;
                std::cout<<"GAME OVER!\n"
                           "Next block that couldn't fit:\n";
                for (const auto& row : game.active_rotation->shape) {
                    for (bool value : row) {
                        std::cout << (value == 1 ? occupiedCellChar : unoccupiedCellChar);
                    }
                    std::cout << std::endl;
                }
                std::cout << std::endl<<"Final grid and score:\n"
                                        "\n"
                                        "Score: "<<game.current_score<<std::endl<<
                                        "High Score: "<<highScore<<"\n";
                onlyPrintGrid(game);
                newBlockColumn=0;
                newBlockRow=0;
                insertToLeaderBoardAndFinishGame(game);
                return false;
            }





        }
        else if (line=="GRAVITY_SWITCH"){
            game.gravity_mode_on=!game.gravity_mode_on;
            if (game.gravity_mode_on){
                dropOldBlock(game);
            }


        }
        else{
            std::cout << "Unknown command: " << line << std::endl;
            continue;
        }

    }
    highScore = (highScore > game.current_score) ? highScore : (int)game.current_score;
    std::cout<<"GAME FINISHED!\n"
               "No more commands.\n"
               "Final grid and score:\n\n";
    std::cout<<"Score: "<<game.current_score<<"\n";
    std::cout<<"High Score: "<<highScore<<"\n";
    cleanOldBlock(game);
    onlyPrintGrid(game);
    newBlockColumn=0;
    newBlockRow=0;
    insertToLeaderBoardAndFinishGame(game);

    return true;
}

void GameController::insertToLeaderBoardAndFinishGame(BlockFall& game){
    auto* newEntry=new LeaderboardEntry(game.current_score,time(nullptr),game.player_name);
    game.leaderboard.insert_new_entry(newEntry);
    game.leaderboard.print_leaderboard();
    game.leaderboard.write_to_file(game.leaderboard_file_name);

}

void GameController::rowControl(BlockFall& game,bool control){
    bool allOnes = std::any_of(game.grid.begin(), game.grid.end(), [](const std::vector<int>& vec) {
        return std::all_of(vec.begin(), vec.end(), [](int value) {
            return value == 1;
        });
    });
    if (allOnes&&control){
        std::cout<<"Before clearing:"<<std::endl;
        onlyPrintGrid(game);

    }



    for (size_t i = 0; i < game.grid.size(); ++i) {
        if (std::all_of(game.grid[i].begin(), game.grid[i].end(), [](int value) { return value == 1; })) {
            game.current_score+=game.cols;
            std::for_each(game.grid[i].begin(), game.grid[i].end(), [](int& value) {
                value = 0;
            });

            for (int j = 0; j < i; ++j) {
                for (int k = 0; k < game.cols; ++k) {
                    if (!control){
                        cleanOldBlock(game);
                        game.grid[i-j][k]=game.grid[i-j-1][k];
                        game.grid[i-j-1][k]=0;
                        placeBlock(game);
                    }
                    else{
                        game.grid[i-j][k]=game.grid[i-j-1][k];
                        game.grid[i-j-1][k]=0;
                    }


                }
            }
        }
    }

}


bool GameController:: isPowerUpExist(BlockFall& game){
    for (int i = 0; i <= game.rows - game.power_up.size(); i++) {
        for (int j = 0; j <= game.cols - game.power_up[0].size(); j++) {
            int isMatch{0};
            for (int k = 0; k < game.power_up.size(); k++) {
                for (int l = 0; l < game.power_up[0].size(); l++) {
                    if (game.grid[i + k][j + l] == game.power_up[k][l])
                        isMatch++;
                }
            }
            if (isMatch == game.power_up.size() * game.power_up[0].size()){
                return true;
            }
        }
    }
    return false;
}


void GameController::dropOldBlock(BlockFall& game){
    for (int i = 0; i <game.cols ; ++i) {
        for (int j = game.rows-1; j >=0 ; --j) {
            bool blockController{true};


             for (int k = 0; k < game.active_rotation->shape.size(); ++k) {
                 for (int l = 0; l < game.active_rotation->shape[0].size(); ++l) {
                     if (game.grid[j][i]==1&&game.active_rotation->shape[k][l]==1&&k==j&&l+newBlockColumn==i){
                         blockController= false;
                     }

                 }
             }

            if (blockController){
                if (game.grid[j][i]==1){
                    bool  controller{true};
                    for (int m = j+1; m <game.rows; ++m) {
                        if (game.grid[m][i]==1){
                            controller= false;
                            game.grid[j][i]=0;
                            game.grid[m-1][i]=1;
                            break;
                        }


                    }
                    if (controller){
                        game.grid[j][i]=0;
                        game.grid[game.grid.size()-1][i]=1;
                    }
                }
            }

        }
    }
    rowControl(game, false);

}


void GameController::dropGravity(BlockFall& game){
    dropGravityDistance(game);

    if (isPowerUpExist(game)){
        game.current_score += 1000+ countBlockNumberOfGrid(game);
        std::cout << "Before clearing:\n";

        onlyPrintGrid(game);

        vector<vector<int>> grid(game.rows, vector<int>(game.cols, 0));
        game.grid = grid;
    }
    rowControl(game, true);


}


void GameController::dropGravityDistance(BlockFall &game) {
    int minValue{0};
    for (int i = 0; i <game.active_rotation->shape[0].size() ; ++i) {
        for (int j =(int) game.active_rotation->shape.size()-1; j >=0 ; --j) {
            if (game.active_rotation->shape[j][i]==1){
                bool  controller{true};
                for (int k = j+1; k <game.grid.size() ; ++k) {
                    if (game.grid[k][i+newBlockColumn]==1){
                        if (minValue==0){minValue=k-1-j;}
                        else{minValue = (minValue < k-1-j) ? minValue : k-1-j;}

                        controller= false;
                        game.grid[k-1][i+newBlockColumn]=1;
                        game.grid[j][i+newBlockColumn]=0;
                        break;
                    }


                }
                if (controller){
                    if (minValue==0){minValue=(int)game.grid.size()-1-j;}
                    else{minValue = (minValue < game.grid.size()-1-j) ? minValue : game.grid.size()-1-j;}
                    game.grid[game.grid.size()-1][i+newBlockColumn]=1;
                    game.grid[j][i+newBlockColumn]=0;
                }






            }
        }
    }
    game.current_score+=minValue* countBlockNumber(game);
}


void GameController::drop(BlockFall& game){

    cleanOldBlock(game);

    newBlockRow= dropDistance(game)-game.active_rotation->shape.size();


    game.current_score=game.current_score+newBlockRow* countBlockNumber(game);
    placeBlock(game);


    if (isPowerUpExist(game)){
        game.current_score += 1000+ countBlockNumberOfGrid(game);

        std::cout << "Before clearing:\n";
        printGrid(game);
        vector<vector<int>> grid(game.rows, vector<int>(game.cols, 0));
        game.grid = grid;
    }
    rowControl(game, true);


}


int GameController::dropDistance(BlockFall& game){
    int minValue{0};

    for (int p = 0; p < game.active_rotation->shape.size(); ++p) {
        for (int l = 0; l < game.active_rotation->shape[0].size(); ++l) {
            if (game.active_rotation->shape[p][l]==0&&game.grid[p][l+newBlockColumn]==1){
                return game.active_rotation->shape.size();
            }
        }

    }


    for (int i = 0; i < game.active_rotation->shape[0].size(); ++i) {
        int startPoint{0};

        for (int k = game.active_rotation->shape.size()-1; k >=0 ; --k) {
            if (game.active_rotation->shape[k][i]==1){
                break;
            }
            startPoint+=1;
        }
        for (int j = game.active_rotation->shape.size() ; j < game.grid.size(); ++j) {

            if (game.grid[j][i+newBlockColumn]==1){


                j+=startPoint;


                if (minValue==0){minValue = (minValue > j) ? minValue : j;}

                else{minValue = (minValue < j) ? minValue : j;}
                break;
            }
        }
    }
    if (minValue==0){minValue=game.grid.size();
    }

    return minValue;
}


void GameController::leftMove(BlockFall& game){
    if (canLeftMove(game)){
        cleanOldBlock(game);
        newBlockColumn-=1;
        placeBlock(game);
    }
}


bool GameController::canLeftMove(BlockFall& game){
    if (newBlockColumn-1<0){
        return false;
    }
    for (int blockRow = 0; blockRow <game.active_rotation->shape.size(); ++blockRow) {
        int j =0;
        while (j<game.active_rotation->shape[blockRow].size() && game.active_rotation->shape[blockRow][j]==0){
            ++j;
        }
        if (game.grid[blockRow][newBlockColumn+j-1]==1){
            return false;
        }
    }
    return true;


}


void GameController::rightMove(BlockFall& game ){
    if (canRightMove(game)){
        cleanOldBlock(game);
        newBlockColumn+=1;
        placeBlock(game);

    }

}


bool GameController::canRightMove(BlockFall& game){
    if (newBlockColumn+game.active_rotation->shape[0].size()+1>game.grid[0].size()){
        return false;
    }
    for (int blockRow = 0; blockRow <game.active_rotation->shape.size(); ++blockRow) {
        int j =game.active_rotation->shape[0].size()-1;
        while (j>=0 && game.active_rotation->shape[blockRow][j]==0){
            --j;
        }
        if (game.grid[blockRow][newBlockColumn+j+1]==1){
            return false;
        }
    }
    return true;
}


void GameController::printGrid( BlockFall& game){
    if (isGameStart(game)){
        placeBlock(game);
        }
    else{
    }
    for (int i = 0; i <game.grid.size(); ++i) {
        for (int j = 0; j < game.grid[i].size(); ++j) {
            std::cout << (game.grid[i][j] == 1 ? occupiedCellChar : unoccupiedCellChar);
        }
        std::cout<<std::endl;
    }
    std::cout<<std::endl<<std::endl;
}


void GameController::rightRotation(BlockFall &game){
    cleanOldBlock(game);
    if (canRightRotate(game)){
        game.active_rotation=game.active_rotation->right_rotation;
        placeBlock(game);
    } else{
        placeBlock(game);
    }
}


void GameController::leftRotation(BlockFall &game){
    cleanOldBlock(game);
    if (canLeftRotate(game)){
        game.active_rotation=game.active_rotation->left_rotation;
        placeBlock(game);
    } else{
        placeBlock(game);
    }
}


bool GameController::canRightRotate(BlockFall &game){
    if (newBlockColumn+game.active_rotation->right_rotation->shape[0].size()>game.grid[0].size()){
        return false;
    }
    for (int blockRow = 0; blockRow <game.active_rotation->right_rotation->shape.size(); ++blockRow) {
        for (int blockCol = 0; blockCol < game.active_rotation->right_rotation->shape[blockRow].size(); ++blockCol) {
            if (game.active_rotation->right_rotation->shape[blockRow][blockCol]){
                if (game.grid[blockRow][blockCol+newBlockColumn]){
                    return false;
                }
            }
        }
    }
    return true;
}


bool GameController::canLeftRotate(BlockFall &game){
    if (newBlockColumn+game.active_rotation->left_rotation->shape[0].size()>game.grid[0].size()){
        return false;
    }
    for (int blockRow = 0; blockRow <game.active_rotation->left_rotation->shape.size(); ++blockRow) {
        for (int blockCol = 0; blockCol < game.active_rotation->left_rotation->shape[blockRow].size(); ++blockCol) {
            if (game.active_rotation->left_rotation->shape[blockRow][blockCol]){
                if (game.grid[blockRow][blockCol+newBlockColumn]){
                    return false;
                }
            }
        }
    }
    return true;
}


void GameController::cleanOldBlock(BlockFall &game){
    for (int blockRow = 0; blockRow <game.active_rotation->shape.size(); ++blockRow) {
        for (int blockCol = 0; blockCol < game.active_rotation->shape[0].size(); ++blockCol) {
            if (game.active_rotation->shape[blockRow][blockCol]){
                game.grid[blockRow][blockCol+newBlockColumn]=0;
            }
        }
    }
}


void GameController::placeBlock(BlockFall &game){

    for (int blockRow = 0; blockRow <game.active_rotation->shape.size(); ++blockRow) {
        for (int blockCol = 0; blockCol < game.active_rotation->shape[0].size(); ++blockCol) {
            if (game.active_rotation->shape[blockRow][blockCol]){
                game.grid[blockRow+newBlockRow][blockCol+newBlockColumn]=game.active_rotation->shape[blockRow][blockCol];
            }
        }
    }
}


bool GameController::isGameStart(BlockFall& game){
    for (int blockRow = 0; blockRow <game.active_rotation->shape.size(); ++blockRow) {
        for (int blockCol = 0; blockCol < game.active_rotation->shape[blockRow].size(); ++blockCol) {
            if (game.grid[blockRow][blockCol]==1&&game.active_rotation->shape[blockRow][blockCol]){
                //GameOver
                return false;
            }
        }
    }
    return true;
}


void GameController::removeR(string &line) {
    size_t position = line.find('\r');
    if (position != std::string::npos) {
        line.erase(position, 1);
    }
}


int GameController::countBlockNumber(BlockFall& game){
    int blockNum = 0;
    for (const auto& boolVector : game.active_rotation->shape) {
        blockNum += std::count(boolVector.begin(), boolVector.end(), true);
    }
    return blockNum;
}


int GameController::countBlockNumberOfGrid(BlockFall& game){
    int blockNumOfGrid = 0;
    for (const auto& boolVector : game.grid) {
        blockNumOfGrid += std::count(boolVector.begin(), boolVector.end(), true);
    }
    return blockNumOfGrid;
}

void GameController::onlyPrintGrid(BlockFall& game){
    for (int i = 0; i <game.grid.size(); ++i) {
        for (int j = 0; j < game.grid[i].size(); ++j) {
            std::cout << (game.grid[i][j] == 1 ? occupiedCellChar : unoccupiedCellChar);
        }
        std::cout<<std::endl;
    }
    std::cout<<std::endl<<std::endl;
}