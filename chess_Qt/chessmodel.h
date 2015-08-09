#ifndef CHESSMODEL_H
#define CHESSMODEL_H

#include <QAbstractTableModel>

class CChessModel : public QAbstractTableModel
{
	Q_OBJECT

public:
	CChessModel(QObject* parent = nullptr);
	~CChessModel();

	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;
	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
	virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

	QStringList figuresPositions() const;
	void loadPositions(const QStringList& slPositions);
	void start();
	void stop();
	void previous();
	void next();

signals:
	void notifyUi(const QString& message);
	void furtherStepsAvailable();
	void closerStepsAvailable();
	void noCloserStepsAvailable();
	void noFurtherStepsAvailable();

private:
	static const int SIZE = 8;
	enum Figures { King = 'K', Queen = 'Q', Bishop = 'B', Knight = 'N', Rook = 'R', Pawn = 'P' };

	QString m_cells[SIZE*SIZE];
	QModelIndex m_firstClick;
	QString m_previousMoveQueue;
	QList<QStringList> m_gameStates;
	int m_currentStepIndex;

	bool containsFigure(const QModelIndex& index) const;
	bool containsOppositeFigure(const QModelIndex& index) const;
	bool isValidMovement(const QModelIndex& index) const;				// checks whether the figure can move in this way
	bool isFreeTrajectory(const QModelIndex& index) const;

	void storeGameState();
};

#endif // CHESSMODEL_H
