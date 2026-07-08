// Shared types between all files
#ifndef TYPES_HPP
#define TYPES_HPP

typedef struct {
	double time; // Time of the solve
	long date; // Date & time solved, represented in time since the Unix Epoch
	uint8_t* scramble; // Binary representation of the scramble
} solve_t;

typedef struct {
	float ao5; // Current Ao5
	float ao12; // Current Ao12
	float ao100; // Current Ao100
	float average; // Session average
} averages_t;

#endif // TYPES_HPP
