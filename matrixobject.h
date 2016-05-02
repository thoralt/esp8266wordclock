// ESP8266 Wordclock
// Copyright (C) 2016 Thoralt Franz, https://github.com/thoralt
//
//  See MatrixObject.cpp for description.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef _MATRIXOBJECT_H_
#define _MATRIXOBJECT_H_

#include <vector>
#include <algorithm>
#include "config.h"

class MatrixObject
{
public:
	bool operator<(const MatrixObject &other) const { return y < other.y; }
	MatrixObject();
	void render(uint8_t *buf);

private:
	void randomize();
	void move();

	static const int MatrixSpeed = 3500;
	static const std::vector<palette_entry> MatrixGradient;
	int speed = 0;
	int count = 0;
	int x = 0;
	int y = 0;
};

#endif /* _MATRIXOBJECT_H_ */
