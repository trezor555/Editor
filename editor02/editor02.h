#ifndef EDITOR02_H
#define EDITOR02_H

#include <QtWidgets/QMainWindow>
#include "ui_editor02.h"
#include <QFile>
#include <QTextStream>
#include <QFileDialog>
#include <QPushButton>
#include <QActionGroup>
#include <vector>

class editor02 : public QMainWindow
{
	Q_OBJECT

public:
	editor02(Engine* engine, QWidget *parent = 0);
	~editor02();

private:
	Ui::editor02Class ui;
	void LoadStaticModelsFromDirs();
	void LoadAnimatedModelsFromDirs();
	QActionGroup* mainActionGroup;
	std::vector<QDockWidget*> m_dockwidgets;
	Engine* m_Engine;
	
public slots:
	void AddToLog(std::string text);
	
private slots:
void on_actionModifyTerrain_triggered();
void on_actionPaintTerrain_triggered();
void on_actionSettings_triggered();
void on_actionAddObjects_triggered();
void on_actionAddAIPath_triggered();
void on_actionNew_level_triggered();
void on_actionAddFoliage_triggered();
void TerrainLayerItemDoubleClicked(QListWidgetItem* item);
void SetWorkingLayer(QListWidgetItem* item);
void ListWaypoints(std::vector<ObjectData> waypoints);
void WaypointItemChanged(QListWidgetItem* item);
void AddFoliage();
void InstancedMeshClicked(QListWidgetItem* item);
void WaypointSelected(QListWidgetItem* item);
void RotationChanged(glm::vec3 val);
void PositionChanged(glm::vec3 val);


void on_actionSave_level_triggered();
void on_actionLoad_level_triggered();
void on_actionExit_triggered();

};

#endif // EDITOR02_H
