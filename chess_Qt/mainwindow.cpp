#include "mainwindow.h"
#include "board.h"
#include "buttonspanel.h"
#include <QDockWidget>
#include <QSizePolicy>
#include <QFileDialog>
#include <QMessageBox>
#include <QDateTime>
#include <QFile>
#include <QTextStream>

const QString CMainWindow::s_DefaultHistoryFolder("./GameHistory");

CMainWindow::CMainWindow(QWidget* parent/* = nullptr*/)
	: QMainWindow(parent)
	, m_pBoard(nullptr)
	, m_pButtonsPanel(nullptr)
{
	m_pBoard = new CBoard;
	m_pBoard->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	setCentralWidget(m_pBoard);

	m_pButtonsPanel = new CButtonsPanel;
	QDockWidget* pbtnPanel = new QDockWidget;
	pbtnPanel->setFeatures(QDockWidget::NoDockWidgetFeatures);
	pbtnPanel->setWidget(m_pButtonsPanel);
	pbtnPanel->setMaximumWidth(pbtnPanel->sizeHint().width());
	addDockWidget(Qt::RightDockWidgetArea, pbtnPanel);

	connect(m_pButtonsPanel, SIGNAL(start()), m_pBoard, SLOT(onStart()));
	connect(m_pButtonsPanel, SIGNAL(save()), this, SLOT(onSave()));
	connect(m_pButtonsPanel, SIGNAL(stop()), m_pBoard, SLOT(onStop()));
	connect(m_pButtonsPanel, SIGNAL(load()), this, SLOT(onLoad()));
	connect(m_pButtonsPanel, SIGNAL(previous()), m_pBoard, SLOT(onPrevious()));
	connect(m_pButtonsPanel, SIGNAL(next()), m_pBoard, SLOT(onNext()));

	connect(m_pBoard, SIGNAL(furtherStepsAvailable()), m_pButtonsPanel, SLOT(onFurtherStepsAvailable()));
	connect(m_pBoard, SIGNAL(closerStepsAvailable()), m_pButtonsPanel, SLOT(onCloserStepsAvailable()));
	connect(m_pBoard, SIGNAL(noFurtherStepsAvailable()), m_pButtonsPanel, SLOT(onNoFurtherStepsAvailable()));
	connect(m_pBoard, SIGNAL(noCloserStepsAvailable()), m_pButtonsPanel, SLOT(onNoCloserStepsAvailable()));

	setWindowTitle(tr("Chess Multi-play"));
	setMinimumSize(665, 560);
}

CMainWindow::~CMainWindow()
{

}

void CMainWindow::onLoad()
{
	const QString fileName = QFileDialog::getOpenFileName(this, tr("Select previously saved game"), s_DefaultHistoryFolder, "*.txt");
	if (!fileName.isEmpty())
	{
		try
		{
			QFile f(fileName);
			if (f.open(QIODevice::ReadOnly))
			{
				QTextStream in(&f);
				QStringList figuresPositions;
				while (!in.atEnd())
				{
					figuresPositions << in.readLine();
				}
				m_pBoard->LoadGame(figuresPositions);
				m_pButtonsPanel->gameLoaded();
			}
			else
			{
				QMessageBox::warning(this, tr("Loading game failed"), tr("Cannot open file %1 - loading failed").arg(fileName), QMessageBox::Ok);
			}
		}
		catch (...)
		{
			QMessageBox::critical(this, tr("exception on loading saved game")
				, tr("something bad happened, pull the cable out of charge net"), QMessageBox::Help);
			close();
		}
	}
}

void CMainWindow::onSave()
{
	const QString fileName = s_DefaultHistoryFolder + "/" + QDateTime::currentDateTimeUtc().toString(Qt::ISODate).replace(":", "-") + ".txt";
	QFile f(fileName);
	if (f.open(QIODevice::WriteOnly))
	{
		QTextStream out(&f);
		const QStringList figuresPositions = m_pBoard->FiguresPositions();
		for (QString aRow : figuresPositions)
		{
			out << aRow << "\n";
		}
	}
}
