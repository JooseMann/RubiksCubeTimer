#include <cstdint>
#include <list>
#include <QString>
#include <string>
#include <utility>

#include "RubiksCube.hpp"
#include "SolveManager.hpp"

// Iterator typedef to avoid writing out the long type declaration every time
typedef std::list<std::pair<int, int>>::const_iterator listitr_t;

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

// Rotates the face given by the parameter by 90 degrees clockwise.
// When doing any move, a face will also have to rotate alongside whatever else was moved. This function handles that.
void RubiksCube::rotateFinalFace(int face) {
    // The bottom row (that we calculate last) will be replaced with the rhs.
    // So store the rhs from before we overwrite it, that way we just move the data around instead of writing incorrect data.
    uint8_t rhs[3];

    // Store the rhs in reverse, so that the resulting pieces will be placed in the right order.
    rhs[0] = m_cubeRepresentation[face][8];
    rhs[1] = m_cubeRepresentation[face][5];
    rhs[2] = m_cubeRepresentation[face][2];

    // We also store the top-left and top-center pieces, as we will need to use them after we overwrite them.
    uint8_t topLeft = m_cubeRepresentation[face][0];
    uint8_t topCenter = m_cubeRepresentation[face][1];

    int row; // Row that the piece lands on (0 = top, 1 = center, 2 = bottom)

    for (int piece = 0; piece < 9; ++piece) {
        row = piece / 3;

        if (piece == 2) {
            // We need to access data in the top-left piece that has already been overwritten.
            // Use the data from before it was changed in topLeft
            m_cubeRepresentation[face][2] = topLeft;
        }
        else if (piece == 5) {
            // Same as with piece #2, but for the top-center piece instead.
            m_cubeRepresentation[face][5] = topCenter;
        }
        else if (row == 0) {
            // Replace 0 with 6, 1 with 3, and 2 with 0.
            m_cubeRepresentation[face][piece] = m_cubeRepresentation[face][6 - piece * 3];
        }
        else if (row == 1) {
            // Replace 3 with 7, 4 with 4 (center stays unchanged), and 5 with 1.
            m_cubeRepresentation[face][piece] = m_cubeRepresentation[face][7 - (piece - 3) * 3];
        }
        else if (row == 2) {
            // Use stored data to make sure that we don't copy over data that we already overwrote.
            m_cubeRepresentation[face][piece] = rhs[piece - 6];
        }
    }
}

void RubiksCube::R() {
    // Map that describes how to rotate the pieces around for a R move.
    // See `util/reference/cube_representation.txt` for details on how the faces are labeled by integers.
    std::list<std::pair<int, int>> turnMap ({
        std::make_pair(2, 5), // Green -> Yellow
        std::make_pair(5, 4), // Yellow -> Blue
        std::make_pair(4, 0), // Blue -> White
        std::make_pair(0, 2)  // White -> Green
    });

    // Store the original values of the first face we overwrite (here, the Green face).
    // That way we can refer back to it after overwriting everything else.
    // We'll need to reference the rhs of the Green face, so store those values.
    uint8_t greenFace[3];
    greenFace[0] = m_cubeRepresentation[2][2];
    greenFace[1] = m_cubeRepresentation[2][5];
    greenFace[2] = m_cubeRepresentation[2][8];

    // Different faces have to be handled slightly differently. Keep track of which face we're editing to make sure that we update it correctly.
    int iteration = 0;

    for (listitr_t itr = turnMap.begin(); itr != turnMap.end(); ++itr, ++iteration) {
        for (int i = 0; i < 3; ++i) {
            if (iteration == 1) { // Yellow -> Blue
                // The blue face is working with the lhs instead of the rhs, opposite of the rest.
                // It also goes in descending order as compared to the other face, so we have to reverse the result.
                m_cubeRepresentation[itr->first][i * 3 + 2] = m_cubeRepresentation[itr->second][6 - i * 3];
            }
            else if (iteration == 2) { // Blue -> White
                // Same as iteration #1, but the blue face is on the other side of the expression.
                // Naturally, we need to reverse the result here as well.
                m_cubeRepresentation[itr->first][i * 3] = m_cubeRepresentation[itr->second][8 - i * 3];
            }
            else if (iteration == 3) { // White -> Green
                // Use the original green face instead of our recalcuated face
                m_cubeRepresentation[itr->first][i * 3 + 2] = greenFace[i];
            }
            else {
                // Otherwise, do the base operation.
                m_cubeRepresentation[itr->first][i * 3 + 2] = m_cubeRepresentation[itr->second][i * 3 + 2];
            }
        }
    }

    // Finally, the Red side needs to be rotated 90 degrees clockwise.
    this->rotateFinalFace(3); // Red face = 3
}

