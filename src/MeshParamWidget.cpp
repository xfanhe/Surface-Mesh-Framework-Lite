#include "MeshParamWidget.h"

MeshParamWidget::MeshParamWidget(QWidget *parent)
	: QWidget(parent)
{
	CreateTabWidget();
	CreateLayout();
}

MeshParamWidget::~MeshParamWidget()
{
}

void MeshParamWidget::CreateTabWidget(void)
{
	pbPrintInfo = new QPushButton(tr("Print Mesh Info"));
	pbFindBoundary = new QPushButton("Minimal Surface");
	pbParameterization = new QPushButton("Parameterization");
	leIterateInPut = new QLineEdit();
	QLabel* lbIterate = new QLabel("Iterate:");
	leLambda = new QLineEdit();
	QLabel* lbLambda = new QLabel("Lambda:");
	cbMethodforParameterization = new QComboBox();
	cbMethodforParameterization->addItem("Uniform Weight Laplacian");
	QLabel* lbMethodforParameterization = new QLabel("Method:");
	connect(pbPrintInfo, &QPushButton::clicked, this, &MeshParamWidget::PrintInfoSignal);
	connect(pbFindBoundary, &QPushButton::clicked, this, &MeshParamWidget::OnFindBoundaryClicked);
	connect(pbParameterization, &QPushButton::clicked, this, &MeshParamWidget::OnParameterizationClicked);
	connect(cbMethodforParameterization, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int index) {
		method = index;
	});
	QVBoxLayout* layout = new QVBoxLayout();
	layout->addWidget(pbPrintInfo);
	layout->addWidget(lbIterate);
	layout->addWidget(leIterateInPut);
	layout->addWidget(lbLambda);
	layout->addWidget(leLambda);
	layout->addWidget(pbFindBoundary);
	layout->addWidget(lbMethodforParameterization);
	layout->addWidget(cbMethodforParameterization);
	layout->addWidget(pbParameterization);
	layout->addStretch();
	wParam = new QWidget();
	wParam->setLayout(layout);
	saParam = new QScrollArea();
	saParam->setFocusPolicy(Qt::NoFocus);
	saParam->setFrameStyle(QFrame::NoFrame);
	saParam->setWidget(wParam);
	saParam->setWidgetResizable(true);
}

void MeshParamWidget::CreateLayout(void)
{
	twParam = new QTabWidget();
	twParam->addTab(saParam, "Tab");
	QGridLayout *layout = new QGridLayout();
	layout->addWidget(twParam, 0, 0, 1, 1);
	this->setLayout(layout);
}

void MeshParamWidget::OnFindBoundaryClicked(void)
{
	if(!leIterateInPut->text().isEmpty())
	{
		iterate = leIterateInPut->text().toInt();
	}
	if(!leLambda->text().isEmpty())
	{
		lambda = leLambda->text().toDouble();
	}

	emit FindBoundary(iterate, lambda);
}

void MeshParamWidget::OnParameterizationClicked(void)
{
	emit Parameterization(method);
}