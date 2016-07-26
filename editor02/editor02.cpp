#include "editor02.h"
#include "createnewlevel.h"

editor02::editor02(Engine* engine, QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	m_Engine = engine;
	ui.widget->Init(m_Engine);
	//connect(ui.widget->WireFrameModeToggled()
	mainActionGroup = new QActionGroup(parent);
	mainActionGroup->addAction(ui.actionModifyTerrain);
	mainActionGroup->addAction(ui.actionPaintTerrain);
	mainActionGroup->addAction(ui.actionAddObjects);
	//mainActionGroup->addAction(ui.actionSettings);
	mainActionGroup->addAction(ui.actionSelectTool);

	mainActionGroup->addAction(ui.actionMoveTool);
	mainActionGroup->addAction(ui.actionSelectTool);
	mainActionGroup->addAction(ui.actionRotationTool);
	mainActionGroup->addAction(ui.actionAddAIPath);
	mainActionGroup->addAction(ui.actionAddFoliage);
	//mainActionGroup->addAction(ui.actionPlay);

	// Hide dockwidgets
	m_dockwidgets.push_back(ui.paintLayersDockWidget);
	m_dockwidgets.push_back(ui.editTerrainDockWidget);
	m_dockwidgets.push_back(ui.waypointDockWidget);
	m_dockwidgets.push_back(ui.addObjectsDockWidget);
	m_dockwidgets.push_back(ui.foliageDockWidget);
	//m_dockwidgets.push_back(ui.actionRotateTool);

	ui.infoDockWidget->hide();

	ui.terrainLayerListWidget->setIconSize(QSize(80, 80));

	ui.terrainLayerListWidget->setResizeMode(QListWidget::Adjust);
	ui.terrainLayerListWidget->setViewMode(QListWidget::IconMode);
	ui.terrainLayerListWidget->setDragDropMode(QAbstractItemView::NoDragDrop);

	ui.terrainLayerListWidget->addItem(new QListWidgetItem(QIcon("../Data/Textures/Terrain/DiffuseMaps/sand_diffuse.bmp"), "Layer 1"));
	ui.terrainLayerListWidget->addItem(new QListWidgetItem(QIcon("../Data/Textures/Misc/notexture_2.bmp"), "Layer 2"));
	ui.terrainLayerListWidget->addItem(new QListWidgetItem(QIcon("../Data/Textures/Misc/notexture_2.bmp"), "Layer 3"));
	ui.terrainLayerListWidget->addItem(new QListWidgetItem(QIcon("../Data/Textures/Misc/notexture_2.bmp"), "Layer 4"));
	QObject::connect(ui.terrainLayerListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(TerrainLayerItemDoubleClicked(QListWidgetItem*)));
	connect(ui.terrainLayerListWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(SetWorkingLayer(QListWidgetItem*)));
	
	connect(ui.addAnimatedModelToWaypointButton, SIGNAL(toggled(bool)), ui.widget, SLOT(AddToWaypoint(bool)));

	connect(ui.textureRepeat_1_Slider, SIGNAL(valueChanged(int)), ui.widget, SLOT(Texture_1_RepeatChanged(int)));
	connect(ui.textureRepeat_2_Slider, SIGNAL(valueChanged(int)), ui.widget, SLOT(Texture_2_RepeatChanged(int)));
	connect(ui.textureRepeat_3_Slider, SIGNAL(valueChanged(int)), ui.widget, SLOT(Texture_3_RepeatChanged(int)));
	connect(ui.textureRepeat_4_Slider, SIGNAL(valueChanged(int)), ui.widget, SLOT(Texture_4_RepeatChanged(int)));
	
	connect(ui.widget, SIGNAL(ListWaypoints(std::vector<ObjectData>)), this, SLOT(ListWaypoints(std::vector<ObjectData>)));
	connect(ui.waypointListWidget, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(WaypointItemChanged(QListWidgetItem*)));
	connect(ui.waypointListWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(WaypointSelected(QListWidgetItem*)));

	for (int i = 0; i < m_dockwidgets.size(); ++i)
		m_dockwidgets[i]->hide();

	connect(ui.actionWireframe, SIGNAL(triggered()), ui.widget, SLOT(WireFrameModeToggled()));
	connect(ui.actionSolid, SIGNAL(triggered()), ui.widget, SLOT(SolidModeToggled()));
	connect(ui.actionFilter, SIGNAL(triggered()), ui.widget, SLOT(FilterTerrainToggled()));
	connect(ui.actionSelectTool, SIGNAL(triggered()), ui.widget, SLOT(SelectToolToggled()));
	connect(ui.actionMoveTool, SIGNAL(triggered()), ui.widget, SLOT(TranslationToolToggled()));
	connect(ui.actionRotationTool, SIGNAL(triggered()), ui.widget, SLOT(RotationToolToggled()));
	connect(ui.actionAddObjects, SIGNAL(triggered()), ui.widget, SLOT(AddObjectToggled()));
	connect(ui.actionSettings, SIGNAL(toggled(bool)), ui.settingsDockWidget, SLOT(setVisible(bool)));

	connect(ui.actionCook, SIGNAL(triggered()), ui.widget, SLOT(CookTerrain()));

	connect(ui.actionModifyTerrain, SIGNAL(triggered()), ui.widget, SLOT(ModifyTerrainToggled()));
	connect(ui.actionPaintTerrain, SIGNAL(triggered()), ui.widget, SLOT(PaintTerrainToggled()));
	
	connect(ui.actionAddFoliage, SIGNAL(triggered()), ui.widget, SLOT(AddFoliageToggled()));

	connect(ui.terrainRadiusSlider, SIGNAL(valueChanged(int)), ui.widget, SLOT(SetTerrainBrushRadius(int)));
	connect(ui.terrainHeightSlider, SIGNAL(valueChanged(int)), ui.widget, SLOT(SetTerrainBrushHeight(int)));
	connect(ui.terrainStrengthSlider, SIGNAL(valueChanged(int)), ui.widget, SLOT(SetTerrainBrushStrength(int)));
	connect(ui.terrainViewDistanceSlider, SIGNAL(valueChanged(int)), ui.widget, SLOT(ChangeTerrainViewDistance(int)));
	connect(ui.cameraSpeedSlider, SIGNAL(valueChanged(int)), ui.widget, SLOT(ChangeCameraSpeed(int)));

	connect(ui.terrainPaintBrushRadiusSlider, SIGNAL(valueChanged(int)), ui.widget, SLOT(SetTerrainPaintBrushRadius(int)));
	connect(ui.terrainPaintBrushStrengthSlider, SIGNAL(valueChanged(int)), ui.widget, SLOT(SetTerrainPaintBrushStrength(int)));

	//connect(ui.renderBoundingBoxCheckBox, SIGNAL(clicked()), ui.widget, SLOT(SetDrawBoundingBox()));
	connect(ui.widget, SIGNAL(ToLog(std::string)), this, SLOT(AddToLog(std::string)));
	connect(ui.renderTerrainCheckBox, SIGNAL(toggled(bool)), ui.widget, SLOT(SetDrawTerrain(bool)));
	connect(ui.renderWaterCheckBox, SIGNAL(clicked()), ui.widget, SLOT(SetDrawWater()));

	connect(ui.addWaypointButton, SIGNAL(toggled(bool)), ui.widget, SLOT(AddWaypointToggled(bool)));

	
	connect(ui.actionPlay, SIGNAL(toggled(bool)), ui.widget, SLOT(PlayToggled(bool)));
	connect(ui.addMultipleCheckBox, SIGNAL(toggled(bool)), ui.widget, SLOT(AddMultipleObject(bool)));

	connect(ui.addFoliagePushButton, SIGNAL(clicked()), this, SLOT(AddFoliage()));
	connect(ui.foliageObjectsListWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(InstancedMeshClicked(QListWidgetItem*)));
	
	// Foliage
	connect(ui.enableCullingCheckBox, SIGNAL(toggled(bool)), ui.widget, SLOT(SetInstancedMeshCulling(bool)));
	connect(ui.enableBlendCheckBox, SIGNAL(toggled(bool)), ui.widget, SLOT(SetInstancedMeshBlend(bool)));
	//connect(ui.enableZWriteCheckBox, SIGNAL(toggled(bool)), ui.widget, SLOT(SetInstancedMeshZWrite(bool)));
	connect(ui.deleteFoliageCheckBox, SIGNAL(toggled(bool)), ui.widget, SLOT(SetDeleteFoliage(bool)));
	connect(ui.foliageBrushSizeSlider, SIGNAL(valueChanged(int)), ui.widget, SLOT(SetFoliageBrushSize(int)));


	connect(ui.rotationXSpinBox, SIGNAL(valueChanged(double)), ui.widget, SLOT(SetObjectRotationX(double)));
	connect(ui.rotationYSpinBox, SIGNAL(valueChanged(double)), ui.widget, SLOT(SetObjectRotationY(double)));
	connect(ui.rotationZSpinBox, SIGNAL(valueChanged(double)), ui.widget, SLOT(SetObjectRotationZ(double)));

	connect(ui.positionXSpinBox, SIGNAL(valueChanged(double)), ui.widget, SLOT(SetPositionX(double)));
	connect(ui.positionYSpinBox, SIGNAL(valueChanged(double)), ui.widget, SLOT(SetPositionY(double)));
	connect(ui.positionZSpinBox, SIGNAL(valueChanged(double)), ui.widget, SLOT(SetPositionZ(double)));

	connect(ui.widget, SIGNAL(RotationChanged(glm::vec3)), this, SLOT(RotationChanged(glm::vec3)));
	connect(ui.widget, SIGNAL(PositionChanged(glm::vec3)), this, SLOT(PositionChanged(glm::vec3)));


	connect(ui.actionSnap_tool, SIGNAL(toggled(bool)), ui.snapToolDockWidget, SLOT(setVisible(bool)));
	connect(ui.actionObject_properties, SIGNAL(toggled(bool)), ui.selectedObjectPropertiesDockWidget, SLOT(setVisible(bool)));

	connect(ui.actionWater, SIGNAL(toggled(bool)), ui.waterDockWidget, SLOT(setVisible(bool)));
	ui.selectedObjectPropertiesDockWidget->setVisible(false);

	ui.settingsDockWidget->setVisible(false);
	ui.waterDockWidget->setVisible(false);
	ui.snapToolDockWidget->setVisible(false);

	connect(ui.snapToGridXValueSpinBox, SIGNAL(valueChanged(int)), ui.widget, SLOT(SetGridSnapWidth(int)));
	connect(ui.snapToGridYValueSpinBox, SIGNAL(valueChanged(int)), ui.widget, SLOT(SetGridSnapHeight(int)));
	connect(ui.snapToGridZValueSpinBox, SIGNAL(valueChanged(int)), ui.widget, SLOT(SetGridSnapDepth(int)));

	connect(ui.waterLevelSlider, SIGNAL(valueChanged(int)), ui.widget, SLOT(SetWaterLevel(int)));
	connect(ui.waterLevelSpinBox, SIGNAL(valueChanged(int)), ui.widget, SLOT(SetWaterLevel(int)));

	connect(ui.waterColorRSlider, SIGNAL(valueChanged(int)), ui.widget, SLOT(SetWaterRedColor(int)));
	connect(ui.waterColorGSlider, SIGNAL(valueChanged(int)), ui.widget, SLOT(SetWaterGreenColor(int)));
	connect(ui.waterColorBSlider, SIGNAL(valueChanged(int)), ui.widget, SLOT(SetWaterBlueColor(int)));

	connect(ui.specularPowerSlider, SIGNAL(valueChanged(int)), ui.widget, SLOT(SetSpecularPower(int)));
	connect(ui.waterOpacitySlider, SIGNAL(valueChanged(int)), ui.widget, SLOT(SetOpacity(int)));

	connect(ui.actionSnap_to_grid, SIGNAL(toggled(bool)), ui.widget, SLOT(ToggleSnapTool(bool)));
	connect(ui.actionStatistic, SIGNAL(toggled(bool)), ui.widget, SLOT(ShowStatisctic(bool)));

	LoadStaticModelsFromDirs();
	LoadAnimatedModelsFromDirs();
}