void RubiksCube::U() {
    // Map for how to swap pieces around for a U move.
    std::list<std::pair<int, int>> turnMap ({
        std::make_pair(2, 3), // Green -> Red
        std::make_pair(3, 4), // Red -> Blue
        std::make_pair(4, 1), // Blue -> Orange
        std::make_pair(1, 2)  // Orange -> Green
    });

    // Like in R, we'll need to store the Green face to refer back to it later.
    // Since we're editing the top row (pieces 0 - 2), store those values
    uint8_t greenFace[3];
    greenFace[0] = m_cubeRepresentation[2][0];
    greenFace[1] = m_cubeRepresentation[2][1];
    greenFace[2] = m_cubeRepresentation[2][2];

    // The last iteration has to use greenFace, so keep track of what iteration we're on here
    int iteration = 0;

    for (listitr_t itr = turnMap.begin(); itr != turnMap.end(); ++itr, ++iteration) {
        for (int i = 0; i < 3; ++i) {
            if (iteration == 3) {
                // Last iteration, use the original Green face instead of our recalcuated face
                m_cubeRepresentation[itr->first][i] = greenFace[i];
            }
            else {
                // Base operation: swap around the pieces directly. Nothing fancy needs to be done as all pieces are swapped in the same manner here
                m_cubeRepresentation[itr->first][i] = m_cubeRepresentation[itr->second][i];
            }
        }
    }

    // White face needs to be rotated by 90 degrees clockwise, do that final calculation here.
    this->rotateFinalFace(0); // White face = 0
}

void RubiksCube::F() {
    // Doing an F move is tricker than an R or U, since the pieces (by index) we move are different for each face.
    // For that reason, we compute the resulting state manually instead of using an iterator to help.
    
    // First store the state of the white face before turning it for later
    uint8_t whiteFace[3];
    whiteFace[0] = m_cubeRepresentation[0][6];
    whiteFace[1] = m_cubeRepresentation[0][7];
    whiteFace[2] = m_cubeRepresentation[0][8];

    // Replace White (pieces 6 - 8) with Orange (pieces 8, 5, and 2, reverse order)
    for (int i = 0; i < 3; ++i) {
        m_cubeRepresentation[0][i + 6] = m_cubeRepresentation[1][8 - i * 3];
    }

    // Replace Orange (pieces 2, 5, and 8) with Yellow (pieces 0 - 2);
    for (int i = 0; i < 3; ++i) {
        m_cubeRepresentation[1][i * 3 + 2] = m_cubeRepresentation[5][i];
    }

    // Replace Yellow (pieces 0 - 2) with Red (pieces 6, 3, and 0, reverse order)
    for (int i = 0; i < 3; ++i) {
        m_cubeRepresentation[5][i] = m_cubeRepresentation[3][6 - i * 3];
    }

    // Replace Red (pieces 0, 3, and 6) with White (pieces 6 - 8)
    // Since we already overwrote the White face with the Orange face, we need to use our cached whiteFace variable
    for (int i = 0; i < 3; ++i) {
        m_cubeRepresentation[3][i * 3] = whiteFace[i];
    }

    // We also have to account for the Green face rotating while doing an F.
    this->rotateFinalFace(2); // Green face = 2
}

