#include "buttonspanel.h"
#include <QPushButton>
#include <QVBoxLayout>

CButtonsPanel::CButtonsPanel(QWidget* parent/* = nullptr*/)
	: QWidget(parent)
	, m_btnStart(nullptr)
	, m_btnSave(nullptr)
	, m_btnStop(nullptr)
	, m_btnLoad(nullptr)
	, m_btnPrevious(nullptr)
	, m_btnNext(nullptr)
	, m_currentStatus(BeginningScreen)
{
	createUI();
	synchronizeToState();

	connect(m_btnStart, SIGNAL(clicked()), this, SLOT(onStart()));
	connect(m_btnStop, SIGNAL(clicked()), this, SLOT(onStop()));

	connect(m_btnStart, SIGNAL(clicked()), this, SIGNAL(start()));
	connect(m_btnStop, SIGNAL(clicked()), this, SIGNAL(stop()));
	connect(m_btnSave, SIGNAL(clicked()), this, SIGNAL(save()));
	connect(m_btnLoad, SIGNAL(clicked()), this, SIGNAL(load()));
	connect(m_btnPrevious, SIGNAL(clicked()), this, SIGNAL(previous()));
	connect(m_btnNext, SIGNAL(clicked()), this, SIGNAL(next()));
}

CButtonsPanel::~CButtonsPanel()
{

}

void CButtonsPanel::gameLoaded()
{
	m_currentStatus = LoadedGame;
	synchronizeToState();
}

void CButtonsPanel::onFurtherStepsAvailable()
{
	m_btnNext->setEnabled(true);
}

void CButtonsPanel::onCloserStepsAvailable()
{
	m_btnPrevious->setEnabled(true);
}

void CButtonsPanel::onNoFurtherStepsAvailable()
{
	m_btnNext->setDisabled(true);
}

void CButtonsPanel::onNoCloserStepsAvailable()
{
	m_btnPrevious->setDisabled(true);
}

void CButtonsPanel::onStart()
{
	m_currentStatus = NormalPlay;
	synchronizeToState();
}

void CButtonsPanel::onStop()
{
	m_currentStatus = BeginningScreen;
	synchronizeToState();
}

void CButtonsPanel::createUI()
{
	m_btnStart = new QPushButton(tr("&Start"));
	m_btnSave = new QPushButton(tr("&Save"));
	m_btnStop = new QPushButton(tr("&Stop"));
	m_btnLoad = new QPushButton(tr("&Load"));
	m_btnPrevious = new QPushButton(tr("&Previous"));
	m_btnNext = new QPushButton(tr("&Next"));

	QVBoxLayout* pLayout = new QVBoxLayout;
	pLayout->addWidget(m_btnStart);
	pLayout->addWidget(m_btnSave);
	pLayout->addWidget(m_btnStop);
	pLayout->addWidget(m_btnLoad);
	pLayout->addStretch();
	pLayout->addWidget(m_btnPrevious);
	pLayout->addWidget(m_btnNext);

	this->setLayout(pLayout);
}

void CButtonsPanel::synchronizeToState()
{
	m_btnStart->setVisible(m_currentStatus != NormalPlay);
	m_btnSave->setVisible(m_currentStatus == NormalPlay);
	m_btnStop->setVisible(m_currentStatus == NormalPlay);
	m_btnLoad->setVisible(m_currentStatus != NormalPlay);
	m_btnPrevious->setVisible(m_currentStatus == LoadedGame);
	m_btnNext->setVisible(m_currentStatus == LoadedGame);
}