editor02::~editor02()
{

}

void editor02::LoadStaticModelsFromDirs()
{
	QDir directory("../Data/Models/static/");
	directory.setFilter(QDir::Files);
	QStringList files = directory.entryList();

	for (int i = 0; i < files.size(); ++i)
	{
		QListWidgetItem* item = new QListWidgetItem(files[i].split(".").first());
		item->setData(Qt::UserRole, "../Data/Models/static/" + files[i]);
		ui.staticObjectsListWidget_2->addItem(item);
	}
}

void editor02::AddToLog(std::string text)
{
	ui.logTextBrowser->append(QString(text.c_str()));
}

void editor02::on_actionModifyTerrain_triggered()
{
	for (int i = 0; i < m_dockwidgets.size(); ++i)
	{
		if (m_dockwidgets[i]->objectName() == "editTerrainDockWidget")
			m_dockwidgets[i]->show();
		else
			m_dockwidgets[i]->hide();
	}
}

void editor02::on_actionPaintTerrain_triggered()
{
	for (int i = 0; i < m_dockwidgets.size(); ++i)
	{
			m_dockwidgets[i]->hide();
	}

	for (int i = 0; i < m_dockwidgets.size(); ++i)
	{
		if (m_dockwidgets[i]->objectName() == "paintLayersDockWidget")
		{
			m_dockwidgets[i]->show();
		}
	}
}

