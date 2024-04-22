#pragma once
#include "MeshViewerWidget.h"
#include<QThread>
#include<QObject>

class Worker;

#ifndef INTERACTIVEVIEWERWIDGET
#define INTERACTIVEVIEWERWIDGET

class InteractiveViewerWidget : public MeshViewerWidget
{
	Q_OBJECT
public:
	InteractiveViewerWidget(QWidget* parent = 0);
	~InteractiveViewerWidget();
	void MinimalSurface(int iteration = 100, double _lambda = 0.01);
	void Parameterization(int method);
protected:
	void dragEnterEvent(QDragEnterEvent *event);
	void dropEvent(QDropEvent *event);
private:
	Worker* worker;
	QThread* thread;
};

#endif // INTERACTIVEVIEWERWIDGET
