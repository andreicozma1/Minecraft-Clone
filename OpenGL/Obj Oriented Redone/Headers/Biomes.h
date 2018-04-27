#pragma once
#include <FastNoise.h>


class Biomes
{
public:
	Biomes();
	~Biomes();


	FastNoise noiseGenerator;


	enum biomeType{
		BIOME_PLAINS = 0,
		BIOME_HILLS = 1,
		BIOME_MOUNTAINS = 2,
		BIOME_CELLULAR  = 3,
		CLOUD = -1
	};


	biomeType current = BIOME_PLAINS;
	int maxHeight = 0;


	float getHeight(biomeType biome, int x, int z);
};