void editor02::on_actionSettings_triggered()
{
	for (int i = 0; i < m_dockwidgets.size(); ++i)
	{
		m_dockwidgets[i]->hide();
	}

	for (int i = 0; i < m_dockwidgets.size(); ++i)
	{
		if (m_dockwidgets[i]->objectName() == "settingsDockWidget")
		{
			m_dockwidgets[i]->show();
		}
	}
}

void editor02::on_actionAddObjects_triggered()
{
	for (int i = 0; i < m_dockwidgets.size(); ++i)
	{
		m_dockwidgets[i]->hide();
	}

	for (int i = 0; i < m_dockwidgets.size(); ++i)
	{
		if (m_dockwidgets[i]->objectName() == "addObjectsDockWidget")
		{
			m_dockwidgets[i]->show();
		}
	}
}

void editor02::on_actionAddAIPath_triggered()
{
	for (int i = 0; i < m_dockwidgets.size(); ++i)
	{
		m_dockwidgets[i]->hide();
	}

	for (int i = 0; i < m_dockwidgets.size(); ++i)
	{
		if (m_dockwidgets[i]->objectName() == "waypointDockWidget")
		{
			m_dockwidgets[i]->show();
		}
	}
}

// Load texture to paint the terrain
void editor02::TerrainLayerItemDoubleClicked(QListWidgetItem* item)
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open Image"), "../Data/Textures/Terrain/DiffuseMaps", tr("Image Files (*.png *.bmp)"));
	std::string layerFileName = "../Data/Textures/Terrain/DiffuseMaps/" + (fileName.section('/', -1).toUtf8());
	std::string layer = item->text().toUtf8().constData();
	if (layerFileName != "")
	{
		if (layer == "Layer 1" && fileName != "")
		{
			item->setIcon(QIcon(layerFileName.c_str()));
			ui.widget->GetTerrain()->SetTerrainLayer(layerFileName, 0);
		}

		if (layer == "Layer 2" && fileName != "")
		{ 
			item->setIcon(QIcon(layerFileName.c_str()));
			ui.widget->GetTerrain()->SetTerrainLayer(layerFileName, 1);
		}

		if (layer == "Layer 3" && fileName != "")
		{
			item->setIcon(QIcon(layerFileName.c_str()));
			ui.widget->GetTerrain()->SetTerrainLayer(layerFileName, 2);
		}

		if (layer == "Layer 4" && fileName != "")
		{
			item->setIcon(QIcon(layerFileName.c_str()));
			ui.widget->GetTerrain()->SetTerrainLayer(layerFileName, 3);
		}
	}
}

