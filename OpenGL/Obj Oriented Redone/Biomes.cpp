#include "Biomes.h"


Biomes::Biomes()
{

	
}


Biomes::~Biomes()
{
}

float Biomes::getHeight(biomeType biome, int x, int z) {
	int result;

	switch (biome) {
	case BIOME_PLAINS:
		noiseGenerator.SetNoiseType(FastNoise::PerlinFractal);
		noiseGenerator.SetFrequency(0.01);
		maxHeight = 15;
		result = noiseGenerator.GetNoise(x, z) * maxHeight;
		break;
	case BIOME_HILLS:
		noiseGenerator.SetNoiseType(FastNoise::PerlinFractal);
		noiseGenerator.SetFrequency(0.01);
		maxHeight = 25;
		result = noiseGenerator.GetNoise(x, z) * maxHeight;
		break;
	case BIOME_MOUNTAINS:
		noiseGenerator.SetNoiseType(FastNoise::SimplexFractal);
		noiseGenerator.SetFrequency(0.005);
		maxHeight = 75;
		result = noiseGenerator.GetNoise(x, z) * maxHeight;
		break;
	case BIOME_CELLULAR:
		noiseGenerator.SetNoiseType(FastNoise::Cellular);
		noiseGenerator.SetFrequency(0.05);
		maxHeight = 15;
		result = noiseGenerator.GetNoise(x, z) * maxHeight;
		break;
	case CLOUD:
		noiseGenerator.SetNoiseType(FastNoise::Cubic);
		noiseGenerator.SetFrequency(0.015);
		maxHeight = 3;
		result = noiseGenerator.GetNoise(x, z) * maxHeight;
		break;
	}

	result = result;
	return result;
}