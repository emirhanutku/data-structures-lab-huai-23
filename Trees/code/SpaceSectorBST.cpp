#include "SpaceSectorBST.h"
#include <stack>

using namespace std;

SpaceSectorBST::SpaceSectorBST() : root(nullptr) {}

SpaceSectorBST::~SpaceSectorBST() {
    for(Sector* sector:allSectors){
        delete sector;
    }
}

void SpaceSectorBST::readSectorsFromFile(const std::string& filename) {
    ifstream file(filename);

    if (file.is_open()) {
        string line;
        getline(file, line);
        while (getline(file, line)) {
            vector<string> locations;
            stringstream iss(line);
            string location;
            while (getline(iss, location, ',')) {
                locations.push_back(location);
            }
            insertSectorByCoordinates(std::stoi(locations[0]), std::stoi(locations[1]), std::stoi(locations[2]));
        }
        file.close();
    }
}

void SpaceSectorBST::insertSectorByCoordinates(int x, int y, int z) {
    root= insertSector(root,x,y,z);
}

void SpaceSectorBST::deleteSector(const std::string& sector_code) {

    deleteItem(sector_code);

}

void SpaceSectorBST::displaySectorsInOrder() {
    std::cout<<"Space sectors inorder traversal:"<<std::endl;
    inorder(root);
    std::cout<<endl;

}

void SpaceSectorBST::displaySectorsPreOrder() {
    std::cout<<"Space sectors preorder traversal:"<<std::endl;
    preorder(root);
    std::cout<<endl;
}

void SpaceSectorBST::displaySectorsPostOrder() {
    std::cout<<"Space sectors postorder traversal:"<<std::endl;
    postorder(root);
    std::cout<<endl;
}

std::vector<Sector*> SpaceSectorBST::getStellarPath(const std::string& sector_code) {
    std::vector<Sector*> path;

    Sector* requiredSector{nullptr};
    stack<Sector*> sectorPath;
    for(Sector* sector:allSectors){
        if (!sector->isDeleted){
            if (sector->sector_code==sector_code){
                requiredSector=sector;
                break;
            }
        }
    }
    while (requiredSector!= nullptr){
        sectorPath.push(requiredSector);
        requiredSector=requiredSector->parent;
    }
    while (!sectorPath.empty()){
        path.push_back(sectorPath.top());
        sectorPath.pop();
    }
    return path;
}

void SpaceSectorBST::printStellarPath(const std::vector<Sector*>& path) {
    if (!path.empty()){
        std::cout<<"The stellar path to Dr. Elara: ";
        int i{0};
        for(Sector* sector:path){
            i++;
            std::cout<<sector->sector_code;
            if (i<path.size()){std::cout<<"->";}
        }
        std::cout<<endl;
    }
    else{std::cout<<"A path to Dr. Elara could not be found.";}
}

void SpaceSectorBST::deleteItem(const std::string& sector_code ){
    for(Sector* sector:allSectors){
        if (!sector->isDeleted){
            if (sector->sector_code==sector_code){
                deleteNodeItem(sector);
                break;
            }
        }
    }
}

void SpaceSectorBST::deleteNodeItem(Sector* sector){
    bool isRoot = sector->sector_code == root->sector_code;
    if (sector->right== nullptr && sector->left== nullptr){
        if (isRoot) {root = nullptr;}
        else if (sector->parent != nullptr && sector->parent->right != nullptr && sector->parent->right==sector){sector->parent->right= nullptr;}
        else if (sector->parent != nullptr){sector->parent->left= nullptr;}
        sector->isDeleted= true;
    }
    else if (sector->left== nullptr){
        if (isRoot) {root = sector->right;}
        else if (sector->parent != nullptr && sector->parent->right != nullptr && sector->parent->right==sector){sector->parent->right= sector->right;}
        else if (sector->parent != nullptr){sector->parent->left= sector->right;}
        sector->isDeleted= true;
        sector->right->parent = sector->parent;

    }
    else if (sector->right== nullptr){
        if (isRoot) {root = sector->left;}
        else if (sector->parent != nullptr && sector->parent->right != nullptr && sector->parent->right==sector){sector->parent->right= sector->left;}
        else if (sector->parent != nullptr){sector->parent->left= sector->left;}
        sector->isDeleted= true;
        sector->left->parent = sector->parent;
    }
    else{
        Sector* inorderSuccessor=findInorderSuccessor(sector->right);
        if (sector->parent!= nullptr&&sector->parent->right!= nullptr&&sector->parent->right==sector){
            inorderSuccessor->parent=sector->parent;
            sector->parent->right=inorderSuccessor;
        }
        else if (sector->parent!= nullptr&&sector->parent->left!= nullptr&&sector->parent->left==sector){
            inorderSuccessor->parent=sector->parent;
            sector->parent->left=inorderSuccessor;
        }
        else{
            root=inorderSuccessor;
            inorderSuccessor->parent = nullptr;
        }
        if (sector->right!= nullptr){
            inorderSuccessor->right=sector->right;
            sector->right->parent=inorderSuccessor;
        }
        if (sector->left!= nullptr){
            inorderSuccessor->left=sector->left;
            sector->left->parent=inorderSuccessor;}
    }
    sector->isDeleted= true;
}

Sector* SpaceSectorBST::findInorderSuccessor(Sector* sector){
    if (sector->left== nullptr){
        if (sector->right!= nullptr){
            if (sector->parent->right==sector){sector->parent->right=sector->right;}
            else{sector->parent->left=sector->right;}
            sector->right->parent = sector->parent;
            sector->right= nullptr;
        }
        else{
            if (sector->parent->right==sector){sector->parent->right= nullptr;}
            else{sector->parent->left= nullptr;}
        }
        return sector;
    } else{
        return findInorderSuccessor(sector->left);
    }
}

Sector* SpaceSectorBST::insertSector(Sector* sector , int x , int y , int z){
    if (sector== nullptr){
        sector=new Sector(x,y,z);
        allSectors.push_back(sector);
        return  sector;
    }
    else{
        int control;

        if (sector->x != x){control= x > sector->x ? 1 : 0;}
        else if (sector->y!=y){control= y > sector->y ? 1 : 0;}
        else if (sector->z!=z){control= z > sector->z ? 1 : 0;}
        else{control=-1;}

        if (control){
            sector->right= insertSector(sector->right,x,y,z);
            sector->right->parent=sector;
        } else{
            sector->left= insertSector(sector->left,x,y,z);
            sector->left->parent=sector;
        }

        return  sector;
    }
}

void SpaceSectorBST::inorder(Sector* sector){
    if (sector!= nullptr){
        inorder(sector->left);
        std::cout<<sector->sector_code<<std::endl;
        inorder(sector->right);
    }
}

void SpaceSectorBST::preorder(Sector* sector){
    if (sector!= nullptr){
        std::cout<<sector->sector_code<<std::endl;
        preorder(sector->left);
        preorder(sector->right);
    }
}

void SpaceSectorBST::postorder(Sector* sector){
    if (sector!= nullptr){
        postorder(sector->left);
        postorder(sector->right);
        std::cout<<sector->sector_code<<std::endl;
    }
}