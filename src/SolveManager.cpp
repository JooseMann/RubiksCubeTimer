#include <algorithm> // sort()
#include <cstdint> // uint8_t
#include <cstdlib> // std::exit
#include <filesystem> // std::filesteam::exists
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "types.hpp"
#include "SolveManager.hpp"

SolveManager::SolveManager(const char* filename) {

	// First determine if the given filename already existed
	bool fileExists = true;
	
	if (!std::filesystem::exists(filename)) fileExists = false;

	// Open a file with the given filename
	m_fileStream = std::fstream ();
	m_fileStream.open(filename, std::ios::in | std::ios::out | std::ios::app); // Input and appending

	// Check if the function call failed (e.g., file does not exist)
	if (m_fileStream.bad()) {
		std::cerr << "Error: failed to open file " << filename << std::endl;
		std::exit(1);
	}

	// If the file was just created (fileExists == false beforehand), write a header for the .csv file.
	if (!fileExists) m_fileStream << "time,date,scramble\n";

	// Initialize solves and averages
	m_solves = std::vector<solve_t>();
	m_averages = { -1.0f, -1.0f, -1.0f, -1.0f }; // Set all averages to -1.0f (none) by default

	// Read in the given CSV file
	readFile();
}

// Make sure to close our file stream and free memory when we're done
SolveManager::~SolveManager() {
	m_fileStream.close();

	for (unsigned int i = 0; i < m_numSolves; ++i) {
		delete [] m_solves[i].scramble;
	}
}

void SolveManager::addSolve(const solve_t& solve) {
	// Add our solve to our array
	m_solves.push_back(solve);
	++m_numSolves;

	// Update our averages
	updateAverages();
	
	// Get our solve as a string
	// To convert time to a string with only 2 decimal places (instead of X.XX0000), use std::streamstring
	std::stringstream strStream;
	strStream << std::fixed << std::setprecision(2) << solve.time;
	std::string solveStr = strStream.str() + "," + std::to_string(solve.date) + "," + SolveManager::ScrambleToString(solve.scramble); 

	// Write to our .csv file using our string
	m_fileStream.seekp(0, m_fileStream.end);
	m_fileStream << solveStr.c_str();
	m_fileStream << "\n";
}

// Convert a scramble representation to a string.
std::string SolveManager::ScrambleToString(const uint8_t* scramble, int scrambleLen) {
    std::string scrambleStr = "";

    // Translate the scramble in binary to a string.
    // Go until finding "no move" (end of scramble), 0xFF, or until reaching the max scramble length of 30.
    for (int i = 0; scramble[i] != 0xFF && i < scrambleLen; ++i) {
        // The raw move, without including the modifier (lower 4 bits)
        uint8_t move = scramble[i] % 16;

        // Convert the raw move according to our binary representation.
        switch (move) {
            case 0: // R
                scrambleStr += "R";
                break;
            case 1: // U
                scrambleStr += "U";
                break;
            case 2: // F
                scrambleStr += "F";
                break;
            case 3: // L
                scrambleStr += "L";
                break;
            case 4: // D
                scrambleStr += "D";
                break;
            case 5: // B
                scrambleStr += "B";
                break;
            default: // Unknown / Invalid move
                scrambleStr += "[INVALID MOVE - " + std::to_string(move) + "]";
                break;
        }

        // The modifier applied to the move (upper 4 bits)
        // Converted to 0, 1, or 2 via a right bit shift by 6.
        uint8_t modifier = scramble[i] >> 6;

        switch (modifier) {
            case 0: // No modifier, do nothing
                scrambleStr += " ";
                break;
            case 1: // Prime
                scrambleStr += "' ";
                break;
            case 2: // Double
                scrambleStr += "2 ";
                break;
            default:
                scrambleStr += "[INVALID MODIFIER - " + std::to_string(modifier) + "] ";
                break;
        }
    }

    // The last character in scrambleStr will always be an extraneous space, remove that here
    scrambleStr[scrambleStr.length() - 1] = '\0';

    return scrambleStr;
}

