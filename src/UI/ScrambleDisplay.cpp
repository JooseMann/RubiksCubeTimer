#include <cstdint>

#include <QPainter>
#include <QWidget>

#include "UI/ScrambleDisplay.hpp"

namespace UI {

ScrambleDisplay::ScrambleDisplay(QWidget* parent) : QWidget(parent) {
    // Set the display to be active initially
    m_active = true;

    // Set a minimum size (30% width, 30% height of the window -- 800 by 600)
    this->setMinimumSize(240, 180);
}

// Updates the scramble display with the provided cube representation.
void ScrambleDisplay::showScramble(uint8_t** cubeRepresentation) {
    m_currentCubeRep = cubeRepresentation;
    m_active = true;

    // Update the display to properly show the new scramble.
    this->update();
}

// Hides the scramble display by setting a "not active" flag.
void ScrambleDisplay::hideScramble() {
    m_active = false;

    // Update the display to hide whatever was drawn previously.
    this->update();
}

void ScrambleDisplay::paintEvent(QPaintEvent* paintEvent) {
    // If we don't want to show the display, just return immediately and draw nothing (effectively hiding the display).
    if (!m_active) return;

    // The scramble will be displayed within our 240 x 180 px (minimum) display area 
    // It will be 9 pieces (3 faces) tall and 12 pieces (4 faces) long, plus additional space between cube faces.

    // Declare useful helper variables
    int width = this->width();
    int height = this->height();

    float pieceWidth = width / 10.5f; // Width of one piece
    float pieceHeight = height / 10.5f; // Height of one piece

    // Take the smaller value between the width and height and make them equal (so that our pieces are squares)
    if (pieceWidth > pieceHeight) {
        pieceWidth = pieceHeight;
    }
    else {
        pieceHeight = pieceWidth;
    }

    // Horizontal and vertical padding between faces
    float hPadding = width / 60.0f;
    float vPadding = height / 40.0f;

    // Offsets to center our drawing within our frame
    float xOffset = (width - (12 * pieceWidth + 4 * hPadding))  / 2.0f; // Furthest x position is 12 * pieceWidth + 4 * hPadding
    float yOffset = (height - (9 * pieceHeight + 3 * vPadding)) / 2.0f; // Furthest y position is 9 * pieceHeight + 3 * vPadding

    // Current x and y positions
    // Initially, x is offset one face to the right (pieceWidth + hPadding), then both x and y have initial padding
    float x = (pieceWidth * 3 + 2 * hPadding) + xOffset;
    float y = vPadding + yOffset;

    // Color of the piece we'll draw
    QColor currentColor;

    // Painter to draw the pieces with
    QPainter painter (this);

    // Activate the painter (if it isn't already)
    if (!painter.isActive()) painter.begin(this);

    // Set the brush to fill the rectangle instead of only drawing the outline
    painter.setBrush(Qt::SolidPattern);

    // Set the pen (outline)'s color to black, so we can outline the colors in pure black.
    painter.setPen(QColor::fromRgb(0, 0, 0));

    // Fill the background of our widget with a dark gray
    painter.fillRect(0, 0, width, height, QColor::fromRgb(71, 71, 71));

    // Loop over every face
    for (int face = 0; face < 6; ++face) {
        for (int piece = 0; piece < 9; ++piece) {
            uint8_t color = m_currentCubeRep[face][piece] % 0x20;

            // Set the paint brush's color, depending on what piece we're drawing.
            switch (color) {
                case 0x00: // White piece
                    currentColor = QColor::fromRgb(255, 255, 255);
                    break;
                case 0x01: // Orange piece
                    currentColor = QColor::fromRgb(226, 152, 72);
                    break;
                case 0x02: // Green piece
                    currentColor = QColor::fromRgb(68, 232, 97);
                    break;
                case 0x04: // Red piece
                    currentColor = QColor::fromRgb(232, 72, 68);
                    break;
                case 0x08: // Blue piece
                    currentColor = QColor::fromRgb(57, 59, 211);
                    break;
                case 0x10: // Yellow piece
                    currentColor = QColor::fromRgb(216, 232, 78);
                    break;
                default: // Black piece in case of error
                    currentColor = QColor::fromRgb(0, 0, 0);
                    break;
            }

            // Draw a (black) border outline
            painter.drawRect(x, y, pieceWidth, pieceHeight);

            // Draw the piece as a filled rectangle
            painter.fillRect(x + 1, y + 1, pieceWidth - 2, pieceHeight - 2, currentColor);

            // Move to the next row if needed
            if (piece % 3 == 2) {
                x -= 2 * pieceWidth; // Restore x to its original position
                y += pieceHeight;
            }
            // Otherwise, just advance to the next piece to our right.
            else {
                x += pieceWidth;
            }
        }

        // Move onto the next set of lines
        if (face == 0) {
            x -= 3 * pieceWidth + hPadding;
            y += vPadding; // y is just past the bottom row
        }
        // Move onto the last set of lines
        else if (face == 4) {
            x -= 6 * pieceWidth + 2 * hPadding;
            y += vPadding;
        }
        // Generic case
        else {
            x += 3 * pieceWidth + hPadding; // Next face to the right
            y -= 3 * pieceHeight; // Restore y position to what it was before this face
        }
    }

    // Tell our painter that we're done painting here.
    painter.end();
}

} // UI


