#ifndef GLWIDGET_H
#define GLWIDGET_H

#include "Skybox.h"
#include "Water.h"
#include "FPSCamera.h"
#include "SceneManager.h"
#include "VertexColorShader.h"
#include "ShadowMapping.h"
#include "FullScreenQuad.h"
#include "TexturedModelShader.h"
#include "PhysXCollision.h"
#include "TranslationTool.h"
#include "RotationTool.h"
#include "InstancingShader.h"
#include <QWidget>
#include <QtOpenGL/QGLWidget>
#include <QKeyEvent>
#include <QTime>
#include <AntTweakBar.h>
#include "Engine.h"

enum ACTION
{
	ACTION_NONE,
	ACTION_PLAY,
	EDIT_TERRAIN,
	PAINT_TERRAIN,
	PLACE_OBJECT,
	ADD_FOLIAGE,
	SELECT_OBJECT,
	DROP_OBJECT,
	ROTATE_OBJECT,
	TRANSLATE_OBJECT,
	ADD_WAYPOINT,
	ADD_TO_WAYPOINT
};

enum SUB_ACTION
{
	SUB_ACTION_NONE,
	ADD_MULTIPLE_OBJECT,
	SUB_AC_DELETE_FOLIAGE,
	SUB_AC_ADD_FOLIAGE
};


class GLWidget : public QGLWidget
{
	Q_OBJECT

public:
	GLWidget(QWidget *parent);
	~GLWidget();
	void Init(Engine* engine);
	TerrainV2* GetTerrain();
	void SetWaypointVisibility(unsigned int id, bool val);
	void CreateTerrain(unsigned int TerrainSize, unsigned int CellWidth);
	bool AddFoliage(std::string filePath, unsigned int& id);
	void SaveMap(QString ChoosenPath);
	void LoadMap(QString FileName, std::vector<std::string>& layerPaths, std::vector<unsigned int>& layersRepeat,
		float& RColor, float& GColor, float& BColor, float& OpacityVal, float& WaterLevelVal, float& specularPower);
	void SelectedWaypoint(int id);
	void Release();
protected:
	void initializeGL();
	void CreateSkybox();
	void paintGL();
	void resizeGL(int width, int height);
	QSize minimumSizeHint() const;
	QSize sizeHint() const;
	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent* evt);
	void LeftMouseButtonPressed(QMouseEvent * event);
	void mouseMoveEvent(QMouseEvent *event);
	void keyPressEvent(QKeyEvent *e);
	void keyReleaseEvent(QKeyEvent *e);
	void timerEvent(QTimerEvent *);

	virtual void dragMoveEvent(QDragMoveEvent *evt);
	void dropEvent(QDropEvent * evt);
	void dragEnterEvent(QDragEnterEvent * evt);
	void Update();
	void Clear();
	void ClearDepth();
	

