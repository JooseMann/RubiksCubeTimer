#ifndef SOLVE_MANAGER_HPP
#define SOLVE_MANAGER_HPP

#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

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


class SolveManager {
public:
	SolveManager(const char* filename);
	~SolveManager();

	// std::vector<solve_t> solves
	const std::vector<solve_t>& solves() const { return m_solves; }
	void addSolve(const solve_t& solve); 

	// averages_t averages
	float ao5() const { return m_averages.ao5; }
	float ao12() const { return m_averages.ao12; }
	float ao100() const { return m_averages.ao100; }
	float average() const { return m_averages.average; }

	static uint8_t* StringToScramble(const std::string& scramble);
    static std::string ScrambleToString(const uint8_t* scramble, int scrambleLen = 30);

private:
	std::fstream m_fileStream; // IO stream for reading and writing solves
	std::vector<solve_t> m_solves; // Solve list as a dynamic array
	averages_t m_averages; // Solve averages for the current session

	void updateAverages();
	void readFile();
};

#endif
