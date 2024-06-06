#pragma once

#include <vector>
#include <string>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <zlib.h>
#include <unordered_map>
#include <cstring>

#define PACK_ASSETS_SUCCESS 0
#define PACK_ASSETS_ERROR -1

struct FileName
{
    std::string full;
    std::string relative;
};

struct Asset {
    char* fileName;
    char* fileType;
	unsigned char* data;
    size_t size;
};

void compressPackFile(const char* fileName, int& success);
void packAssets(const char* directory, const char* fileName, int& success);

std::unordered_map<std::string, Asset> unpackAssets(const char* fileName, int& success);
void freeAssets(std::unordered_map<std::string, Asset>& assets);