// Set the current texture layer to paint the terrain
void editor02::SetWorkingLayer(QListWidgetItem* item)
{
		std::string layer = item->text().toUtf8().constData();
		if (layer != "")
		{
			if (layer == "Layer 1")
			{
				ui.widget->GetTerrain()->SetWorkingLayer(0);
			}

			if (layer == "Layer 2")
			{
				ui.widget->GetTerrain()->SetWorkingLayer(1);
			}

			if (layer == "Layer 3")
			{
				ui.widget->GetTerrain()->SetWorkingLayer(2);
			}

			if (layer == "Layer 4")
			{
				ui.widget->GetTerrain()->SetWorkingLayer(3);
			}
		}
	
}

void editor02::ListWaypoints(std::vector<ObjectData> waypoints)
{
	ui.waypointListWidget->clear();
	for (size_t i = 0; i < waypoints.size(); i++)
	{
		QListWidgetItem* item = new QListWidgetItem(QString::fromStdString(waypoints[i].name));
		item->setData(Qt::UserRole, waypoints[i].id);
		item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
		item->setCheckState(Qt::Checked);
		ui.waypointListWidget->addItem(item);
	}
}

// Show or hide the selected waypoint in the editor
void editor02::WaypointItemChanged(QListWidgetItem* item)
{
	if (item->checkState() == Qt::Checked)
	{
		QVariant data = item->data(Qt::UserRole);
		ui.widget->SetWaypointVisibility(data.toInt(), true);
	}
	else
	{
		QVariant data = item->data(Qt::UserRole);
		ui.widget->SetWaypointVisibility(data.toInt(), false);
	}
}

