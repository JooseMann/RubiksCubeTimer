#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <thread>

#include <Qt>
#include <QFont>
#include <QGridLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QWidget>

#include "RubiksCube.hpp"
#include "SolveManager.hpp"
#include "UI/Averages.hpp"
#include "UI/Timer.hpp"

namespace UI {

class Window : public QWidget {
Q_OBJECT

public: 
	explicit Window(const char* filename = "session0.csv", QWidget* parent = nullptr); // filename refers to the .csv file that contains our solve data
	~Window();

	// Qt overrides
	void keyPressEvent(QKeyEvent* event);

	// QGridLayout* layout
	QGridLayout* layout() const { return m_layout; }
	void setupLayout();

	// QFont font
	QFont font() const { return m_font; }
	void setFont(QFont font) { m_font = font; m_scrambleLabel->setFont(m_font); }
	void setupFont();

	// QLabel* scrambleLabel
	QLabel* scrambleLabel() const { return m_scrambleLabel; }
	void setScrambleLabel(QLabel* label) { m_scrambleLabel = label; }
	void setupScrambleLabel();

	// Averages* averages
	Averages* averages() const { return m_averages; }
	void setAverages(Averages* averages) { m_averages = averages; }

	// Timer* timer
	Timer* timer() const { return m_timer; }
	void incrementTimer() { m_timer->tick(); }
	void setupTimer();

	// bool running
	bool running() const { return m_running; }

	// Reset the scramble
	void refreshScramble();

private:
	// UI
	QGridLayout* m_layout; // Grid layout of the window
	QFont m_font; // The scramble's font
	QLabel* m_scrambleLabel; // Label that contains the scramble, to be shown on the screen
	Averages* m_averages; // Label containing information about our solve averages
	Timer* m_timer; // Timer to be shown on screen

	// Related UI
	QKeyEvent* m_keyEvent; // Handles watching for space to start / stop the timer

	// Other
	RubiksCube* m_cube; // Our representation of the Rubiks cube
	SolveManager* m_solveManager; // Manager to hold data on our solves over time.
	bool m_running; // True while the program is running, false once it closes. Used to help manage threads.
	std::thread m_timerThread; // The thread that handles incrementing the timer while it is active. 

signals:
public slots:

};

// Helper functions
void timerHandler(Window* window);

} // UI

#endif // WINDOW_HPP
