#include <QtCore>
#include <QtGui>
#include <iostream>
#include "InteractiveViewerWidget.h"
#include "Worker.h"

InteractiveViewerWidget::InteractiveViewerWidget(QWidget* parent /* = 0 */)
	:MeshViewerWidget(parent)
{
}

InteractiveViewerWidget::~InteractiveViewerWidget()
{
	if(worker){
		delete worker;
	}
	if(thread){
		delete thread;
	}
}


void InteractiveViewerWidget::dragEnterEvent(QDragEnterEvent* event)
{
	if (event->mimeData()->hasFormat("text/uri-list"))
	{
		event->acceptProposedAction();
	}
}

void InteractiveViewerWidget::dropEvent(QDropEvent* event)
{
	QList<QUrl> urls = event->mimeData()->urls();
	if (urls.isEmpty())
		return;
	QString fileName = urls.first().toLocalFile();
	if (fileName.isEmpty())
		return;

	if (fileName.endsWith(".off") || fileName.endsWith(".obj") || fileName.endsWith(".stl") || fileName.endsWith(".ply"))
	{
		if (LoadMesh(fileName.toStdString()))
		{
			emit(LoadMeshOKSignal(true, fileName));
		}
		else
		{
			emit(LoadMeshOKSignal(false, "No Mesh"));
		}
	}
}

void InteractiveViewerWidget::MinimalSurface(int iteration /* = 100 */, double _lambda /* = 0.01*/)
{
	worker = new Worker;
	thread = new QThread;
	(*worker).moveToThread(thread);
	QObject::connect(thread, &QThread::started, [this, iteration, _lambda]()
		{ this->worker->MinimalSurface(this, iteration, _lambda); });
	QObject::connect(worker, &Worker::IterationFinished, [this]()
		{ mesh.update_normals(); this->update(); });
	QObject::connect(worker, &Worker::WorkFinished, thread, &QThread::quit);
	thread->start();
}

void InteractiveViewerWidget::Parameterization(int method) {
	worker = new Worker;
	thread = new QThread;
	(*worker).moveToThread(thread);
	QObject::connect(thread, &QThread::started, [this, method]
		{ this->worker->Parameterization(this, method); });
	QObject::connect(worker, &Worker::WorkFinished, [this] {
		mesh.update_normals();
		this->update();
		//this->MeshInitialization();
		this->thread->quit();
		});
	thread->start();
}