// Create a new level
void editor02::on_actionNew_level_triggered()
{
	createnewlevel newLevel(this);

	if (newLevel.exec())
	{
		//QString levelName = newLevel.ui.levelNameLineEdit->text();
		int heightmapResolution = newLevel.ui.terrainSizeComboBox->currentText().toInt();
		int cellWidth = newLevel.ui.cellWidthComboBox->currentText().toInt();
		ui.widget->CreateTerrain(heightmapResolution, cellWidth);

		// Set the values to their default
		QListWidgetItem* item = ui.terrainLayerListWidget->item(0);
		item->setIcon(QIcon("../Data/Textures/Terrain/DiffuseMaps/sand_diffuse.bmp"));

		for (int i = 1; i < ui.terrainLayerListWidget->count(); ++i)
		{
			QListWidgetItem* item = ui.terrainLayerListWidget->item(i);
			item->setIcon(QIcon("../Data/Textures/Misc/notexture_2.bmp"));
		}

		ui.foliageObjectsListWidget->clear();
		ui.waypointListWidget->clear();

		ui.terrainHeightSlider->setValue(150);
		ui.terrainRadiusSlider->setValue(10);
		ui.terrainStrengthSlider->setValue(100);
		ui.terrainViewDistanceSlider->setValue(15000);
		ui.terrainPaintBrushRadiusSlider->setValue(10);
		ui.terrainPaintBrushStrengthSlider->setValue(100);

		ui.snapToGridXValueSpinBox->setValue(8);
		ui.snapToGridYValueSpinBox->setValue(8);
		ui.snapToGridZValueSpinBox->setValue(8);

		ui.textureRepeat_1_Slider->setValue(128);
		ui.textureRepeat_2_Slider->setValue(128);
		ui.textureRepeat_3_Slider->setValue(128);
		ui.textureRepeat_4_Slider->setValue(128);

		ui.waterLevelSlider->setValue(20);
		ui.waterColorRSlider->setValue(0);
		ui.waterColorGSlider->setValue(186);
		ui.waterColorBSlider->setValue(219);
		ui.waterOpacitySlider->setValue(50);
		ui.specularPowerSlider->setValue(20);
	}
}

// Add a model to the editor to use as an instanced mesh
void editor02::AddFoliage()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open Model"), "../Data/Models/Foliage/", tr("Model Files (*.X *.fbx)"));
	if (fileName != "")
	{
		std::string filePath = fileName.toUtf8();
		unsigned int index;
		if (ui.widget->AddFoliage(filePath, index))
		{
			QListWidgetItem* item = new QListWidgetItem(fileName.section('/', -1).toUtf8());
			item->setData(Qt::UserRole, index);
			ui.foliageObjectsListWidget->addItem(item);
		}
	}
}

void editor02::InstancedMeshClicked(QListWidgetItem* item)
{
	QVariant data = item->data(Qt::UserRole);
	bool cull;
	bool zwrite;
	bool blend;
	ui.widget->SelectInstancedMesh(data.toInt(), cull, zwrite, blend);

	if (cull)
		ui.enableCullingCheckBox->setCheckState(Qt::Checked);
	else
		ui.enableCullingCheckBox->setCheckState(Qt::Unchecked);

	if (blend)
		ui.enableBlendCheckBox->setCheckState(Qt::Checked);
	else
		ui.enableBlendCheckBox->setCheckState(Qt::Unchecked);

	
}

