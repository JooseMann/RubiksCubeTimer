#ifndef SCRAMBLE_DISPLAY_HPP
#define SCRAMBLE_DISPLAY_HPP

#include <cstdint>
#include <QWidget>

namespace UI {

class ScrambleDisplay : public QWidget {
Q_OBJECT

public:
    // Base constructor
    explicit ScrambleDisplay(QWidget* parent = nullptr);

    // Methods to show and hide the scramble display
    void showScramble(uint8_t** cubeRepresentation);
    void hideScramble();

protected:
    // Qt override to draw our scramble whenever we call update()
    void paintEvent(QPaintEvent*) override;

private:
    uint8_t** m_currentCubeRep; // Current cube representation, taken from a RubiksCube instance directly
    bool m_active; // Whether the display is currently being shown or not.
};

} // UI

#endif // SCRAMBLE_DISPLAY_HPP
