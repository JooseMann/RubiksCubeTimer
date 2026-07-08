#ifndef SOLVE_MANAGER_HPP
#define SOLVE_MANAGER_HPP

#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "types.hpp"

class SolveManager {
public:
	SolveManager(const char* filename);
	~SolveManager();

	// std::vector<solve_t> solves
	const std::vector<solve_t>& solves() const { return m_solves; }
	void addSolve(const solve_t& solve); 

	// averages_t averages
	const averages_t& averages() const { return m_averages; }

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
