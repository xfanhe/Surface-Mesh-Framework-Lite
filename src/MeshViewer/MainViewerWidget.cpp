#include "MainViewerWidget.h"
#include "../MeshParamWidget.h"
#include "InteractiveViewerWidget.h"
#include <QLayout>
#include <QMessageBox>
#include <iostream>

MainViewerWidget::MainViewerWidget(QWidget* _parent/* =0 */)
	:loadmeshsuccess(false)
{
	InitViewerWindow();
}

MainViewerWidget::~MainViewerWidget(void)
{
}

void MainViewerWidget::InitViewerWindow(void)
{
	CreateViewerDialog();
	CreateParamWidget();

	QHBoxLayout* main_layout = new QHBoxLayout();
	main_layout->addWidget(meshparamwidget);
	main_layout->addWidget(interactviewerwidget, 1);
	this->setLayout(main_layout);
}

void MainViewerWidget::CreateParamWidget(void)
{
	meshparamwidget = new MeshParamWidget();
	connect(meshparamwidget, SIGNAL(PrintInfoSignal()), interactviewerwidget, SLOT(PrintMeshInfo()));
	connect(meshparamwidget, &MeshParamWidget::FindBoundary, [this](int iterate, double lambda) { this->interactviewerwidget->MinimalSurface(iterate, lambda); });
	connect(meshparamwidget, &MeshParamWidget::Parameterization, [this](int method) { this->interactviewerwidget->Parameterization(method); });
}

void MainViewerWidget::CreateViewerDialog(void)
{
	interactviewerwidget = new InteractiveViewerWidget(NULL);
	interactviewerwidget->setAcceptDrops(true);
	connect(interactviewerwidget, SIGNAL(LoadMeshOKSignal(bool, QString)), SLOT(LoadMeshFromInner(bool, QString)));
}

void MainViewerWidget::OpenMeshGUI(const QString & fname)
{
	if (fname.isEmpty() || !interactviewerwidget->LoadMesh(fname.toStdString()))
	{
		QString msg = "Cannot read mesh from file:\n '" + fname + "'";
		QMessageBox::critical(NULL, windowTitle(), msg);
	}
	else
	{
		loadmeshsuccess = true;
		emit(haveLoadMesh(fname));
	}
}

void MainViewerWidget::SaveMeshGUI(const QString & fname)
{
	if (fname.isEmpty() || !interactviewerwidget->SaveMesh(fname.toStdString()))
	{
		QString msg = "Cannot read mesh from file:\n '" + fname + "'";
		QMessageBox::critical(NULL, windowTitle(), msg);
	}
}

void MainViewerWidget::LoadMeshFromInner(bool OK, QString fname)
{
	loadmeshsuccess = OK;
	emit(haveLoadMesh(fname));
}

void MainViewerWidget::Open(void)
{
	QString fileName = QFileDialog::getOpenFileName(this,
		tr("Open mesh file"),
		tr(""),
		tr("Mesh Files (*.obj *.off *.ply *.stl);;"
		"OFF Files (*.off);;"
		"OBJ Files (*.obj);;"
		"PLY Files (*.ply);;"
		"STL Files (*.stl);;"
		"All Files (*)"));
	if (!fileName.isEmpty())
	{
		OpenMeshGUI(fileName);
	}
}

void MainViewerWidget::Save(void)
{
	QString fileName = QFileDialog::getSaveFileName(this,
		tr("Save mesh file"),
		tr("untitled.obj"),
		tr("OBJ Files (*.obj);;"
		"OFF Files (*.off);;"
		"PLY Files (*.ply);;"
		"STL Files (*.stl);;"
		"All Files (*)"));
	if (!fileName.isEmpty())
	{
		SaveMeshGUI(fileName);
	}
}

void MainViewerWidget::ClearMesh(void)
{
	if (loadmeshsuccess)
	{
		loadmeshsuccess = false;
		interactviewerwidget->Clear();
	}
}

void MainViewerWidget::Screenshot(void)
{
	interactviewerwidget->ScreenShot();
}

void MainViewerWidget::ShowPoints(void)
{
	interactviewerwidget->SetDrawMode(InteractiveViewerWidget::POINTS);
}

void MainViewerWidget::ShowWireframe(void)
{
	interactviewerwidget->SetDrawMode(InteractiveViewerWidget::WIREFRAME);
}

void MainViewerWidget::ShowHiddenLines(void)
{
	interactviewerwidget->SetDrawMode(InteractiveViewerWidget::HIDDENLINES);
}

void MainViewerWidget::ShowFlatLines(void)
{
	interactviewerwidget->SetDrawMode(InteractiveViewerWidget::FLATLINES);
}

void MainViewerWidget::ShowFlat(void)
{
	interactviewerwidget->SetDrawMode(InteractiveViewerWidget::FLAT);
}

void MainViewerWidget::ShowSmooth(void)
{
	interactviewerwidget->SetDrawMode(InteractiveViewerWidget::SMOOTH);
}

void MainViewerWidget::Lighting(bool b)
{
	interactviewerwidget->EnableLighting(b);
}

void MainViewerWidget::DoubleSideLighting(bool b)
{
	interactviewerwidget->EnableDoubleSide(b);
}

void MainViewerWidget::ShowBoundingBox(bool b)
{
	interactviewerwidget->SetDrawBoundingBox(b);
}

void MainViewerWidget::ShowBoundary(bool b)
{
	interactviewerwidget->SetDrawBoundary(b);
}

void MainViewerWidget::ResetView(void)
{
	interactviewerwidget->ResetView();
}

void MainViewerWidget::ViewCenter(void)
{
	interactviewerwidget->ViewCenter();
}

void MainViewerWidget::CopyRotation(void)
{
	interactviewerwidget->CopyRotation();
}

void MainViewerWidget::LoadRotation(void)
{
	interactviewerwidget->LoadRotation();
}

void MainViewerWidget::MinimalSurface(void)
{
	interactviewerwidget->MinimalSurface();
}
