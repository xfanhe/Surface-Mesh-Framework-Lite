#pragma once

#include <QWidget>
#include <QtGui>
#include <QtWidgets>

class MeshParamWidget : public QWidget
{
	Q_OBJECT

public:
	MeshParamWidget(QWidget *parent = 0);
	~MeshParamWidget(void);
private:
	void CreateTabWidget(void);
	void CreateLayout(void);
signals:
	void PrintInfoSignal();
	void FindBoundary(int iterate, double lambda);
	void Parameterization(int method);
public slots:
	void OnFindBoundaryClicked(void);
	void OnParameterizationClicked(void);
private:
	QTabWidget *twParam;
	QWidget *wParam;
	QScrollArea *saParam;
	QPushButton *pbPrintInfo;
	QPushButton* pbFindBoundary;
	QLineEdit* leIterateInPut;
	QLabel* lbIterate;
	QLineEdit* leLambda;
	QLabel* lbLambda;
	QPushButton* pbParameterization;
	QComboBox* cbMethodforParameterization;
	QLabel* lbMethodforParameterization;
	int iterate = 100;
	double lambda = 0.1;
	int method = 0;
};
