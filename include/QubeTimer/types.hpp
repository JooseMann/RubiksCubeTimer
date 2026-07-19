// Shared types between all files
#ifndef TYPES_HPP
#define TYPES_HPP

#include <ctime>
#include <string>

typedef struct {
	double time; // Time of the solve
	std::string comment; // User comment about this solve
	uint8_t* scramble; // Binary representation of the scramble
	time_t date; // Date & time solved
} solve_t;

typedef struct {
	float ao5; // Current Ao5
	float ao12; // Current Ao12
	float ao100; // Current Ao100
	float average; // Session average
} averages_t;

#endif // TYPES_HPP
