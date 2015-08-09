#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class CBoard;
class CButtonsPanel;

class CMainWindow : public QMainWindow
{
	Q_OBJECT

public:
	CMainWindow(QWidget* parent = nullptr);
	~CMainWindow();

private slots:
	void onLoad();
	void onSave();

private:
	CBoard* m_pBoard;
	CButtonsPanel* m_pButtonsPanel;
	
	static const QString s_DefaultHistoryFolder;
};

#endif // MAINWINDOW_H
