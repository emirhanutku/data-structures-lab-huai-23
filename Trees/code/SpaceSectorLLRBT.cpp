#include "SpaceSectorLLRBT.h"

using namespace std;

SpaceSectorLLRBT::SpaceSectorLLRBT() : root(nullptr) {}

void SpaceSectorLLRBT::readSectorsFromFile(const std::string& filename) {
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
SpaceSectorLLRBT::~SpaceSectorLLRBT() {
    for(Sector* sector:allSectors){
        delete sector;
    }
}
void SpaceSectorLLRBT::insertSectorByCoordinates(int x, int y, int z) {
    insertSector(root,x,y,z);
    balanceControl(allSectors.back());
    root->color= false;
}
Sector* SpaceSectorLLRBT::insertSector(Sector* sector , int x , int y , int z){
    if (sector== nullptr){
        sector=new Sector(x,y,z);
        if (root== nullptr){
            sector->color= false;
            root=sector;
        }
        else{sector->color= true;}
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
void SpaceSectorLLRBT::balanceControl(Sector* sector){
    if (sector->parent!= nullptr){
        if (sector->parent->right!= nullptr&&sector->parent->left!= nullptr&&sector->parent->right->color && sector->parent->left->color){
            colorFlip(sector);
            balanceControl(sector->parent);
        }
        else if (sector->parent->right!= nullptr&&sector->parent->right==sector && sector->color){
            leftRotate(sector);
            balanceControl(sector->left);
        }
        else if (sector->parent->color && sector->color){
            rightRotate(sector);
            balanceControl(sector);
        }
    }
}
void  SpaceSectorLLRBT::colorFlip(Sector* sector){
    sector->parent->right->color= false;
    sector->parent->left->color= false;
    sector->parent->color= true;
}

void SpaceSectorLLRBT::leftRotate(Sector* sector){
    if (root->right!= nullptr&&root->right==sector){
        root=root->right;
    }
    Sector* parentSector=sector->parent;
    if (parentSector->parent!= nullptr){
        sector->parent=parentSector->parent;
        if (parentSector->parent->right==parentSector){
            parentSector->parent->right=sector;
        }
        else{
            parentSector->parent->left=sector;
        }
    }
    else{sector->parent= nullptr;}
    parentSector->parent=sector;
    bool colorOfParent=parentSector->color;
    bool colorOfSector=sector->color;
    sector->color= colorOfParent;
    parentSector->color= colorOfSector;
    if (sector->left!= nullptr){
        parentSector->right=sector->left;
        parentSector->right->parent=parentSector;
    }
    else{
        parentSector->right= nullptr;
    }
    sector->left=parentSector;
}

void SpaceSectorLLRBT::rightRotate(Sector* sector){
    if (root->left!= nullptr&&root->left==sector->parent){
        root=root->left;
    }
    Sector* parentSector=sector->parent;
    Sector* grandParentSector=sector->parent->parent;

    if (grandParentSector->parent!= nullptr){
        parentSector->parent=grandParentSector->parent;

        if (grandParentSector->parent->right==grandParentSector){
            grandParentSector->parent->right=parentSector;
        }
        else{
            grandParentSector->parent->left=parentSector;
        }
    }
    else{parentSector->parent= nullptr;}

    grandParentSector->parent=parentSector;

    bool colorOfParentSector=parentSector->color;
    parentSector->color= grandParentSector->color;
    grandParentSector->color= colorOfParentSector;

    if (parentSector->right!= nullptr){
        grandParentSector->left=parentSector->right;
        grandParentSector->left->parent=grandParentSector;
    } else{grandParentSector->left= nullptr;
    }
    parentSector->right=grandParentSector;
}

void SpaceSectorLLRBT::displaySectorsInOrder() {

    std::cout<<"Space sectors inorder traversal:"<<std::endl;
    inorder(root);
    std::cout<<endl;
}

void SpaceSectorLLRBT::displaySectorsPreOrder() {
    std::cout<<"Space sectors preorder traversal:"<<std::endl;
    preorder(root);
    std::cout<<endl;
}

void SpaceSectorLLRBT::displaySectorsPostOrder() {
    std::cout<<"Space sectors postorder traversal:"<<std::endl;
    postorder(root);
    std::cout<<endl;
}

void SpaceSectorLLRBT::inorder(Sector* sector){
    if (sector!= nullptr){
        inorder(sector->left);
        std::string color = sector->color ? "RED" : "BLACK";
        std::cout<<color<<" sector: ";
        std::cout<<sector->sector_code<<std::endl;
        inorder(sector->right);
    }
}

void SpaceSectorLLRBT::preorder(Sector* sector){
    if (sector!= nullptr){
        std::string color = sector->color ? "RED" : "BLACK";
        std::cout<<color<<" sector: ";
        std::cout<<sector->sector_code<<std::endl;
        preorder(sector->left);
        preorder(sector->right);
    }
}

void SpaceSectorLLRBT::postorder(Sector* sector){
    if (sector!= nullptr){
        postorder(sector->left);
        postorder(sector->right);
        std::string color = sector->color ? "RED" : "BLACK";
        std::cout<<color<<" sector: ";
        std::cout<<sector->sector_code<<std::endl;
    }
}

std::vector<Sector*> SpaceSectorLLRBT::getStellarPath(const std::string& sector_code) {
    std::vector<Sector*> path;
    Sector* requiredSector{findRequiredSector(sector_code)};
    Sector* worldSector{findRequiredSector("0SSS")};
    queue<Sector*> worldToPath;
    std::vector<Sector*> requiredSectorParents;
    std::vector<Sector*> worldSectorParents;
    stack<Sector*> pathToRequired;

    if (requiredSector!= nullptr){
        while (requiredSector->parent!= nullptr){
            requiredSectorParents.push_back(requiredSector);
            requiredSector=requiredSector->parent;
        }
        while (worldSector!= nullptr){
            worldSectorParents.push_back(worldSector);
            worldSector=worldSector->parent;
        }
        bool control{false};
        for (auto & worldSectorParent : worldSectorParents) {
            worldToPath.push(worldSectorParent);
            while (!pathToRequired.empty()) {pathToRequired.pop();}

            for (auto & requiredSectorParent : requiredSectorParents) {
                if (worldSectorParent==requiredSectorParent){
                    control= true;
                    break;
                }
                pathToRequired.push(requiredSectorParent);
            }
            if (control){break;}
        }
        if(!worldToPath.empty()){
            while (!worldToPath.empty()){
                path.push_back(worldToPath.front());
                worldToPath.pop();
            }
            while (!pathToRequired.empty()){
                path.push_back(pathToRequired.top());
                pathToRequired.pop();
            }
        }
    }
    return path;
}
Sector* SpaceSectorLLRBT::findRequiredSector(const std::string& sector_code){
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
    return requiredSector;
}

void SpaceSectorLLRBT::printStellarPath(const std::vector<Sector*>& path) {
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