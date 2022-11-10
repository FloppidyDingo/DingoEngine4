#pragma once
#include "DE4Types.h"
#include <vector>

void error(std::string error);

void log(std::string log);

float range(Entity e1, Entity e2);

float clamp(float val, float min, float max);

bool intersects(Entity e1, Entity e2);

bool inBetween(float point, float lowBound, float highBound);

void loadImage(unsigned char*& buffer, unsigned int& iWidth, unsigned int& iHeight, const char path[], bool bounds);

bool load_wav(const std::string& filename, std::uint8_t& channels, std::int32_t& sampleRate, std::uint8_t& bitsPerSample, std::vector<char> &data);

std::int32_t convert_to_int(char* buffer, std::size_t len);

bool load_wav_file_header(std::ifstream& file, std::uint8_t& channels, std::int32_t& sampleRate, std::uint8_t& bitsPerSample, ALsizei& size);

void stringSplit(std::string const& str, const char delim, std::vector<std::string>& out);

bool intersects(Entity e1, float x, float y);

float randomFloat(float min, float max);

void seedRNG();
