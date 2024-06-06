#include <iostream>
#include <packAssets.h>

int main(int argc, char** argv) {
    printf("Choose a directory to pack assets from: ");
    std::string directory;
    std::cin >> directory;

    int packSuccess;
    packAssets(directory.c_str(), "DATA.pck", packSuccess);
    if (packSuccess != PACK_ASSETS_SUCCESS)
    {
        std::cerr << "Could not pack the assets at " << directory.c_str() << "\n";
        return 0;
    }

    int unpackSuccess;
    std::unordered_map<std::string, Asset> assets = unpackAssets("DATA.pck", unpackSuccess);

    if (unpackSuccess == PACK_ASSETS_SUCCESS) {
        for (const auto& asset : assets)
        {
            std::cout << "Asset Name: " << asset.second.fileName << ", Asset Type: " << asset.second.fileType << ", Asset Data: \n" << asset.second.data << "\n======\n";
        }
    }
    else
    {
        std::cerr << "Could not unpack assets from DATA.pck\n";
    }

    freeAssets(assets);
    return 0;
}

