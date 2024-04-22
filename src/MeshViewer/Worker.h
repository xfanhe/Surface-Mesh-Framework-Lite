#pragma once
#include<QObject>
#include"../MeshDefinition.h"
#include<Eigen/Sparse>
#include<vector>
#ifndef WORKER
#define WORKER

class InteractiveViewerWidget;
class Worker : public QObject
{
	Q_OBJECT
public:
	void MinimalSurface(InteractiveViewerWidget* viewer, int iterate, double _lambda);
	void Parameterization(InteractiveViewerWidget* viewer, int method);
signals:
	void IterationFinished();
	void WorkFinished();
};
#endif // !WORKER
