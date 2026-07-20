#ifndef RUBIKSCUBE_HPP
#define RUBIKSCUBE_HPP

#include <cstdint>
#include <QString>

class RubiksCube {
public:
    /*
     * Initializes the class and generates a scramble between 20 and 30 moves long.
     */
    RubiksCube();
    ~RubiksCube();

    /*
     * The cube representation is an array of bytes that show what state the cube is in after a scramble.
     * This array is reset and shuffled when generating a scramble, as to match that scramble.
     */
    uint8_t** getCubeRepresentation() const { return m_cubeRepresentation; }

    /*
     * The raw scramble is the scramble in its binary representation. 
     * The string scramble is the scrambled translated from binary to text, as a human-readable string of moves.
     */
    uint8_t* getRawScramble() const { return m_scramble; };
    QString getStringScramble() const;

    int getScrambleLen() const { return m_scrambleLen; }
 
    void generateScramble(); // Generates a binary representation of a scramble.
private:
    uint8_t** m_cubeRepresentation; // Binary representation of the cube. See util/cube_representation.txt
    uint8_t* m_scramble; // Binary representation of the scramble. See util/move_representation.txt
    int m_scrambleLen; // Length of the scramble.

    // Individual moves for our cube representation.
    void R();
    void U();
    void F();
    void L();
    void D();
    void B();

    void resetCubeRepresentation(); // Resets the cube representation to as if it were solved.
    void updateCubeRepresentation(); // Updates the cube representation in accordance with the current scramble.
};

#endif