// Helper method to parse a scramble from a string.
uint8_t* SolveManager::StringToScramble(const std::string& scramble) {
	// Set up helper variables
	unsigned int pos = 0;
	unsigned int moveNum = 0;

	// Heap allocated scramble representation
	uint8_t* scrambleRep = new uint8_t[30]; 

	// For every move in the scramble, we want to get its base move and any potential modifier.
	// Keep looping until we reach the end of the scramble, or after 30 moves (at which point, we can't store more data).
	while (pos < scramble.length() && moveNum < 30) {
		// Set the move to 0 by default (heap allocated means we have garbage initially)
		scrambleRep[moveNum] = 0;
		
		// First check for the base move. Add the corresponding integer that represents that move if we find it.
		switch (scramble[pos]) {
			case 'R':
				scrambleRep[moveNum] += 0;
				break;
			case 'U':
				scrambleRep[moveNum] += 1;
				break;
			case 'F':
				scrambleRep[moveNum] += 2;
				break;
			case 'L':
				scrambleRep[moveNum] += 3;
				break;
			case 'D':
				scrambleRep[moveNum] += 4;
				break;
			case 'B':
				scrambleRep[moveNum] += 5;
				break;
			default:
				scrambleRep[moveNum] = 0xFF; // Set to 0xFF in case of an unknown move (error)
				break;
		}
		
		// Move to the next character
		++pos;

		// Now check for modifiers (prime or double)
		switch(scramble[pos]) {
			case ' ': // No modifier
				break;
			case '\'': // Prime
				scrambleRep[moveNum] += (1 << 6); // 01 bit shift left 6 times
				++pos; // Move to the next character (space), our position in the string scramble is consistent.
				break;
			case '2': // Double
				scrambleRep[moveNum] += (2 << 6); // 10 bit shift left 6 times
				++pos; // Move to the next character (space), our position in the string scramble is consistent.
				break;
		}

		// Go to the next move
		++pos;
		++moveNum;
	}

	// Fill the rest of the data with "no move" (0xFF)
	for (int i = moveNum; i < 30; ++i) {
		scrambleRep[i] = 0xFF;
	}

	return scrambleRep;
}

void SolveManager::updateAverages() {
	double sumOfTimes = 0.0;
	double adjustedTime = 0.0;
	size_t numSolves = 0;
	std::vector<double> sortedSolves = std::vector<double>();

	// Looping from the end (newest) to the start (oldest),
	// Calculate ao5, ao12, and ao100 (if applicable).
	for (int i = m_numSolves - 1; i >= 0; --i) {
		sumOfTimes += m_solves[i].time;
		sortedSolves.push_back(m_solves[i].time);

		++numSolves;

		// Compute the corresponding average, if/when numSolves gets to that number of solves
		if (numSolves == 5) { // Ao5
			// Remove the 5% best and worst solves (for Ao5, the best and worst solve)
			sort(sortedSolves.begin(), sortedSolves.end()); // Sort so that we know the best and worst times

			adjustedTime = sumOfTimes;
			adjustedTime -= sortedSolves[0]; // Best solve
			adjustedTime -= sortedSolves[sortedSolves.size() - 1]; // Worst solve

			m_averages.ao5 = adjustedTime / 3;
		}
		if (numSolves == 12) { // Ao12
			// Same deal as Ao5: Remove the best and worst solve time
			sort(sortedSolves.begin(), sortedSolves.end()); // Sort to find the best & worst times

			adjustedTime = sumOfTimes;
			adjustedTime -= sortedSolves[0]; // Best solve
			adjustedTime -= sortedSolves[sortedSolves.size() - 1]; // Worst solve

			m_averages.ao12 = adjustedTime / 10;
		}
		if (numSolves == 100) { // Ao100
			// Same idea, but removing the 5% best and worst scores now involves removing 5 of each
			sort(sortedSolves.begin(), sortedSolves.end()); // Sort all 100 solves to find the best and worst ones

			adjustedTime = sumOfTimes;

			for (int j = 0; j < 5; ++j) {
				adjustedTime -= sortedSolves[i]; // Best 5 solves
				adjustedTime -= sortedSolves[sortedSolves.size() - i - 1]; // Worst 5 solves
			}

			m_averages.ao100 = adjustedTime / 90;
		}
	}

	// Assuming we have a solve, compute the average of the entire session
	if (m_numSolves > 0) m_averages.average = sumOfTimes / m_numSolves;
}

// Private method to read in the data from the current .csv file
void SolveManager::readFile() {
	// First move to the start of the file
	m_fileStream.seekg(0, m_fileStream.beg);

	// Set up helper variables
	unsigned int numLines = 0;
	char rawLine[1024];
	std::string line;
	std::string stats[3]; // Stats here are time (0), date (1), and scramble (2)

	// Keep reading the file while not at EOF
	while (!m_fileStream.eof()) {
		m_fileStream.getline(rawLine, 1024);

		// Skip the first line (header)
		if (numLines == 0) {
			++numLines;
			continue;
		}

		line = std::string(rawLine);

		// We might have an empty line to indicate EOF. In the case, break out
		if (line.length() <= 1) break;

		// Here, we are reading a CSV file. So, we deliminate by commas here.
		for (int i = 0; i < 3; ++i) {
			// Get the comma's position
			int delimPos = line.find(',');

			// Get our desired statistic (time -> date -> scramble)
			stats[i] = line.substr(0, delimPos);

			// Erase everything up to and including the comma
			// Note that this only erases our copy of the string, not the information in the .csv file itself
			line.erase(0, delimPos + 1);
		}

		// Write it all into a solve
		double time = std::stod(stats[0]);
		long date = std::stol(stats[1]);
		uint8_t* scramble = SolveManager::StringToScramble(stats[2]); // Heap allocated
		solve_t solve = { time, date, scramble };

		// Add this solve to our list of solves
		m_solves.push_back(solve);

		// Move onto the next line (solve)
		++numLines;
	}

	// Reset flags that might have been set while reading the file
	m_fileStream.clear();

	// Update the number of solves
	m_numSolves = m_solves.size();

	// Now compute averages
	updateAverages();
}
