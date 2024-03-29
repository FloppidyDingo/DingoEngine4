#pragma once
#include "Utils.h"
#include "lodepng.h"
#include <math.h>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <vector>
#include <sstream>

bool inBetween(float point, float lowBound, float highBound) {
	return (point > lowBound) && (point < highBound);
}

//loads the image specified by path and places the data in buffer. As of right now only supports PNG. Boolean is if the image loaded is a Bounds image
void loadImage(unsigned char*& buffer, unsigned int& iWidth, unsigned int& iHeight, const char path[], bool bounds)
{
	std::vector<unsigned char> image;
	unsigned int width, height;
	unsigned error = lodepng::decode(image, width, height, path);
	// If there's an error, display it.
	if (error != 0) {
		std::cout << "error " << error << ": " << lodepng_error_text(error) << std::endl;
		return;
	}
	iWidth = width;
	iHeight = height;
	if (bounds) {//check if bounds mode is selected
		//compress image to 8bpp for bounds computation
		buffer = new unsigned char[width * height];
		unsigned int i2 = 0;
		for (unsigned int i = 0; i < width * height * 4; i+=4) {
			buffer[i2] = image[i] | image[i + 1] | image[i + 2] | image[i + 3];
			i2++;
			image[i] = 0xaa;
		}
	} else {
		//copy image to buffer
		buffer = new unsigned char[width * height * 4];
		for (unsigned int i = 0; i < image.size(); i++) {
			buffer[i] = image[i];
		}
	}
	image.clear();
}

float range(Entity e1, Entity e2)
{
	return sqrtf(powf(e2.x - e1.x, 2) + powf(e2.y - e1.y, 2));
}

float clamp(float val, float min, float max)
{
	if (val > max) {
		val = max;
	}
	else if (val < min) {
		val = min;
	}
	return val;
}

//WAVE file functions
//Uses code from https://indiegamedev.net/2020/02/15/the-complete-guide-to-openal-with-c-part-1-playing-a-sound/
std::int32_t convert_to_int(char* buffer, std::size_t len)
{
    std::int32_t a = 0;
    if (std::endian::native == std::endian::little)
        std::memcpy(&a, buffer, len);
    else
        for (std::size_t i = 0; i < len; ++i)
            reinterpret_cast<char*>(&a)[3 - i] = buffer[i];
    return a;
}

