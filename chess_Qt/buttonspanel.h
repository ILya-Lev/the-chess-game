#ifndef BUTTONSPANEL_H
#define BUTTONSPANEL_H

#include <QWidget>
#include "Statuses.h"

class QPushButton;

class CButtonsPanel : public QWidget
{
	Q_OBJECT

public:
	CButtonsPanel(QWidget* parent = nullptr);
	~CButtonsPanel();

	void gameLoaded();
	
public slots:
	void onFurtherStepsAvailable();
	void onCloserStepsAvailable();
	void onNoFurtherStepsAvailable();
	void onNoCloserStepsAvailable();

signals:
	void start();
	void save();
	void stop();
	void load();
	void previous();
	void next();

private slots:
	void onStart();
	void onStop();

private:
	QPushButton* m_btnStart;
	QPushButton* m_btnSave;
	QPushButton* m_btnStop;
	QPushButton* m_btnLoad;
	QPushButton* m_btnPrevious;
	QPushButton* m_btnNext;

	Statuses m_currentStatus;

	void createUI();
	void synchronizeToState();
};

#endif // BUTTONSPANEL_H
