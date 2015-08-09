#include "chessmodel.h"
#include <QColor>
#include <QBrush>
#include <QFont>
#include <algorithm>
#include <iterator>

CChessModel::CChessModel(QObject* parent/* = nullptr*/)
	: QAbstractTableModel(parent)
	, m_currentStepIndex(-1)
{
	stop();
}

CChessModel::~CChessModel()
{

}

int CChessModel::rowCount(const QModelIndex &/*parent = QModelIndex()*/) const
{
	return SIZE;
}

int CChessModel::columnCount(const QModelIndex &/*parent = QModelIndex()*/) const
{
	return SIZE;
}

QVariant CChessModel::data(const QModelIndex &index, int role /*= Qt::DisplayRole*/) const
{
	if (role == Qt::BackgroundColorRole)
	{
		return (index.row() + index.column()) % 2 ? QColor(Qt::lightGray) : QColor(Qt::black);
	}
	if (role == Qt::DisplayRole)
	{
		const QString cellValue = m_cells[index.row()*SIZE + index.column()];
		if (cellValue.isEmpty())
		{
			return QVariant();
		}
		return QString(cellValue.at(1));
	}
	if (role == Qt::ForegroundRole)
	{
		const QString cellValue = m_cells[index.row()*SIZE + index.column()];
		if (cellValue.isEmpty())
		{
			return QVariant();
		}
		QBrush brush(cellValue.at(0) == 'B' ? Qt::green : Qt::red);
		return brush;
	}
	if (role == Qt::TextAlignmentRole)
	{
		return Qt::AlignCenter;
	}
	if (role == Qt::ToolTipRole)
	{
		return QVariant();
	}

	return QVariant(); 
}

bool CChessModel::setData(const QModelIndex &index, const QVariant &value, int role /*= Qt::EditRole*/)
{
	Q_UNUSED(value);
	if (role == Qt::UserRole)
	{
		if (!m_firstClick.isValid())
		{
			if (containsFigure(index))
			{
				if (m_cells[index.row()*SIZE + index.column()].at(0) != m_previousMoveQueue)
				{
					m_firstClick = index;
				}
				else
					emit notifyUi(tr("it's not your turn! Let %1 make move.").arg(m_previousMoveQueue == "B" ? "white" : "black"));
			}
		}
		else
		{
			if (index != m_firstClick && isValidMovement(index) && (!containsFigure(index) || containsOppositeFigure(index)))
			{
				m_cells[index.row()*SIZE + index.column()] = m_cells[m_firstClick.row()*SIZE + m_firstClick.column()];
				m_cells[m_firstClick.row()*SIZE + m_firstClick.column()] = "";
				emit dataChanged(m_firstClick, m_firstClick);
				emit dataChanged(index, index);
				m_firstClick = QModelIndex();
				m_previousMoveQueue = m_cells[index.row()*SIZE + index.column()].at(0);

				storeGameState();
				// todo: check whether checkmate has been done
				return true;
			}
			emit notifyUi(tr("you attempted to make invalid move. figure selection cleared. try again!"));
			m_firstClick = QModelIndex();
		}
	}
	return false;
}

QVariant CChessModel::headerData(int section, Qt::Orientation orientation, int role /*= Qt::DisplayRole*/) const
{
	if (role == Qt::BackgroundColorRole)
	{
		return QColor(105, 250, 105);
	}
	if (role == Qt::DisplayRole)
	{
		if (orientation == Qt::Horizontal)
		{
			return QChar(section + 'A');
		}
		else
			return section + 1;
	}
	return QVariant();
}

QStringList CChessModel::figuresPositions() const
{
	QStringList gameStates;
	for (auto aState : m_gameStates)
	{
		QString oneStepState = "";
		for (int row = 0; row < SIZE; ++row)
		{
			for (int col = 0; col < SIZE; ++col)
			{
				const QString cellValue = aState[row*SIZE + col];
				if (!cellValue.isEmpty())
				{
					oneStepState += QString("%1%2%3").arg(cellValue, QString::number(row), QString::number(col));
				}
			}
		}
		gameStates << oneStepState;
	}
	
	return gameStates;
}