bool load_wav_file_header(std::ifstream& file, std::uint8_t& channels, std::int32_t& sampleRate, std::uint8_t& bitsPerSample, ALsizei& size)
{
    char buffer[4];
    if (!file.is_open())
        return false;

    // the RIFF
    if (!file.read(buffer, 4))
    {
        std::cerr << "ERROR: could not read RIFF" << std::endl;
        return false;
    }
    if (std::strncmp(buffer, "RIFF", 4) != 0)
    {
        std::cerr << "ERROR: file is not a valid WAVE file (header doesn't begin with RIFF)" << std::endl;
        return false;
    }

    // the size of the file
    if (!file.read(buffer, 4))
    {
        std::cerr << "ERROR: could not read size of file" << std::endl;
        return false;
    }

    // the WAVE
    if (!file.read(buffer, 4))
    {
        std::cerr << "ERROR: could not read WAVE" << std::endl;
        return false;
    }
    if (std::strncmp(buffer, "WAVE", 4) != 0)
    {
        std::cerr << "ERROR: file is not a valid WAVE file (header doesn't contain WAVE)" << std::endl;
        return false;
    }

    // "fmt/0"
    if (!file.read(buffer, 4))
    {
        std::cerr << "ERROR: could not read fmt/0" << std::endl;
        return false;
    }

    // this is always 16, the size of the fmt data chunk
    if (!file.read(buffer, 4))
    {
        std::cerr << "ERROR: could not read the 16" << std::endl;
        return false;
    }

    // PCM should be 1?
    if (!file.read(buffer, 2))
    {
        std::cerr << "ERROR: could not read PCM" << std::endl;
        return false;
    }

    // the number of channels
    if (!file.read(buffer, 2))
    {
        std::cerr << "ERROR: could not read number of channels" << std::endl;
        return false;
    }
    channels = convert_to_int(buffer, 2);

    // sample rate
    if (!file.read(buffer, 4))
    {
        std::cerr << "ERROR: could not read sample rate" << std::endl;
        return false;
    }
    sampleRate = convert_to_int(buffer, 4);

    // (sampleRate * bitsPerSample * channels) / 8
    if (!file.read(buffer, 4))
    {
        std::cerr << "ERROR: could not read (sampleRate * bitsPerSample * channels) / 8" << std::endl;
        return false;
    }

    // ?? dafaq
    if (!file.read(buffer, 2))
    {
        std::cerr << "ERROR: could not read dafaq" << std::endl;
        return false;
    }

    // bitsPerSample
    if (!file.read(buffer, 2))
    {
        std::cerr << "ERROR: could not read bits per sample" << std::endl;
        return false;
    }
    bitsPerSample = convert_to_int(buffer, 2);

    // data chunk header "data"
    if (!file.read(buffer, 4))
    {
        std::cerr << "ERROR: could not read data chunk header" << std::endl;
        return false;
    }
    if (std::strncmp(buffer, "data", 4) != 0)
    {
        std::cerr << "ERROR: file is not a valid WAVE file (doesn't have 'data' tag)" << std::endl;
        return false;
    }

    // size of data
    if (!file.read(buffer, 4))
    {
        std::cerr << "ERROR: could not read data size" << std::endl;
        return false;
    }
    size = convert_to_int(buffer, 4);

    /* cannot be at the end of file */
    if (file.eof())
    {
        std::cerr << "ERROR: reached EOF on the file" << std::endl;
        return false;
    }
    if (file.fail())
    {
        std::cerr << "ERROR: fail state set on the file" << std::endl;
        return false;
    }

    return true;
}

void stringSplit(std::string const& str, const char delim, std::vector<std::string>& out) {
    // construct a stream from the string
    std::stringstream ss(str);

    std::string s;
    while (std::getline(ss, s, delim)) {
        out.push_back(s);
    }
}

bool load_wav(const std::string& filename, std::uint8_t& channels, std::int32_t& sampleRate, std::uint8_t& bitsPerSample, std::vector<char> &data)
{
    ALsizei size;
    std::ifstream in(filename, std::ios::binary);
    if (!in.is_open())
    {
        error("Could not open \"" + filename + "\"");
        return false;
    }
    if (!load_wav_file_header(in, channels, sampleRate, bitsPerSample, size))
    {
        error("Could not load wav header of \"" + filename + "\"");
        return false;
    }

    data.resize(size);
    in.read(data.data(), size);

    return true;
}

bool intersects(Entity e1, Entity e2) {
    float e1top = e1.y + (e1.getHeight() / 2);
    float e1bottom = e1.y - (e1.getHeight() / 2);
    float e1left = e1.x - (e1.getWidth() / 2);
    float e1right = e1.x + (e1.getWidth() / 2);
    float e2top = e2.y + (e2.getHeight() / 2);
    float e2bottom = e2.y - (e2.getHeight() / 2);
    float e2left = e2.x - (e2.getWidth() / 2);
    float e2right = e2.x + (e2.getWidth() / 2);

    return e1left < e2right && e1right > e2left && e1top > e2bottom && e1bottom < e2top;
}

float randomFloat(float min, float max) {
    return min + (float)(rand()) * (float)(max - min) / RAND_MAX;
}

void seedRNG() {
    srand(time(NULL));
}

bool intersects(Entity e1, float x, float y) {
    return x < (e1.x + e1.getWidth() / 2) &&
        x >(e1.x - e1.getWidth() / 2) &&
        y < (e1.y + e1.getHeight() / 2) &&
        y >(e1.y - e1.getHeight() / 2);
}
