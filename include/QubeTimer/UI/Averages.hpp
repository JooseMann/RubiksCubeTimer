#ifndef AVERAGES_HPP
#define AVERAGES_HPP

#include <Qt>
#include <QLabel>
#include <QString>

#include "types.hpp"

namespace UI {

class Averages : public QWidget {
Q_OBJECT

public:
	explicit Averages(QWidget* parent = nullptr);

	// QLabel* avgLabel
	QLabel* avgLabel() const { return m_avgLabel; }
	void updateAverages(const averages_t& avgs);

	// QFont font
	QFont font() const { return m_font; }
	
private:
	QLabel* m_avgLabel;
	QFont m_font;

	QString generateString(const averages_t& avgs);

signals:
public slots:

};

} // UI

#endif // AVERAGES_HPP
