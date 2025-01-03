#ifndef PA2_GAMECONTROLLER_H
#define PA2_GAMECONTROLLER_H

#include "BlockFall.h"

using namespace std;

class GameController {
public:
    int newBlockRow{0};
    int newBlockColumn{0};
    int highScore{0};
    bool play(BlockFall &game, const string &commands_file); // Function that implements the gameplay
    void removeR(string &line);
    void printGrid( BlockFall& game);
    bool isGameStart(BlockFall& game);
    void placeBlock(BlockFall &game);
    void rightRotation(BlockFall &game);
    bool canRightRotate(BlockFall &game);
    void  cleanOldBlock(BlockFall &game);
    bool canLeftRotate(BlockFall &game);
    void leftRotation(BlockFall &game);
    void rightMove(BlockFall& game);
    bool canRightMove(BlockFall &game);
    void leftMove(BlockFall& game);
    bool canLeftMove(BlockFall& game);
    int dropDistance(BlockFall& game);
    void drop(BlockFall& game);
    void dropGravity(BlockFall& game);
    void dropGravityDistance(BlockFall &game);
    void dropOldBlock(BlockFall& game);
    int countBlockNumber(BlockFall& game);
    int countBlockNumberOfGrid(BlockFall& game);
    bool  isPowerUpExist(BlockFall& game);
    void rowControl(BlockFall& game,bool control);
    void onlyPrintGrid(BlockFall& game);
    void insertToLeaderBoardAndFinishGame(BlockFall& game);

};


#endif //PA2_GAMECONTROLLER_H
