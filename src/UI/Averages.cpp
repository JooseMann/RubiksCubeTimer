#include <Qt>
#include <QLabel>
#include <QString>

#include <format>
#include <iomanip>
#include <string>
#include <sstream>

#include "types.hpp"
#include "UI/Averages.hpp"

namespace UI {

Averages::Averages(QWidget* parent) : QWidget(parent) {
	m_avgLabel = new QLabel("Ao5     | N/A\nAo12    | N/A\nAo100   | N/A\nAverage | N/A\n", this);
	
	m_font = QFont("Calibri", 14);

	m_avgLabel->setFont(m_font);
}

// Publicly facing function to update the label with new averages.
// The averages themselves are calculated via functions in the SolveManager class.
void Averages::updateAverages(const averages_t& avgs) {
	// Generate a new string with our updated averages
	QString str = generateString(avgs);

	// Update our label's text with this new string
	m_avgLabel->setText(str);
}

/*
 * Generate a formatted string.
 * We create a string with the format: 
 * "Ao5     | 1.00
    Ao12    | 2.00
    Ao100   | 3.00
    Average | 4.00"
 * (With 1.00, 2.00, 3.00, and 4.00 as dummy values representing the corresponding average).
 */
QString Averages::generateString(const averages_t& avgs) {
	QString qStr ("");

	// Check if this is a valid number (-1.0 is N/A)
	// Same goes for all other averages
	if (avgs.ao5 > -1.0) {
		// Formatted as "Ao5 [Spaces] | [avgs.ao5]", where the number of spaces makes the "|" line up for each line
		// Same for other averages
		qStr += QString(("Ao5     | " + QString::number(avgs.ao5, 'f', 2).toStdString() + "\n").c_str());
	}
	else {
		qStr += QString(std::string("Ao5     | N/A\n").c_str());
	}

	if (avgs.ao12 > -1.0) {
		qStr += QString(("Ao12    | " + QString::number(avgs.ao12, 'f', 2).toStdString() + "\n").c_str());
	}
	else {
		qStr += QString(std::string("Ao12    | N/A\n").c_str());
	}

	if (avgs.ao100 > -1.0) {
		qStr += QString(("Ao100   | " + QString::number(avgs.ao100, 'f', 2).toStdString() + "\n").c_str());
	}
	else {
		qStr += QString(std::string("Ao100   | N/A\n").c_str());
	}

	if (avgs.average > -1.0) {
		qStr += QString(("Average | " + QString::number(avgs.average, 'f', 2).toStdString()).c_str());
	}
	else {
		qStr += QString(std::string("Average | N/A\n").c_str());
	}

	return qStr;
}

} // UI
