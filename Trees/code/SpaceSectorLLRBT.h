#ifndef SPACESECTORLLRBT_H
#define SPACESECTORLLRBT_H

#include "Sector.h"
#include <iostream>
#include <fstream>  
#include <sstream>
#include <vector>
#include <stack>
#include <queue>

class SpaceSectorLLRBT {
public:
    Sector* root;
    SpaceSectorLLRBT();
    ~SpaceSectorLLRBT();
    void readSectorsFromFile(const std::string& filename);
    void insertSectorByCoordinates(int x, int y, int z);
    void displaySectorsInOrder();
    void displaySectorsPreOrder();
    void displaySectorsPostOrder();
    std::vector<Sector*> getStellarPath(const std::string& sector_code);
    void printStellarPath(const std::vector<Sector*>& path);
    Sector* insertSector(Sector* sector , int x , int y , int z);
    void balanceControl(Sector* sector);
    void  colorFlip(Sector* sector);
    void leftRotate(Sector* sector);
    void rightRotate(Sector* sector);
    std::vector<Sector*> allSectors;
    void inorder(Sector* sector);
    void preorder(Sector* sector);
    void postorder(Sector* sector);
    Sector* findRequiredSector(const std::string& sector_code);

};

#endif // SPACESECTORLLRBT_H