public slots:
	void WireFrameModeToggled();
	void SolidModeToggled();
	void FilterTerrainToggled();
	void ModifyTerrainToggled();
	void AddFoliageToggled();
	void PaintTerrainToggled();
	void ChangeTerrainViewDistance(int value);
	void ChangeCameraSpeed(int value);
	void SetTerrainBrushRadius(int value);
	void SetTerrainBrushHeight(int value);
	void SetTerrainBrushStrength(int value);

	void SetTerrainPaintBrushRadius(int value);
	void SetTerrainPaintBrushStrength(int value);

	void CookTerrain();
	void SetDrawTerrain(bool val);
	void SetDrawWater();
	void SetDrawBoundingBox();
	void AddWaypointToggled(bool val);

	void SelectToolToggled();
	void TranslationToolToggled();
	void RotationToolToggled();
	void AddObjectToggled();
	void SettingsToggled();

	void Texture_1_RepeatChanged(int value);
	void Texture_2_RepeatChanged(int value);
	void Texture_3_RepeatChanged(int value);
	void Texture_4_RepeatChanged(int value);
	void SetWaypointClosed(bool val);
	void PlayToggled(bool val);
	void AddMultipleObject(bool val);
	void SelectInstancedMesh(unsigned int index, bool& cull, bool& zwrite, bool& blend);

	void SetInstancedMeshCulling(bool val);
	void SetInstancedMeshZWrite(bool val);
	void SetInstancedMeshBlend(bool val);
	void SetDeleteFoliage(bool val);
	void SetFoliageBrushSize(int val);
	void AddToWaypoint(bool val);

	void SetObjectRotationX(double val);
	void SetObjectRotationY(double val);
	void SetObjectRotationZ(double val);

	void SetPositionX(double val);
	void SetPositionY(double val);
	void SetPositionZ(double val);

	void SetGridSnapWidth(int val);
	void SetGridSnapHeight(int val);
	void SetGridSnapDepth(int val);

	void ToggleSnapTool(bool val);
	void SetWaterLevel(int waterLevel);

	void SetWaterRedColor(int val);
	void SetWaterGreenColor(int val);
	void SetWaterBlueColor(int val);

	void SetSpecularPower(int val);
	void SetOpacity(int val);
	void ShowStatisctic(bool val);
	
signals:
	void ToLog(std::string text);
	void ListWaypoints(std::vector<ObjectData> waypoints);
	void RotationChanged(glm::vec3 rotation);
	void PositionChanged(glm::vec3 rotation);
private:
	Engine* m_Engine;
	void Draw();
	void InitAntTweakBar();
	glm::vec3 SnapCoordToGridPoint(const glm::vec3& point);
	FPSCamera* m_pMainCamera;;
	ShadowMapping shadowMapping;
	
	
	float deltaTime; 
	unsigned int lastFrame;
	
	GLint viewLoc;
	GLint projectionLoc;
	
	GLint worldLoc;
	Shader basicShader;
	StaticMesh* tesztModel;

	StaticMesh* m_pDropInStaticMesh;
	SkinnedMesh* dropInSkinnedMesh;

	QTime currentTime;
	glm::tvec3<double> m_dTerrainPos;
	float mouseXpos;
	float mouseYpos;
	bool m_bMouseFirstClick;

	float m_fDeltaX;
	float m_fDeltaY;

	bool leftButtonDown;

	int mouseXpos2;
	int mouseYpos2;

	QMap<int, bool> keys;
	SceneManager m_ScreenMgr;


	int RenderedTiles;

	TwBar *myBar;
	Frustum frustum;
	glm::vec3 capsulePosition;
	Shader waterShader;
	Shader modelShader;
	Water m_Water;
	CubeTexture* cubetex;
	Skybox skybox;
	QTimer* timer;
	TerrainV2* m_pMainTerrain;
	
	VertexColorShader colorShader;
	FullScreenQuad fullquad;

	int m_iScreenWidth;
	int m_iScreenHeight;

	ACTION m_currentAction;
	SUB_ACTION m_CurrentSubAction;

	bool m_DrawTerrain;
	bool m_DrawWater;
	bool m_DrawBoundingBox;
	bool m_LeftMouseDown;
	bool m_AnimatedLoaded;
	bool m_SnappingActive;
	bool m_IsGrounded;

	TexturedModelShader texturedModelShader;
	PhysXCollision m_PhysXCollision;
	glm::vec3 CameraVelocityVec;
	TranslationTool translationTool;
	RotationTool rotationTool;
	SkinnedMeshShader skinnedMeshShader;

	AIPath m_tempPath;
	unsigned int m_LastFrame2;
	long long m_StartTime;
	double m_RunningTime;
	float m_PlayerTempSpeed;
	float m_DeltaTime2;
	unsigned int m_FoliageBrushSize;
	int m_SelectedWaypointID;
	float m_FallSpeed;
	bool m_ShowStatistic;
};

#endif // GLWIDGET_H
