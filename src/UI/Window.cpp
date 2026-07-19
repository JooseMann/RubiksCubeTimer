#include <cstring> // std::memcpy
#include <thread>
#include <unistd.h> // usleep(int)

#include <Qt>
#include <QFont>
#include <QGridLayout>
#include <QLabel>
#include <QRect>
#include <QWidget>

#include "RubiksCube.hpp"
#include "SolveManager.hpp"
#include "UI/Averages.hpp"
#include "UI/Timer.hpp"
#include "UI/Window.hpp"

namespace UI {

Window::Window(const char* filename, QWidget* parent) : QWidget(parent) {
	// First define the layout
	m_layout = new QGridLayout(this);

	// Then define the rest of the variables
	m_cube = new RubiksCube();
    m_solveManager = new SolveManager(filename);
	m_scrambleLabel = new QLabel(m_cube->getStringScramble(), this);
	m_font = QFont();
	m_timer = new Timer(this);
	m_averages = new Averages(this);
	m_keyEvent = new QKeyEvent(QEvent::KeyRelease, Qt::Key_Space, Qt::NoModifier);

	// Additional variables for handling the timer thread
	m_running = true;
	m_timerThread = std::thread(timerHandler, this);

	// Set the minimum size to 800x600 (though, ideally it'd be larger)
	setMinimumSize(800, 600);
	
	// Setup all of the UI elements
	setupLayout();
	setupFont();
	setupScrambleLabel();
	setupTimer();

	// Setup our averages label
	m_averages->updateAverages(m_solveManager->averages());
}

Window::~Window() {
	// In the destructor, delete the pointers that aren't deleted by Qt automatically
	delete m_cube;
	delete m_solveManager;
	delete m_keyEvent;

	// Now stop the timer thread
	m_running = false;
	m_timerThread.join();
}

void Window::keyPressEvent(QKeyEvent* event) {
	// First check whether the timer is currently active or inactive.
	// If active, then we are stopping the timer and need to generate a new scramble.
	// If inactive, then we are starting the timer and need to reset it before marking it as active.

	// Stopping timer -> write our solve data to a .csv, update our averages, and generate a new scramble
	if (m_timer->active()) { 
		// Create a data solve_t

		// Solve time
		double time = m_timer->time();

		// Current date
		std::time_t date = std::time(NULL);

		// Copy of the current scramble
		uint8_t* scramble = new uint8_t[30];
		std::memcpy(scramble, m_cube->getRawScramble(), 30); // Scramble is always (max) 30 moves, each move is 1 byte -> 30 bytes

		solve_t solve = { time, std::string(""), scramble, date }; // Comment is blank by default

		// Record the solve to our .csv file
		m_solveManager->addSolve(solve);

		// Update our averages
		m_averages->updateAverages(m_solveManager->averages());

		// Generate and display a new scramble
		m_cube->generateScramble();
		m_scrambleLabel->setText(m_cube->getStringScramble());
	}

	// Starting timer -> reset timer to 0.00 and update absolute time to start measuring elapsed time
	if (!m_timer->active()) {
		m_timer->resetTimer();
		m_timer->updateAbsoluteTime();
	}

	// Mark the timer as active
	// This will cause m_timerThread to start / stop incrementing the timer
	m_timer->toggleActive();
}

// Set the default layout settings
void Window::setupLayout() {
	
	// Get screen width and height for future reference
	int screenWidth = this->geometry().width();
	int screenHeight = this->geometry().height();

	// Set the basic properties of the layout
	// Takes up the whole screen as one big grid, and has no spacing
	m_layout->setGeometry(QRect(0, 0, screenWidth, screenHeight));
	m_layout->setHorizontalSpacing(0); // No horizontal spacing between columns
	m_layout->setVerticalSpacing(0); // No vertical spacing between rows

	// Add our widgets to the layout
	// Scramble is top-center, timer is in the exact center, averages are bottom-left
	m_layout->addWidget(m_scrambleLabel, 0, 0, 1, 10, Qt::AlignHCenter); // Also creates 10 columns
	m_layout->addWidget(m_timer, 2, 0, 6, 10, Qt::AlignCenter);
	m_layout->addWidget(m_averages->avgLabel(), 6, 0, 10, 2, Qt::AlignLeft | Qt::AlignBottom); // Also creates 10 rows

	// Disallow the rows and columns from stretching
	// Also set each of their sizes to 1/10th of the screen's width or height, respectively
	// This way, the rows and columns always exactly fit the screen
	for (int i = 0; i < 10; ++i) {
		// Safety if statement, break out early if the row/column count wasn't set correctly (should be exactly 10)
		if (i >= m_layout->columnCount() || i >= m_layout->rowCount()) break;

		m_layout->setColumnStretch(i, 0);
		m_layout->setRowStretch(i, 0);

		m_layout->setColumnMinimumWidth(i, screenWidth / 10);
		m_layout->setRowMinimumHeight(i, screenHeight / 10);
	}

}

// Set default properties for m_scrambleLabel
void Window::setupScrambleLabel() {
	// Set the font and alignment of the label
	m_scrambleLabel->setFont(m_font);
	m_scrambleLabel->setAlignment(Qt::AlignHCenter | Qt::AlignTop); // Align top-center
}

// Set default properties for m_font
void Window::setupFont() {
	m_font.setFamily("Calibri");
	m_font.setPointSize(22);
}

// Set the default properties for m_timer
void Window::setupTimer() {

	// Screen width and height, for convenience
	int screenWidth = this->geometry().width();
	int screenHeight = this->geometry().height();

	// Set the minimum size to account for the rows and columns we give it, so it fills the whole space
	// 10 / 10 columns and 6 / 10 rows -> (width, 0.6 * height)
	m_timer->setMinimumSize(screenWidth, 6 * (screenHeight / 10.0));
	m_timer->updateTimerLabelGeometry(); // Update the label for the timer accordingly
}

// Refresh the internal scramble
void Window::refreshScramble() {
	// To refresh the internal scramble, just recall generateScramble() to overwrite with a new scramble
	m_cube->generateScramble();
}

void timerHandler(Window* window) {
	// While the program is running, check to see if we should increment the timer.
	// If so, then go ahead. Otherwise, just keep checking.
	while (window->running()) {
		// If the timer is active, then increment the timer
		if (window->timer()->active()) window->timer()->tick();

		// Wait 0.01 seconds
        usleep(10000);
	}
}

} // UI