void CChessModel::loadPositions(const QStringList& slPositions)
{
	stop();
	for (QString aState : slPositions)
	{
		std::for_each(std::begin(m_cells), std::end(m_cells), [](QString& str) {str.clear(); });
		for (int i = 0; i < aState.size(); i += 4)
		{
			const QString aCell = aState.mid(i, 4);
			int row = aCell.mid(2, 1).toInt();
			int col = aCell.mid(3, 1).toInt();
			m_cells[row*SIZE + col] = aCell.mid(0, 2);
		}
		storeGameState();
	}
	next();
	emit noCloserStepsAvailable();
}

void CChessModel::start()
{
	stop();
	// black figures are on top
	m_cells[0] = m_cells[7] = "BR";		// Rook
	m_cells[1] = m_cells[6] = "BN";		// Knight, because of King
	m_cells[2] = m_cells[5] = "BB";		// Bishop
	m_cells[4] = "BQ";					// Queen
	m_cells[3] = "BK";					// King
	for (int i = 0; i < SIZE; ++i)
	{
		m_cells[SIZE * 1 + i] = "BP";		//Pawn
	}

	m_cells[7 * 8 + 0] = m_cells[7 * 8 + 7] = "WR";
	m_cells[7 * 8 + 1] = m_cells[7 * 8 + 6] = "WN";
	m_cells[7 * 8 + 2] = m_cells[7 * 8 + 5] = "WB";
	m_cells[7 * 8 + 3] = "WQ";
	m_cells[7 * 8 + 4] = "WK";
	for (int i = 0; i < SIZE; ++i)
	{
		m_cells[SIZE * (SIZE - 2) + i] = "WP";
	}
	storeGameState();
	emit dataChanged(index(0, 0), index(SIZE - 1, SIZE - 1));
}

void CChessModel::stop()
{
	m_gameStates.clear();
	m_currentStepIndex = -1;
	std::for_each(std::begin(m_cells), std::end(m_cells), [](QString& str) {str.clear(); });
	m_previousMoveQueue = "B";
	m_firstClick = QModelIndex();
	emit dataChanged(index(0, 0), index(SIZE - 1, SIZE - 1));
}

void CChessModel::previous()
{
	if (m_currentStepIndex - 1 >= 0)
	{
		m_currentStepIndex--;
		std::copy(m_gameStates[m_currentStepIndex].begin(), m_gameStates[m_currentStepIndex].end(), std::begin(m_cells));
		emit dataChanged(index(0, 0), index(SIZE - 1, SIZE - 1));
		if (m_currentStepIndex + 1 < m_gameStates.size())
		{
			emit furtherStepsAvailable();
		}
	}
	
	if (m_currentStepIndex - 1 < 0)
		emit noCloserStepsAvailable();
}

void CChessModel::next()
{
	if (m_currentStepIndex + 1 < m_gameStates.size())
	{
		m_currentStepIndex++;
		std::copy(m_gameStates[m_currentStepIndex].begin(), m_gameStates[m_currentStepIndex].end(), std::begin(m_cells));
		emit dataChanged(index(0, 0), index(SIZE - 1, SIZE - 1));
		if (m_currentStepIndex - 1 >= 0)
		{
			emit closerStepsAvailable();
		}
	}
	
	if (m_currentStepIndex + 1 >= m_gameStates.size())
		emit noFurtherStepsAvailable();
}

bool CChessModel::containsFigure(const QModelIndex& index) const
{
	if (index.isValid())
	{
		return !m_cells[index.row()*SIZE + index.column()].isEmpty();
	}
	return false;
}

// use it only when first click is valid
bool CChessModel::containsOppositeFigure(const QModelIndex& index) const
{
	if (index.isValid())
	{
		const QString currentFigure = m_cells[m_firstClick.row()*SIZE + m_firstClick.column()];
		const QString targetCell = m_cells[index.row()*SIZE + index.column()];
		if (targetCell.isEmpty())
			return false;
		return currentFigure.at(0) != targetCell.at(0);			// compare colors of the figures
	}
	return false;
}