void editor02::on_actionAddFoliage_triggered()
{
	for (int i = 0; i < m_dockwidgets.size(); ++i)
	{
		m_dockwidgets[i]->hide();
	}

	for (int i = 0; i < m_dockwidgets.size(); ++i)
	{
		if (m_dockwidgets[i]->objectName() == "foliageDockWidget")
		{
			m_dockwidgets[i]->show();
		}
	}
}

// Load the animated models names from the Data/Models/Animated dir 
void editor02::LoadAnimatedModelsFromDirs()
{
	QDir directory("../Data/Models/Animated/");
	directory.setFilter(QDir::Files);
	QStringList files = directory.entryList();

	for (int i = 0; i < files.size(); ++i)
	{
		QListWidgetItem* item = new QListWidgetItem(files[i].split(".").first());
		item->setData(Qt::UserRole, "../Data/Models/Animated/" + files[i]);
		ui.animatedModelListWidget->addItem(item);
	}
}

void editor02::on_actionSave_level_triggered()
{
	QString choosenPath = QFileDialog::getSaveFileName(this, tr("Save Level"), "../Data/Maps/", tr("Map (*.xml)"));
	if (choosenPath.size() != 0)
	{
		ui.widget->SaveMap(choosenPath);
	}
}

void editor02::on_actionLoad_level_triggered()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open map"), "../Data/Maps/", tr("Map (*.xml)"));
	if (fileName.size() != 0)
	{
		std::vector<std::string> layerPaths;
		std::vector<unsigned int> layersRepeat;
		float RValue, GValue, BValue, Opacity, WaterLevel, SpecularPower;
		ui.widget->LoadMap(fileName, layerPaths, layersRepeat, RValue, GValue, BValue, Opacity, WaterLevel, SpecularPower);

		for (int i = 0; i < ui.terrainLayerListWidget->count(); ++i)
		{
			QListWidgetItem* item = ui.terrainLayerListWidget->item(i);
			if (layerPaths[i] != "")
			{
				
				item->setIcon(QIcon(layerPaths[i].c_str()));
			}
			else
			{
				item->setIcon(QIcon("../Data/Textures/Misc/notexture_2.bmp"));
			}
		}

		ui.textureRepeat_1_Slider->setValue(layersRepeat[0]);
		ui.textureRepeat_2_Slider->setValue(layersRepeat[1]);
		ui.textureRepeat_3_Slider->setValue(layersRepeat[2]);
		ui.textureRepeat_4_Slider->setValue(layersRepeat[3]);

		ui.foliageObjectsListWidget->clear();
		ui.waypointListWidget->clear();

		ui.terrainHeightSlider->setValue(0);
		ui.terrainRadiusSlider->setValue(10);
		ui.terrainStrengthSlider->setValue(100);
		ui.terrainViewDistanceSlider->setValue(15000);
		ui.terrainPaintBrushRadiusSlider->setValue(10);
		ui.terrainPaintBrushStrengthSlider->setValue(100);

		ui.snapToGridXValueSpinBox->setValue(8);
		ui.snapToGridYValueSpinBox->setValue(8);
		ui.snapToGridZValueSpinBox->setValue(8);

		ui.waterLevelSlider->setValue(WaterLevel);
		ui.waterColorRSlider->setValue(RValue);
		ui.waterColorGSlider->setValue(GValue);
		ui.waterColorBSlider->setValue(BValue);
		ui.waterOpacitySlider->setValue(Opacity);
		ui.specularPowerSlider->setValue(SpecularPower);
	}
}

void editor02::WaypointSelected(QListWidgetItem* item)
{
	QVariant data = item->data(Qt::UserRole);
	ui.widget->SelectedWaypoint(data.toInt());
}

void editor02::on_actionExit_triggered()
{
	QApplication::quit();
}

void editor02::RotationChanged(glm::vec3 val)
{
	ui.rotationXSpinBox->setValue(val.x);
	ui.rotationYSpinBox->setValue(val.y);
	ui.rotationZSpinBox->setValue(val.z);
}

void editor02::PositionChanged(glm::vec3 val)
{
	ui.positionXSpinBox->setValue(val.x);
	ui.positionYSpinBox->setValue(val.y);
	ui.positionZSpinBox->setValue(val.z);
}


