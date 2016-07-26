#include "createnewlevel.h"

createnewlevel::createnewlevel(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	ui.terrainSizeComboBox->addItem("512");
	ui.terrainSizeComboBox->addItem("1024");
	ui.terrainSizeComboBox->addItem("2048");
	ui.terrainSizeComboBox->addItem("4096");
	ui.terrainSizeComboBox->addItem("8192");

	ui.cellWidthComboBox->addItem("8");
	ui.cellWidthComboBox->addItem("16");
	ui.cellWidthComboBox->addItem("32");
	ui.cellWidthComboBox->addItem("64");
	ui.cellWidthComboBox->addItem("128");
	ui.cellWidthComboBox->addItem("256");
}

createnewlevel::~createnewlevel()
{
	
}

void createnewlevel::accept()
{
	done(Accepted);
}
