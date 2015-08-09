#ifndef BOARD_H
#define BOARD_H

#include <QWidget>
#include <QStringList>
#include "Statuses.h"

class QTableView;
class CChessModel;

class CBoard : public QWidget
{
	Q_OBJECT

public:
	CBoard(QWidget* parent = nullptr);
	~CBoard();
	void LoadGame(const QStringList& slFigurePositions);
	QStringList FiguresPositions();

	virtual bool eventFilter(QObject *, QEvent *) override;

signals:
	void closerStepsAvailable();
	void furtherStepsAvailable();
	void noCloserStepsAvailable();
	void noFurtherStepsAvailable();

public slots:
	void onStart();
	void onStop();
	void onNext();
	void onPrevious();

private slots:
	void onNotificationFromModel(const QString& message);

private:
	QTableView* m_view;
	CChessModel* m_model;
	Statuses m_currentStatus;
};

#endif // BOARD_H
