#include <Qt>
#include <QApplication>
#include <QDebug>
#include <QFont>
#include <QLabel>

#include "RubiksCube.hpp"
#include "UI/Window.hpp"

int main(int argc, char* argv[]) {
    // Set up randomness in advance
    srand(time(nullptr));

    // Base application
    QApplication app (argc, argv);
    
    // Create the window
    // Handles making the rubiks cube, its scramble, and the solve manager in the constructor
    UI::Window* window = new UI::Window("data/session0.csv");

    // Show the window when running the app (below) 
    window->show();

    // Run the app
    int rc = app.exec();

    // Cleanup
    delete window;

    return rc;
}
