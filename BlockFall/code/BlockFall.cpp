#include "BlockFall.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iostream>

BlockFall::BlockFall(string grid_file_name, string blocks_file_name, bool gravity_mode_on, const string &leaderboard_file_name, const string &player_name) : gravity_mode_on(
        gravity_mode_on), leaderboard_file_name(leaderboard_file_name), player_name(player_name) {
    initialize_grid(grid_file_name);
    read_blocks(blocks_file_name);
    leaderboard.read_from_file(leaderboard_file_name);



}

void BlockFall::read_blocks(const string &input_file) {
    std::ifstream file(input_file);
    if (!file.is_open()) {
        std::cerr << "Cannot open file: " << input_file << std::endl;
        return;
    }
    std::string line;
    vector<std::string> blockVector;
    vector<vector<std::string>> shapeVector;

    while (std::getline(file, line)) {
        removeR(line);

        if (!line.empty()){


            if (line[0] == '[' && line[line.size()-1]==']') {
                blockVector.push_back(line.substr(1,line.size()-2));
                shapeVector.push_back(blockVector);
                blockVector.clear();
            }
            else{
                if (line[0]=='['){
                    blockVector.push_back(line.substr(1,line.size()-1));
                }
                else if (line[line.size()-1]==']'){
                    blockVector.push_back(line.substr(0,line.size()-1));
                    shapeVector.push_back(blockVector);
                    blockVector.clear();
                }
                else{
                    blockVector.push_back(line.substr(0,line.size()));
                }
            }

        }
    }
    //Create next-block issues here
    Block* block=new Block(setBlock(shapeVector));
    initial_block=block;
    setRotation(*block);











}
void BlockFall::setRotation(Block &block){
    Block* currentBlock=&block;



    while (currentBlock!= nullptr){
        Block* headBlock=currentBlock;

        while (rotateRight(currentBlock->shape)!=headBlock->shape){
            currentBlock->right_rotation=new Block();
            currentBlock->right_rotation->shape= rotateRight(currentBlock->shape);


            currentBlock->right_rotation->next_block=currentBlock->next_block;

            currentBlock->right_rotation->left_rotation=currentBlock;
            currentBlock=currentBlock->right_rotation;





        }
        currentBlock->right_rotation=headBlock;
        headBlock->left_rotation=currentBlock;
        currentBlock=currentBlock->next_block;
    }






}

vector<vector<bool>> BlockFall::rotateRight(const vector<vector<bool>>& oldShape){
    int rowSize{static_cast<int>(oldShape.size())};
    int columnSize{static_cast<int>(oldShape[0].size())};
    vector<vector<bool>> newShape(columnSize,vector<bool>(rowSize, false));
    for (int i = 0; i < rowSize; ++i) {
        for (int j = 0; j < columnSize; ++j) {
            newShape[j][rowSize-1-i]=oldShape[i][j];

        }


    }

    return newShape;
}
vector<vector<bool>> BlockFall::rotateLeft(const vector<vector<bool>>& oldShape){
    int rowSize{static_cast<int>(oldShape.size())};
    int columnSize{static_cast<int>(oldShape[0].size())};
    vector<vector<bool>> newShape(columnSize,vector<bool>(rowSize, false));
    for (int i = 0; i < rowSize; ++i) {
        for (int j = 0; j < columnSize; ++j) {
            newShape[j][i]=oldShape[i][columnSize-1-j];
        }
    }
    return newShape;

}

Block BlockFall::setBlock(const vector<vector<std::string>>& blocks){
    Block firstBlock;
    Block* currentBlock=&firstBlock;
    int blockIndex{0};

    for (const auto& blockRow : blocks) {
        vector<vector<bool>> shapeVectorOfBlock;
        for (const std::string& value : blockRow) {

            vector<bool> shape;
            for ( const char& singleBlock:value) {
                shape.push_back(singleBlock=='1');
            }
            shapeVectorOfBlock.push_back(shape);
        }
        if (blockIndex<blocks.size()-1){
            currentBlock->shape=shapeVectorOfBlock;
        }
        if (blockIndex<blocks.size()-2){
            currentBlock->next_block=new Block();
            currentBlock=currentBlock->next_block;
        }
        if (blockIndex==blocks.size()-1){
            power_up=shapeVectorOfBlock;
        }



        blockIndex++;
    }
    return firstBlock;
}





void BlockFall::initialize_grid(const string &input_file) {

    std::ifstream file(input_file);
    if (!file.is_open()) {
        std::cerr << "Cannot open file: " << input_file << std::endl;
        return;
    }
    std::string line;

    while (std::getline(file, line)) {
        removeR(line);
        std::istringstream iss(line);
        int value;
        std::vector<int> rowVec;
        while (iss >> value) {
            rowVec.push_back(value);
        }
        grid.push_back(rowVec);
    }
    rows=(int) grid.size();
    cols=(int)grid[0].size();

}


BlockFall::~BlockFall() {
    Block* currenBlock=initial_block;
    while (currenBlock!= nullptr){
        Block* block1=currenBlock->next_block;
        while (currenBlock->right_rotation!=currenBlock){
            Block* block2=currenBlock->right_rotation;
            block2->right_rotation->left_rotation=currenBlock;
            currenBlock->right_rotation=block2->right_rotation;
            delete block2;
        }
        delete currenBlock;
        currenBlock=block1;
    }





}

void BlockFall::removeR(string &line) {
    size_t position = line.find('\r');
    if (position != std::string::npos) {
        line.erase(position, 1);
    }

}