void RubiksCube::L() {
    // This case has the same faces altered as R, but with the direction reversed.
    // We also have to work with the lhs of the cube instead of the rhs (with the exception of the Blue face).
    std::list<std::pair<int, int>> turnMap ({
        std::make_pair(0, 4), // White -> Blue
        std::make_pair(4, 5), // Blue -> Yellow
        std::make_pair(5, 2), // Yellow -> Green
        std::make_pair(2, 0)  // Green -> White
    });

    // We'll have to overwrite and later reference the White face, so make sure to cache the lhs here.
    uint8_t whiteFace[3];
    whiteFace[0] = m_cubeRepresentation[0][0];
    whiteFace[1] = m_cubeRepresentation[0][3];
    whiteFace[2] = m_cubeRepresentation[0][6];

    // Keeps track of what iteration of the loop we're on.
    int iteration = 0;

    for (listitr_t itr = turnMap.begin(); itr != turnMap.end(); ++itr, ++iteration) {
        for (int i = 0; i < 3; ++i) {
            if (iteration == 0) { // White -> Blue
                // The Blue face has to turn its rhs instead of lhs (unlike the others)
                // Blue pieces go in descending order instead of ascending order, so reverse the result
                m_cubeRepresentation[itr->first][i * 3] = m_cubeRepresentation[itr->second][8 - i * 3];
            }
            else if (iteration == 1) { // Blue -> Yellow
                // Same idea as with the first iteration, but overwriting the Blue face instead of overwriting with the Blue face.
                m_cubeRepresentation[itr->first][i * 3 + 2] = m_cubeRepresentation[itr->second][6 - i * 3];
            }
            else if (iteration == 3) { // Green -> White
                // We already overwrote the White face, use our cached pieces in place
                m_cubeRepresentation[itr->first][i * 3] = whiteFace[i];
            }
            else {
                // Base formula, move around the lhs pieces.
                m_cubeRepresentation[itr->first][i * 3] = m_cubeRepresentation[itr->second][i * 3];
            }
        }
    }

    // Make sure to also rotate the Orange face 90 degrees counterclockwise
    this->rotateFinalFace(1); // Orange face = 1
}

void RubiksCube::D() {
    // This case is similar to U, turning the same faces but in reverse.
    std::list<std::pair<int, int>> turnMap ({
        std::make_pair(2, 1), // Green -> Orange
        std::make_pair(1, 4), // Orange -> Blue
        std::make_pair(4, 3), // Blue -> Red
        std::make_pair(3, 2)  // Red -> Green
    });

    // Cache the Green face, so we use the original values while turning instead of overwritten values.
    uint8_t greenFace[3];
    greenFace[0] = m_cubeRepresentation[2][6];
    greenFace[1] = m_cubeRepresentation[2][7];
    greenFace[2] = m_cubeRepresentation[2][8];

    // Iteration keeps track of when we need to use greenFace instead of swapping values in m_cubeRepresentation
    int iteration = 0;

    for (listitr_t itr = turnMap.begin(); itr != turnMap.end(); ++itr, ++iteration) {
        for (int i = 0; i < 3; ++i) {
            if (iteration == 3) { // Red -> Green
                // Make sure to use our cached values instead of the overwritten values on the last iteration
                m_cubeRepresentation[itr->first][i + 6] = greenFace[i];
            }
            else {
                // Base formula, swap the bottom row
                m_cubeRepresentation[itr->first][i + 6] = m_cubeRepresentation[itr->second][i + 6];
            }
        }
    }

    // Make sure to rotate the Yellow face at the end.
    this->rotateFinalFace(5); // Yellow face = 5
}

void RubiksCube::B() {
    // B is like F in that each face has different pieces moved.
    // So, like in F(), we take each case manually instead of in a nested for loop.

    // We need to cache the white face, since that'll be overwritten and we'll need it later
    // Store the top row in reverse, so that the right pieces are moved around later on
    uint8_t whiteFace[3];
    whiteFace[0] = m_cubeRepresentation[0][2];
    whiteFace[1] = m_cubeRepresentation[0][1];
    whiteFace[2] = m_cubeRepresentation[0][0];
    
    // White (pieces 0 - 2) -> Red (pieces 2, 5, and 8)
    for (int i = 0; i < 3; ++i) {
        m_cubeRepresentation[0][i] = m_cubeRepresentation[3][i * 3 + 2];
    }

    // Red (pieces 2, 5, and 8) -> Yellow (pieces 8 - 6, reversed order)
    for (int i = 0; i < 3; ++i) {
        m_cubeRepresentation[3][i * 3 + 2] = m_cubeRepresentation[5][8 - i];
    }

    // Yellow (pieces 6 - 8) -> Orange (pieces 0, 3, and 6)
    for (int i = 0; i < 3; ++i) {
        m_cubeRepresentation[5][i + 6] = m_cubeRepresentation[1][i * 3];
    }

    // Orange (pieces 0, 3, and 6) -> White (original values in whiteFace)
    for (int i = 0; i < 3; ++i) {
        m_cubeRepresentation[1][i * 3] = whiteFace[i];
    }

    // Update the Blue face to be rotated clockwise as a result of the turn
    this->rotateFinalFace(4); // Blue face = 4
}

// Resets the Rubik's Cube's representation in memory (m_cubeRepresentation) to its default value, a solved cube.
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

// Updates the Rubik's Cube's representation in memory, m_cubeRepresentation, from the scramble in m_scramble.
// Overwrites whatever was in m_cubeRepresentation beforehand.
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

