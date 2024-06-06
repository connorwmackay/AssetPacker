#include "packAssets.h"

void compressPackFile(const char* fileName, int& success)
{
    std::ifstream packFile(fileName, std::ios::binary | std::ios::in);

    if (!packFile.is_open())
    {
        success = PACK_ASSETS_ERROR;
        return;
    }

    std::string packFileData((std::istreambuf_iterator<char>(packFile)),
        std::istreambuf_iterator<char>());
    packFile.close();

    gzFile writePackFile = gzopen(fileName, "wb");
    gzwrite(writePackFile, packFileData.c_str(), packFileData.size());
    gzclose(writePackFile);

    success = PACK_ASSETS_SUCCESS;
}

void packAssets(const char* directory, const char* fileName, int& success) {
    auto filesInDirectory = std::vector<FileName>();

    // Find each file in the directory
    try {
        for (const auto& file : std::filesystem::recursive_directory_iterator(directory)) {
            if (!std::filesystem::is_directory(file))
                filesInDirectory.push_back({
                    file.path().string(),
                    std::filesystem::relative(file.path(), std::filesystem::path(directory)).string()
                    });
        }
    }
    catch (std::filesystem::filesystem_error& error)
    {
        success = PACK_ASSETS_ERROR;
        return;
    }

    // Print out each file found
    for (const FileName& filePath : filesInDirectory) {
        printf("%s\n", filePath.relative.c_str());
    }

    std::ofstream packFile(fileName, std::ios::binary | std::ios::out);

    int numFiles = filesInDirectory.size();
    packFile.write((char*)&numFiles, sizeof(int));

    for (const auto& file : filesInDirectory)
    {
        std::ifstream assetFile(file.full, std::ios::binary | std::ios::in);

        std::vector<unsigned char> assetFileData((std::istreambuf_iterator<char>(assetFile)),
            std::istreambuf_iterator<char>());

        size_t fileNameSize = file.relative.size() * sizeof(char) + 1;
        size_t fileDataSize = assetFileData.size() * sizeof(char) + 1;

        packFile.write((const char*)&fileNameSize, sizeof(size_t)); // File Name Size Header
        packFile.write((const char*)&fileDataSize, sizeof(size_t)); // File Data Size Header
        packFile.write(file.relative.c_str(), file.relative.size() + 1); // File Name
        packFile.write((char*)assetFileData.data(), assetFileData.size() + 1); // File Data

        assetFile.close();
    }

    packFile.close();
    success = PACK_ASSETS_SUCCESS;
    compressPackFile(fileName, success);
}

std::unordered_map<std::string, Asset> unpackAssets(const char* fileName, int& success) {
    std::unordered_map<std::string, Asset> assets = std::unordered_map<std::string, Asset>();
    gzFile packFile = gzopen(fileName, "rb");

    int numFiles = 0;
    gzread(packFile, &numFiles, sizeof(int));

    for (int i = 0; i < numFiles; i++) {
        size_t fileNameSize = 0;
        size_t fileDataSize = 0;

        if (gzread(packFile, &fileNameSize, sizeof(size_t)) == -1)
        {
            std::cerr << "Error: GZRead could not decompress file name size\n";
            success = PACK_ASSETS_ERROR;
            return assets;
        }

        if (gzread(packFile, &fileDataSize, sizeof(size_t)) == -1)
        {
            std::cerr << "Error: GZRead could not decompress file data size\n";
            success = PACK_ASSETS_ERROR;
            return assets;
        }

        char* assetFileName = (char*)malloc(fileNameSize + 1);
        if (gzread(packFile, assetFileName, fileNameSize / sizeof(char)) == -1)
        {
            std::cerr << "Error: GZRead could not decompress file name\n";
            success = PACK_ASSETS_ERROR;
            return assets;
        }
        assetFileName[(fileNameSize / sizeof(char)) - 1] = '\0';

        unsigned char* fileData = (unsigned char*)malloc(fileDataSize + 1);
        if (gzread(packFile, fileData, fileDataSize / sizeof(unsigned char)) == -1)
        {
            std::cerr << "Error: GZRead could not decompress file data\n";
            success = PACK_ASSETS_ERROR;
            return assets;
        }
        fileData[(fileDataSize / sizeof(char)) - 1] = '\0';

        std::string fileNameStr(assetFileName);
        std::size_t startIndOfFileType = fileNameStr.find_last_of(".", fileNameStr.size() - 1);

        Asset asset{};
        asset.size = fileDataSize;
        asset.data = fileData;
        asset.fileName = assetFileName;

        char* fileType = (char*)malloc(1);
        if (fileType == nullptr)
        {
            success = PACK_ASSETS_ERROR;
            return assets;
        }
        fileType[0] = '\0';
        if (startIndOfFileType != std::string::npos) {
            std::string fileTypeStr = fileNameStr.substr(startIndOfFileType);
            fileType = (char*)realloc(fileType, fileTypeStr.size() * sizeof(char) + 1);
            if (fileType == nullptr)
            {
                success = PACK_ASSETS_ERROR;
                return assets;
            }
            strcpy_s(fileType, fileTypeStr.size() * sizeof(char) + 1, fileTypeStr.data());
        }

        asset.fileType = fileType;

        assets.insert_or_assign(asset.fileName, asset);
    }

    gzclose(packFile);
    success = PACK_ASSETS_SUCCESS;
    return assets;
}

void freeAssets(std::unordered_map<std::string, Asset>& assets)
{
    for (auto& asset : assets)
    {
        if (asset.second.data)
        {
            free(asset.second.data);
        }

        if (asset.second.fileName)
        {
            free(asset.second.fileName);
        }

        if (asset.second.fileType)
        {
            free(asset.second.fileType);
        }
    }

    assets.clear();
}