bool CChessModel::isValidMovement(const QModelIndex& index) const
{
	if (index.isValid())
	{
		const QString currentFigure = m_cells[m_firstClick.row()*SIZE + m_firstClick.column()];

		const int xDiff = abs(m_firstClick.row() - index.row());
		const int yDiff = abs(m_firstClick.column() - index.column());

		switch (currentFigure.at(1).toLatin1())
		{
		case King: return (xDiff == 1 || xDiff == 0) && (yDiff == 1 || yDiff == 0) && isFreeTrajectory(index);
		case Queen: return xDiff == yDiff || xDiff == 0 || yDiff == 0 && isFreeTrajectory(index);
		case Bishop: return xDiff == yDiff && isFreeTrajectory(index);
		case Knight: return (xDiff == 2 && yDiff == 1) || (xDiff == 1 && yDiff == 2);
		case Rook: return xDiff == 0 || yDiff == 0 && isFreeTrajectory(index);
		case Pawn:
			{
				bool canMove = false;
				const QString color = currentFigure.at(0);
				const int pawnRowDiff = index.row() - m_firstClick.row();
				const bool isAllovedColumnShifting = (yDiff == 0 && !containsFigure(index)) || (yDiff == 1 && containsOppositeFigure(index));
				if (color == "B")
				{
					if (m_firstClick.row() == 1)
						canMove = (pawnRowDiff == 1 || pawnRowDiff == 2) && isAllovedColumnShifting;
					else
						canMove = pawnRowDiff == 1 && isAllovedColumnShifting;
				}
				else if (color == "W")
				{
					if (m_firstClick.row() == SIZE-2)
						canMove = (pawnRowDiff == -1 || pawnRowDiff == -2) && isAllovedColumnShifting;
					else
						canMove = pawnRowDiff == -1 && isAllovedColumnShifting;
				}
				return canMove && isFreeTrajectory(index);
			}
		}
	}
	return false;
}

bool CChessModel::isFreeTrajectory(const QModelIndex& targetIndex) const
{
	if (targetIndex.isValid())
	{
		const int xDiff = m_firstClick.row() - targetIndex.row();
		const int yDiff = m_firstClick.column() - targetIndex.column();

		if (xDiff == 0)
		{
			const int minY = yDiff > 0 ? targetIndex.column() : m_firstClick.column();
			const int maxY = yDiff < 0 ? targetIndex.column() : m_firstClick.column();
			for (int y = minY+1; y < maxY; ++y)
			{
				QModelIndex currentIdx = index(m_firstClick.row(), y);
				if (containsFigure(currentIdx) && !containsOppositeFigure(currentIdx))
					return false;
			}
			return true;
		}
		if (yDiff == 0)
		{
			const int minX = xDiff > 0 ? targetIndex.row() : m_firstClick.row();
			const int maxX = xDiff < 0 ? targetIndex.row() : m_firstClick.row();
			for (int x = minX+1; x < maxX; ++x)
			{
				QModelIndex currentIdx = index(x, m_firstClick.column());
				if (containsFigure(currentIdx) && !containsOppositeFigure(currentIdx))
					return false;
			}
			return true;
		}
		if (abs(xDiff) == abs(yDiff))
		{
			const int signX = targetIndex.row() > m_firstClick.row() ? +1 : -1;
			const int signY = targetIndex.column() > m_firstClick.column() ? +1 : -1;

			for (int step = 1; step < abs(xDiff); ++step)
			{
				QModelIndex currentIdx = index(m_firstClick.row() + signX*step, m_firstClick.column() + signY*step);
				if (containsFigure(currentIdx) && !containsOppositeFigure(currentIdx))
					return false;
			}
			return true;
		}
	}
	return false;
}

void CChessModel::storeGameState()
{
	using namespace std;
	QStringList gameState;
	gameState.reserve(SIZE*SIZE);
	copy(begin(m_cells), end(m_cells), back_insert_iterator<QStringList>(gameState));
	m_gameStates << gameState;
}
