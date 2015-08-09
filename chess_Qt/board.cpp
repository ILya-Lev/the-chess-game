#include "board.h"
#include <QTableView>
#include <QHeaderView>
#include <QHBoxLayout>
#include "chessmodel.h"
#include <QFont>
#include <QEvent>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QItemSelectionModel>
#include <QMessageBox>

CBoard::CBoard(QWidget* parent/* = nullptr*/)
	: QWidget(parent)
	, m_view(nullptr)
	, m_model(nullptr)
	, m_currentStatus(BeginningScreen)
{
	m_model = new CChessModel;
	connect(m_model, SIGNAL(notifyUi(const QString&)), this, SLOT(onNotificationFromModel(const QString&)));
	connect(m_model, SIGNAL(noFurtherStepsAvailable()), this, SIGNAL(noFurtherStepsAvailable()));
	connect(m_model, SIGNAL(noCloserStepsAvailable()), this, SIGNAL(noCloserStepsAvailable()));
	connect(m_model, SIGNAL(furtherStepsAvailable()), this, SIGNAL(furtherStepsAvailable()));
	connect(m_model, SIGNAL(closerStepsAvailable()), this, SIGNAL(closerStepsAvailable()));

	m_view = new QTableView;
	m_view->setModel(m_model);
	m_view->verticalHeader()->setDefaultSectionSize(64);
	m_view->horizontalHeader()->setDefaultSectionSize(64);
	m_view->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
	m_view->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
	m_view->setFont(QFont("times", 30));
	m_view->setSelectionBehavior(QAbstractItemView::SelectItems);
	m_view->setSelectionMode(QAbstractItemView::SingleSelection);
	m_view->setEditTriggers(QAbstractItemView::NoEditTriggers);

	QHBoxLayout* pLayout = new QHBoxLayout;
	pLayout->addWidget(m_view);
	setLayout(pLayout);

	m_view->installEventFilter(this);
}

CBoard::~CBoard()
{

}

void CBoard::LoadGame(const QStringList& slFigurePositions)
{
	m_model->loadPositions(slFigurePositions);
	m_currentStatus = LoadedGame;
	m_view->clearSelection();
}

QStringList CBoard::FiguresPositions()
{
	return m_model->figuresPositions();
}

bool CBoard::eventFilter(QObject* obj, QEvent* e)
{
	QTableView* view = qobject_cast<QTableView*>(obj);
	if (view != nullptr && m_currentStatus == NormalPlay)
	{
		// todo: fix this!
		if (e->type() == QEvent::MouseButtonPress)
		{
			m_model->setData(m_view->selectionModel()->currentIndex(), QVariant(), Qt::UserRole);
			return true;
		}
		else if (e->type() == QEvent::KeyPress)
		{
			QKeyEvent* keyEvent = static_cast<QKeyEvent*>(e);
			if (keyEvent->key() == Qt::Key_Space)
			{
				m_model->setData(m_view->selectionModel()->currentIndex(), QVariant(), Qt::UserRole);
				return true;
			}
		}
	}
	return false;
}

void CBoard::onStart()
{
	m_model->start();
	m_currentStatus = NormalPlay;
}

void CBoard::onStop()
{
	m_model->stop();
	m_currentStatus = BeginningScreen;
}

void CBoard::onNext()
{
	m_model->next();
}

void CBoard::onPrevious()
{
	m_model->previous();
}

void CBoard::onNotificationFromModel(const QString& message)
{
	QMessageBox::information(this, tr("Information"), message, QMessageBox::Ok);
}
