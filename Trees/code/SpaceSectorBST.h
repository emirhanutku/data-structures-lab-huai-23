#ifndef SPACESECTORBST_H
#define SPACESECTORBST_H

#include <iostream>
#include <fstream>  
#include <sstream>
#include <vector>

#include "Sector.h"

class SpaceSectorBST {
  
public:
    Sector *root;
    SpaceSectorBST();
    ~SpaceSectorBST();
    void readSectorsFromFile(const std::string& filename); 
    void insertSectorByCoordinates(int x, int y, int z);
    void deleteSector(const std::string& sector_code);
    void displaySectorsInOrder();
    void displaySectorsPreOrder();
    void displaySectorsPostOrder();
    std::vector<Sector*> getStellarPath(const std::string& sector_code);
    void printStellarPath(const std::vector<Sector*>& path);
    Sector* insertSector(Sector* sector , int x , int y , int z);
    void inorder(Sector* sector);
    void preorder(Sector* sector);
    void postorder(Sector* sector);
    void deleteItem(const std::string& sector_code );
    void deleteNodeItem(Sector* sector);
    Sector* findInorderSuccessor(Sector* sector);
    Sector* findSectorFromCode(Sector* sector , const std::string& sector_code);
    std::vector<Sector*> allSectors;
};

#endif // SPACESECTORBST_H
