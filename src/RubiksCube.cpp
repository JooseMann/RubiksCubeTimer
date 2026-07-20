#include <cstdint>
#include <random>
#include <string>
#include <QString>

#include "RubiksCube.hpp"
#include "SolveManager.hpp"

RubiksCube::RubiksCube() {
    // Allocate space for 30 moves as the scramble
    // This space will be reused for each new solve
    m_scramble = new uint8_t[30];

    // Also allocate space for our cube representation
    m_cubeRepresentation = new uint8_t*[6]; // Allocate room for 6 faces
    for (int i = 0; i < 6; ++i) {
        m_cubeRepresentation[i] = new uint8_t[9]; // Allocate space for 9 pieces for every face
    }

    // Generate the scramble here
    // Also determines the scramble length
    this->generateScramble();
}

RubiksCube::~RubiksCube() {
    delete [] m_scramble; // Delete the scramble memory
    for (int i = 0; i < 6; ++i) {
        delete [] m_cubeRepresentation[i]; // Delete the memory for each face of the cube
    }
    delete [] m_cubeRepresentation; // Delete the memory for the cube itself
}

QString RubiksCube::getStringScramble() const {
    // Convert our scramble to a std::string, then to a QString using its C string.
    QString scrambleStr(SolveManager::ScrambleToString(m_scramble, m_scrambleLen).c_str());

    return scrambleStr;
}

void RubiksCube::generateScramble() {
    m_scrambleLen = (rand() % 11) + 20; // 20 - 30 move scramble

    // Scramble creation process: generate the move, then add an optional modifier

    // Generate the lower 4 bytes
    // The first move can be any of the 6 faces, so we use % 6
    uint8_t move = rand() % 6;

    // Store the raw move in prevMove, so we can make sure that we don't have the same move twice in a row
    uint8_t prevMove = move;

    // Add a modifier, bounded [0, 2]
    // Using binary, we have 00, 01, or 10. We can transfer these to 
    // 0000 0000, 0100 0000, and 1000 0000 via a left bit shift by 6.
    move += ((rand() % 3) << 6);

    // Add this to the list of moves
    m_scramble[0] = move;

    // Now repeat for the rest of the moves, making sure we never repeat the same move twice
    for (int i = 1; i < m_scrambleLen; ++i) {
        // Generate lower 4 bytes
        // We now want to disallow the same move from happening twice.
        // So we take % 5 instead, and add 1 if move > prevMove
        // This keeps the odds of the remaining 5 moves the same, but prevents the same move from happening again.
        move = rand() % 5;

        if (move >= prevMove) ++move;

        // Update prevMove
        prevMove = move;

        // Add a modifier, same as above
        move += ((rand() % 3) << 6);

        // Add to the list of moves
        m_scramble[i] = move;
    }

    // Fill any unused moves in the scramble with 1111 1111 (marker for "no move")
    for (int i = m_scrambleLen; i < 30; ++i) {
        m_scramble[i] = 0xFF;
    }

    // Now update our cube representation accordingly
    this->updateCubeRepresentation();
}

void RubiksCube::R() {
    // Not implemented
}

void RubiksCube::U() {
    // Not implemented
}

void RubiksCube::F() {
    // Not implemented
}

void RubiksCube::L() {
    // Not implemented
}

void RubiksCube::D() {
    // Not implemented
}

void RubiksCube::B() {
    // Not implemented
}

void RubiksCube::resetCubeRepresentation() {
    // Reset the cube to a solved state
    for (int face = 0; face < 6; ++face) {
        for (int piece = 0; piece < 9; ++piece) {
            // Set each piece's color
            // The color (in a solved state) is just based on the face
            // Use a bit shift to get the right color (see util/cube_representation.txt for details)
            m_cubeRepresentation[face][piece] = 1 << (face - 1);

            // Now set the right piece type
            if (piece == 4) { // Index 4 is center
                // Center is denoted by adding 0x40
                m_cubeRepresentation[face][piece] += 0x40;
            }
            else if (piece % 2 == 1) { // Odd indexes are edges
                // Edges are denoted by adding 0x80
                m_cubeRepresentation[face][piece] += 0x80;
            }
            else if (piece % 2 == 0) { // Even indexes (except 4) are corners
                // Corners are denoted by adding 0xC0
                m_cubeRepresentation[face][piece] += 0xC0;
            }
        }
    }
}

void RubiksCube::updateCubeRepresentation() {
    // First reset the cube's representation.
    this->resetCubeRepresentation();

    // Now scramble it according to our generated scramble
    for (int moveNum = 0; moveNum < m_scrambleLen; ++moveNum) {
        unsigned int move = m_scramble[moveNum] % 16; // 0 - 5, determining the move
        unsigned int modifier = m_scramble[moveNum] >> 6; // 0 - 2, determining the modifier (how many times to turn the cube).

        // Determine which move to do and how many times to do it.
        // Note that (for example) R' is the same as R 3 times.
        switch (move) {
            case 0: // R
                this->R(); // Base move
                if (modifier == 1 || modifier == 2) this->R(); // Move twice if R2
                if (modifier == 1) this->R(); // Move 3 times if R'
                break;
            case 1: // U
                this->U(); // Base move
                if (modifier == 1 || modifier == 2) this->U(); // Move twice if U2
                if (modifier == 1) this->U(); // Move 3 times if U'
                break;
            case 2: // F
                this->F(); // Base move
                if (modifier == 1 || modifier == 2) this->F(); // Move twice if F2
                if (modifier == 1) this->F(); // Move 3 times if F'
                break;
            case 3: // L
                this->L(); // Base move
                if (modifier == 1 || modifier == 2) this->L(); // Move twice if L2
                if (modifier == 1) this->L(); // Move 3 times if L'
                break;
            case 4: // D
                this->D(); // Base move
                if (modifier == 1 || modifier == 2) this->D(); // Move twice if D2
                if (modifier == 1) this->D(); // Move 3 times if D'
                break;
            case 5: // B
                this->B(); // Base move
                if (modifier == 1 || modifier == 2) this->B(); // Move twice if B2
                if (modifier == 1) this->B(); // Move 3 times if B'
                break;
        }
    }
}
