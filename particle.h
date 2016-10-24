/*
 * particle.h
 *
 *  Created on: 23.10.2016
 *      Author: franzt
 */

#ifndef PARTICLE_H_
#define PARTICLE_H_

#include <vector>
#include <algorithm>
#include "config.h"

#define MAX_PARTICLE_DISTANCE 8

class Particle
{
private:
	static const float ParticleGradient[MAX_PARTICLE_DISTANCE];
	float x0, y0;
	int delay;

	void move();

public:
	float x, y, vx, vy;
	bool alive;

	Particle(float x, float y, float vx, float vy, int delay);
	virtual ~Particle();

	void render(uint8_t *target, palette_entry palette[]);
	float distance();
	float distanceTo(float x, float y);
};

#endif /* PARTICLE_H_ */
