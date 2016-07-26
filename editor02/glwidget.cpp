#include "glwidget.h"
#include "MaterialManager.h"
#include <QTimer>
#include <QMimeData>

#include <stdio.h>      
#include <stdlib.h>     
#include <time.h> 
#include <QXmlStreamWriter>
#include <QFile>
#include <QTextStream>
#include <QFileDialog>
#include <QtXml>
#include <QMessageBox>


InstancingShader instancingshader;
SceneAnimator* animator;
QTime myTimer;

GLWidget::GLWidget(QWidget *parent) : QGLWidget(parent)
{
	timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(update()));
	timer->start(17);
	myTimer.start();
	m_StartTime = GetTickCount();

	srand(time(nullptr));
	setFocusPolicy(Qt::StrongFocus);
	startTimer(1000 / 60);
	setAcceptDrops(true);
	setMouseTracking(true);
	setUpdatesEnabled(true);
	setAttribute(Qt::WA_PaintOnScreen, true);
	setAttribute(Qt::WA_NativeWindow, true);

	m_pDropInStaticMesh = nullptr;
	dropInSkinnedMesh = nullptr;
	m_LeftMouseDown = false;
	m_currentAction = ACTION_NONE;
	m_CurrentSubAction = SUB_ACTION_NONE;
	m_DrawTerrain = true;
	m_DrawWater = true;
	m_DrawBoundingBox = false;
	m_FoliageBrushSize = 500;
	m_AnimatedLoaded = false;
	m_SelectedWaypointID = -1;
	m_bMouseFirstClick = true;
	m_SnappingActive = false;
	m_IsGrounded = false;
	m_DeltaTime2 = 0.0;
	m_FallSpeed = 400.0f;
	m_ShowStatistic = true;
}

GLWidget::~GLWidget()
{
	//TwTerminate();
}

QSize GLWidget::minimumSizeHint() const
{
	return QSize(500, 500);
}

QSize GLWidget::sizeHint() const
{
	return QSize(800, 600);
}

void GLWidget::initializeGL()
{
	currentTime.start();
	lastFrame = currentTime.elapsed();
	m_LastFrame2 = currentTime.elapsed();
	if (glewInit() != GLEW_OK)
		return;
	
	m_Engine->Init();

	colorShader.Init();
	shadowMapping.Init();
	texturedModelShader.Init();
	fullquad.Init();
	
	translationTool.Init();
	rotationTool.Init();
	skinnedMeshShader.Init();
	instancingshader.Init();

	m_ScreenMgr.Init();
	CreateSkybox();
	
	if (!basicShader.CreateShader("texturedModel.vs", "texturedModel.ps"))
	{
		//emit ToLog(basicShader.filePath);
		//emit ToLog(basicShader.infoLog);
	}

	m_pMainCamera = new FPSCamera(800, 600);
	m_pMainCamera->CreatePerspectiveProjection(m_iScreenWidth, m_iScreenHeight, 45.0f, 5.0f, 10000.0f);
	
	worldLoc = glGetUniformLocation(basicShader.GetShaderProgram(), "world");
	viewLoc = glGetUniformLocation(basicShader.GetShaderProgram(), "view");
	projectionLoc = glGetUniformLocation(basicShader.GetShaderProgram(), "projection");
	
	m_pMainTerrain = new TerrainV2(1024, 128, 128, 32, 32);
	m_pMainTerrain->Setup();
	m_ScreenMgr.SetTerrain(m_pMainTerrain);

	m_PhysXCollision.Initialize();
	m_PhysXCollision.CookTerrain(m_pMainTerrain->GetHeightData(), m_pMainTerrain->GetTerrainSize(), m_pMainTerrain->GetTerrainSize());

	modelShader.CreateShader("texturedModel.vs", "texturedModel.ps");
	m_Water.Setup(m_pMainTerrain->GetTerrainSize(), m_pMainTerrain->GetTerrainCellSize());

	glViewport(0, 0, 800, 600);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_STENCIL_TEST);
	
	glEnable(GL_CLIP_PLANE0);

	glClearColor(0.2f, 0.8f, 0.3f, 1.0f);
	InitAntTweakBar();
}

void GLWidget::InitAntTweakBar()
{
	TwInit(TW_OPENGL, nullptr);
	TwWindowSize(800, 600);

	myBar = TwNewBar("Statistic");

	TwAddVarRO(myBar, "CameraPosX", TW_TYPE_FLOAT, &m_pMainCamera->cameraPos.x, "");
	TwAddVarRO(myBar, "CameraPosY", TW_TYPE_FLOAT, &m_pMainCamera->cameraPos.y, "");
	TwAddVarRO(myBar, "CameraPosZ", TW_TYPE_FLOAT, &m_pMainCamera->cameraPos.z, "");

	TwAddVarRO(myBar, "MousePosOnTerrainX", TW_TYPE_DOUBLE, &m_dTerrainPos.x, "");
	TwAddVarRO(myBar, "MousePosOnTerrainY", TW_TYPE_DOUBLE, &m_dTerrainPos.y, "");
	TwAddVarRO(myBar, "MousePosOnTerrainZ", TW_TYPE_DOUBLE, &m_dTerrainPos.z, "");

	TwAddVarRO(myBar, "MousePosX", TW_TYPE_INT32, &mouseXpos2, "");
	TwAddVarRO(myBar, "MousePosY", TW_TYPE_INT32, &mouseYpos2, "");

	TwAddVarRW(myBar, "VisibleTileCount", TW_TYPE_UINT32, &m_pMainTerrain->GetRenderedTileCount(), "");

	TwDefine(" Statistic refresh=0.1 ");
}

void GLWidget::paintGL()
{
	//m_Engine->Update();
	//m_Engine->Draw();
	Update();
	Draw();
}

void GLWidget::resizeGL(int width, int height)
{
	glViewport(0, 0, width, height);
	m_pMainCamera->CreatePerspectiveProjection(width, height, 45.0f, 5.0f, 29000.0f);
	m_pMainCamera->SetScreenSize(width, height);
	m_pMainTerrain->SetViewportSize(glm::tvec4<double>(0, 0, width, height));
	TwWindowSize(width, height);

	m_iScreenWidth = width;
	m_iScreenHeight = height;
}

void GLWidget::keyPressEvent(QKeyEvent *e)
{
	keys[e->key()] = true;
	QWidget::keyPressEvent(e);
}

void GLWidget::keyReleaseEvent(QKeyEvent *e)
{
	keys[e->key()] = false; QWidget::keyReleaseEvent(e);
}


void GLWidget::timerEvent(QTimerEvent *)
{
	GLfloat cameraSpeed = m_pMainCamera->GetCameraSpeed() * deltaTime;
	if (keys[Qt::Key_W])
		CameraVelocityVec += cameraSpeed * m_pMainCamera->cameraFront;
	if (keys[Qt::Key_S])
		CameraVelocityVec -= cameraSpeed * m_pMainCamera->cameraFront;
	if (keys[Qt::Key_A])
		CameraVelocityVec -= glm::normalize(glm::cross(m_pMainCamera->cameraFront, m_pMainCamera->cameraUp)) * cameraSpeed;
	if (keys[Qt::Key_D])
		CameraVelocityVec += glm::normalize(glm::cross(m_pMainCamera->cameraFront, m_pMainCamera->cameraUp)) * cameraSpeed;

	if (keys[Qt::Key_O])
		animator->SetAnimIndex(0);

	if (keys[Qt::Key_I])
		animator->SetAnimIndex(1);

	if (keys[Qt::Key_Delete])
	{
		if (m_ScreenMgr.selectedStaticMesh != nullptr)
		{
			m_PhysXCollision.DeleteMesh(m_ScreenMgr.selectedStaticMesh->GetId());
			m_ScreenMgr.DeleteSelectedObject();
		}
	}
		
	if (keys[Qt::Key_L])
		dropInSkinnedMesh->SetAnimIndex("attack");
		//skinnedmesh->SetAnimIndex(1, true, 1.0f);
}


void GLWidget::mouseReleaseEvent(QMouseEvent* evt)
{
	switch (evt->button())
	{
	case Qt::LeftButton:
		TwMouseButton(TW_MOUSE_RELEASED, TW_MOUSE_LEFT);

		if (m_currentAction == TRANSLATE_OBJECT)
			translationTool.LeftMouseButtonReleased();

		if (m_currentAction == ROTATE_OBJECT)
			rotationTool.LeftMouseButtonReleased();

		break;

	case Qt::RightButton:
		TwMouseButton(TW_MOUSE_RELEASED, TW_MOUSE_RIGHT);
		break;
	}

	m_bMouseFirstClick = true;
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{

	switch (event->buttons())
	{
	case Qt::LeftButton:
		LeftMouseButtonPressed(event);
		TwMouseButton(TW_MOUSE_PRESSED, TW_MOUSE_LEFT);
		break;
	case Qt::RightButton:
		TwMouseButton(TW_MOUSE_PRESSED, TW_MOUSE_RIGHT);
		break;
	}
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
	event->accept();
	mouseXpos2 = event->x();
	mouseYpos2 = event->y();
	TwMouseMotion(mouseXpos2, mouseYpos2);

	if (m_currentAction == TRANSLATE_OBJECT)
	{
		translationTool.MouseMove(m_pMainCamera->GetView(), m_pMainCamera->GetProjectionMatrix(), event->x(), event->y(), m_iScreenWidth, m_iScreenHeight);
		if (m_ScreenMgr.selectedStaticMesh != nullptr)
		{
			glm::vec3 translationToolPos = translationTool.GetPosition();

			if (m_SnappingActive)
				translationToolPos = SnapCoordToGridPoint(translationToolPos);
			
			m_ScreenMgr.selectedStaticMesh->SetPosition(translationToolPos);
			m_PhysXCollision.ChangeRigidBodyPos(m_ScreenMgr.selectedStaticMesh->GetId(), translationToolPos);
			emit PositionChanged(m_ScreenMgr.selectedStaticMesh->GetPosition());
		}
	}

	if (m_currentAction == ROTATE_OBJECT)
	{
		rotationTool.MouseMove(m_pMainCamera->GetView(), m_pMainCamera->GetProjectionMatrix(), event->x(), event->y(), m_iScreenWidth, m_iScreenHeight);
		if (m_ScreenMgr.selectedStaticMesh != nullptr)
		{
			glm::vec3 pos = rotationTool.GetPosition();
			glm::vec3 pos2 = m_ScreenMgr.selectedStaticMesh->GetPosition();
			
			m_ScreenMgr.selectedStaticMesh->AddRotationX((pos.x - pos2.x)*0.5);
			m_ScreenMgr.selectedStaticMesh->AddRotationY((pos.y - pos2.y)*0.5);
			m_ScreenMgr.selectedStaticMesh->AddRotationZ((pos.z - pos2.z)*0.5);
			m_PhysXCollision.ChangeRigidBodyRot(m_ScreenMgr.selectedStaticMesh->GetId(), m_ScreenMgr.selectedStaticMesh->GetRotation());
			rotationTool.SetPosition(pos2);
			emit RotationChanged(m_ScreenMgr.selectedStaticMesh->GetRotation());
			//phcollision.ChangeRigidBodyPos(m_ScreenMgr.selectedStaticMesh->GetId(), rotationTool.GetPosition());
		}
	}

	if (event->buttons() & Qt::LeftButton)
	{
		//QPoint p = mapToGlobal(QPoint(mouseXpos2, mouseYpos2));
		if (m_currentAction == ADD_FOLIAGE)
		{
			
			if (m_CurrentSubAction == SUB_AC_ADD_FOLIAGE)
			{
				float angle = static_cast <float> (rand()) / static_cast <float> (RAND_MAX)* glm::pi<float>() * 2;
				float radius = sqrtf(static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * m_FoliageBrushSize;
				float x = m_dTerrainPos.x + radius * cos(angle);
				float y = m_dTerrainPos.z + radius * sin(angle);

				if (x >= 0 && y >= 0 && x < ((m_pMainTerrain->GetTerrainSize() - 1) * m_pMainTerrain->GetCellWidth()) && y < ((m_pMainTerrain->GetTerrainSize() - 1)  * m_pMainTerrain->GetCellWidth()))
				{
					InstancedStaticMesh* statmehs = m_ScreenMgr.GetCurrentInstancedStaticMesh();
					if (statmehs != nullptr)
					{
						statmehs->AddInstance(glm::vec3(x, m_pMainTerrain->GetHeight(x, y), y), y);
					}
				}
				
			}

			if (m_CurrentSubAction == SUB_AC_DELETE_FOLIAGE )
			{
				InstancedStaticMesh* statmehs = m_ScreenMgr.GetCurrentInstancedStaticMesh();
				
				for (int i = 0; i < statmehs->m_PositionsRotations.size(); ++i)
				{
					float dist = sqrtf((m_dTerrainPos.x - statmehs->m_PositionsRotations[i].position.x)*(m_dTerrainPos.x - statmehs->m_PositionsRotations[i].position.x) + (m_dTerrainPos.z - statmehs->m_PositionsRotations[i].position.z)*(m_dTerrainPos.z - statmehs->m_PositionsRotations[i].position.z));
					if (dist < (m_FoliageBrushSize))
					{
						statmehs->m_PositionsRotations.erase(statmehs->m_PositionsRotations.begin() + i);
						statmehs->m_TransformationMatrices.erase(statmehs->m_TransformationMatrices.begin() + i);
						statmehs->m_numInstance--;
					}
				}
			}
		}
		m_LeftMouseDown = true;
	}

	// Center the mouse position to avoid flicker
	if (event->buttons() & Qt::RightButton && m_bMouseFirstClick == false)
	{
		m_fDeltaX = event->x() - mouseXpos;
		m_fDeltaY = event->y() - mouseYpos; 

		m_pMainCamera->MouseMove(m_fDeltaX, m_fDeltaY);
		QCursor::setPos(mapToGlobal(QPoint(mouseXpos, mouseYpos)));
	}
	
	// Save the mouse position for the first time
	if (event->buttons() & Qt::RightButton && m_bMouseFirstClick)
	{
		mouseXpos = event->x();
		mouseYpos = event->y();
		m_bMouseFirstClick = false;
	}
	
}

void GLWidget::Update()
{
	// Calculate running time
	m_RunningTime = ((double)GetTickCount() - m_StartTime) / 1000.0f;
	// Calculate the elapsed time between the frames
	deltaTime = (myTimer.elapsed() - lastFrame) * 0.001f;
	lastFrame = myTimer.elapsed();
	m_DeltaTime2 = (currentTime.elapsed() - m_LastFrame2) * 0.001f;

	m_pMainCamera->Update(deltaTime);

	
	if (m_currentAction == ACTION_PLAY)
	{
		// If the user presses the play button then apply gravity
		const PxU32 flags = m_PhysXCollision.characterController->move(PxVec3(CameraVelocityVec.x, CameraVelocityVec.y - 9.81f, CameraVelocityVec.z), 0.2, deltaTime, 0, 0);
		if (flags & PxControllerFlag::eCOLLISION_DOWN)
		{
			m_IsGrounded = true;
		}
		else
		{
			m_IsGrounded = false;
			m_FallSpeed += 0.1f;
		}
	}
	else
	{
		m_PhysXCollision.characterController->move(PxVec3(CameraVelocityVec.x, CameraVelocityVec.y, CameraVelocityVec.z), 0.2, deltaTime*0.001, 0, 0);
		m_FallSpeed = 0.0f;
	}
	
	// Update the PhysX simulation
	m_PhysXCollision.Update(deltaTime);
	
	// Get the camera corrected position from the character controller
	const PxExtendedVec3& v = m_PhysXCollision.characterController->getPosition();
	m_pMainCamera->cameraPos = glm::vec3(v.x, v.y, v.z);

	CameraVelocityVec = glm::vec3(0, 0, 0);
}

float posX = 0.0f;

// Draw the scene
void GLWidget::Draw()
{
	Clear();
	glm::mat4 view = m_pMainCamera->GetView();
	glm::vec3 camPos = m_pMainCamera->GetPosition();
	glm::mat4 projection = m_pMainCamera->GetProjectionMatrix();

	frustum.ExtractFrustum(view, m_pMainCamera->GetProjectionMatrix());
	m_ScreenMgr.SetFrustum(frustum);

	m_pMainTerrain->Draw(view, projection, camPos, frustum);
	
	// Get the mouse position on the terrain to paint or edit the terrain
	if (m_pMainTerrain->PickTerrain(mouseXpos2, mouseYpos2, (glm::dmat4)view, (glm::dmat4)projection, m_dTerrainPos))
	{
		if (m_LeftMouseDown)
		{
			if (m_currentAction == PAINT_TERRAIN)
				m_pMainTerrain->PaintTerrain(deltaTime);

			if (m_currentAction == EDIT_TERRAIN)
				m_pMainTerrain->ModifyHeight();
		}
	}
	
	m_LeftMouseDown = false;

	texturedModelShader.SetProjectionMatrix(projection);
	texturedModelShader.SetViewMatrix(view);
	texturedModelShader.Set();

	glm::vec3 pos = m_dTerrainPos;
	glm::mat4 w;
	
	// Draw the placed static meshes
	m_ScreenMgr.Draw(texturedModelShader);
	m_pMainTerrain->PickTerrain(mouseXpos2, mouseYpos2, (glm::dmat4)view, (glm::dmat4)projection, m_dTerrainPos);

	// Show the selected static mesh where the ray hit the terrain
	if (m_pDropInStaticMesh != nullptr)
	{
		if (m_CurrentSubAction == ADD_MULTIPLE_OBJECT)
		{
			glm::vec3 gridPos = SnapCoordToGridPoint(pos);
			m_pDropInStaticMesh->SetPosition(gridPos);
		}

		w = glm::translate(w, m_pDropInStaticMesh->GetPosition());
		glm::vec3 rotationInRadians = (m_pDropInStaticMesh->GetRotation() * glm::pi<float>()) / 180.0f;
		w = glm::rotate(w, rotationInRadians.x, glm::vec3(1.0f, 0.0f, 0.0f));
		w = glm::rotate(w, rotationInRadians.y, glm::vec3(0.0f, 1.0f, 0.0f));
		w = glm::rotate(w, rotationInRadians.z, glm::vec3(0.0f, 0.0f, 1.0f));

		texturedModelShader.SetWorldMatrix(w);
		m_pDropInStaticMesh->Draw();
	}

	m_ScreenMgr.DrawWaypoints(texturedModelShader);
	if (m_tempPath.GetWaypoints().size() != 0)
	{
		m_ScreenMgr.DrawTempWaypoints(texturedModelShader, m_tempPath);
	}
	
	skinnedMeshShader.Set();
	m_ScreenMgr.DrawAnimatedModels(deltaTime, currentTime.currentTime().msec(), &skinnedMeshShader, view, projection);

	if (m_DrawWater)
	{
		m_Water.Draw(view, projection, cubetex->GetID(), camPos, m_DeltaTime2, currentTime.second());
	}
	
	colorShader.SetProjectionMatrix(projection);
	colorShader.SetViewMatrix(view);
	
	colorShader.Set();
	glm::mat4 mMatrix;

	//if (m_bDrawBoundingBox)
	//{
		m_ScreenMgr.DrawBoundingBoxes(colorShader);
	//}
	
	if (m_pDropInStaticMesh != nullptr)
	{
		colorShader.SetWorldMatrix(w);
		m_pDropInStaticMesh->GetModelData()->boundingbox.Draw();
	}

	skybox.Draw(view, projection, m_pMainCamera->cameraPos);

	instancingshader.SetProjectionMatrix(projection);
	instancingshader.SetViewMatrix(view);
	instancingshader.Set();
	m_ScreenMgr.DrawInstancedModels();

	colorShader.SetProjectionMatrix(projection);
	colorShader.SetViewMatrix(view);

	colorShader.Set();
	
	if (m_currentAction == TRANSLATE_OBJECT)
	{
		glDisable(GL_DEPTH_TEST);
		translationTool.Update(camPos);
		mMatrix = glm::translate(mMatrix, translationTool.GetPosition());
		mMatrix = glm::scale(mMatrix, translationTool.GetScale());
		colorShader.SetWorldMatrix(mMatrix);
		translationTool.Draw();
		glEnable(GL_DEPTH_TEST);
	}

	if (m_currentAction == ROTATE_OBJECT)
	{
		glDisable(GL_DEPTH_TEST);
		rotationTool.Update(camPos);
		mMatrix = glm::translate(mMatrix, rotationTool.GetPosition());
		mMatrix = glm::scale(mMatrix, rotationTool.GetScale());
		colorShader.SetWorldMatrix(mMatrix);
		rotationTool.Draw();
		glEnable(GL_DEPTH_TEST);
	}

	if (m_ShowStatistic)
		TwDraw();
	
}

void GLWidget::dragMoveEvent(QDragMoveEvent* evt)
{
	evt->accept();
	//terrainPos = V2Terrain->PickTerrain(evt->pos().x(), evt->pos().y(), camera->GetView(), projection);
	mouseXpos2 = evt->pos().x();
	mouseYpos2 = evt->pos().y();

	if (m_AnimatedLoaded)
	{
		dropInSkinnedMesh->SetPosition(glm::vec3(m_dTerrainPos.x, m_dTerrainPos.y, m_dTerrainPos.z));
	}
	else
	{
		glm::vec3 gridPos = m_dTerrainPos;

		if (m_SnappingActive)
			gridPos = SnapCoordToGridPoint(m_dTerrainPos);

		m_pDropInStaticMesh->SetPosition(glm::vec3(gridPos.x, gridPos.y, gridPos.z));
		emit PositionChanged(glm::vec3(gridPos.x, gridPos.y, gridPos.z));
	}
	

	/*if (m_resourceManager->GetTerrain() != nullptr)
	{
		if (m_resourceManager->GetTerrain()->isHitted(evt->pos().x(), evt->pos().y(), v3, width(), height()))
		{
			m_renderDevice->SetPickedMeshPosition(v3);
		}
	}*/

	//scrManager->SetPickedMesh(comp);
}


void GLWidget::dropEvent(QDropEvent * evt)
{
	QString pos = evt->mimeData()->text();
	std::string utf8_text = (pos.toUtf8().constData());

	if (m_AnimatedLoaded)
	{
		m_ScreenMgr.AddAnimatedmesh(dropInSkinnedMesh);
		m_AnimatedLoaded = false;
	}
	else
	{
		m_ScreenMgr.AddObject(m_pDropInStaticMesh);
		m_PhysXCollision.CookMesh(
			(void*)&m_pDropInStaticMesh->GetModelData()->GetVertices(),
			(void*)&m_pDropInStaticMesh->GetModelData()->GetIndices(), 
			m_pDropInStaticMesh->GetModelData()->GetVertexCount(), 
			m_pDropInStaticMesh->GetModelData()->GetIndexCount(), 
			m_pDropInStaticMesh->GetPosition(), m_pDropInStaticMesh->GetId(), m_pDropInStaticMesh->GetRotation());
	}
	
	if (m_CurrentSubAction == ADD_MULTIPLE_OBJECT)
	{
		
	}
	else
	{
		m_pDropInStaticMesh = nullptr;
		dropInSkinnedMesh = nullptr;
	}
	
	/*if (m_resourceManager->GetTerrain() != nullptr)
	{
		Entity* ent = new Entity("utf8_text" + index);

		if (sModel)
		{

			ent->AddComponent(staticMeshComponent);
			staticMeshComponent->position = v3;

			ent->SetPosition(v3);
			m_screenManager->AddStaticEntity((index + "fd"), ent);
		}

		if (animModel)
		{

			ent->AddComponent(animatedMeshComponent);
			ent->AddComponent(new CreatureAIComponent());
			animatedMeshComponent->position = v3;
			animatedMeshComponent->right = Vector3(-1, 0, 0);
			m_screenManager->AddAnimatedEntity((index + "fd"), ent);
		}



		//resManager->GetTerrain()->isHitted(evt->pos().x(), evt->pos().y(), v3, width(), height());




		emit GameObjectAdded();
		index++;
		m_screenManager->SetPickedMesh(NULL);
	}
	*/
}

// Load the selected model when the mouse enters the widget
void GLWidget::dragEnterEvent(QDragEnterEvent * evt)
{
	evt->acceptProposedAction();
	QString pos = evt->mimeData()->text();
	std::string filePath = (pos.toUtf8().constData());

	if (pos.contains("Animated"))
	{
		dropInSkinnedMesh = ModelManager::LoadAnimatedMesh(filePath.c_str());
		m_AnimatedLoaded = true;
	}
	else
	{
		m_ScreenMgr.selectedStaticMesh = nullptr;
		m_pDropInStaticMesh = ModelManager::LoadStaticModel(filePath.c_str());
	}
}

void GLWidget::WireFrameModeToggled()
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void GLWidget::SolidModeToggled()
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void GLWidget::FilterTerrainToggled()
{
	m_pMainTerrain->filter3x3();
}

void GLWidget::Clear()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void GLWidget::ClearDepth()
{
	glClear( GL_DEPTH_BUFFER_BIT);
}

void GLWidget::ModifyTerrainToggled()
{
	m_currentAction = EDIT_TERRAIN;
	m_pMainTerrain->ShowCircleBrush(true);
	m_pMainTerrain->SetCircleBrushSize((float)m_pMainTerrain->GetEditBrushRadius() / m_pMainTerrain->GetTerrainSize());
}

void GLWidget::PaintTerrainToggled()
{
	m_currentAction = PAINT_TERRAIN;
	m_pMainTerrain->ShowCircleBrush(true);
	m_pMainTerrain->SetCircleBrushSize(((float)m_pMainTerrain->GetPaintBrushRadius() /m_pMainTerrain->GetTerrainSize()));
}

void GLWidget::SetTerrainBrushRadius(int value)
{
	m_pMainTerrain->SetEditBrushRadius(value);
	m_pMainTerrain->SetCircleBrushSize((float)value / m_pMainTerrain->GetTerrainSize());
}

void GLWidget::ChangeTerrainViewDistance(int value)
{
	m_pMainTerrain->SetTerrainViewDistance(value);
}

void GLWidget::ChangeCameraSpeed(int value)
{
	m_pMainCamera->SetCameraSpeed(value );
}

void GLWidget::SetTerrainBrushHeight(int value)
{
	m_pMainTerrain->SetEditBrushHeight(value+32768);
}

void GLWidget::SetDrawTerrain(bool val)
{
	m_pMainTerrain->SetVisible(val);
}

void GLWidget::SetDrawWater()
{
	m_DrawWater = !m_DrawWater;
}

void GLWidget::SetTerrainBrushStrength(int value)
{
	m_pMainTerrain->SetEditBrushStrength(value);
}

void GLWidget::SetDrawBoundingBox()
{
	m_DrawBoundingBox = !m_DrawBoundingBox;
}

void GLWidget::CookTerrain()
{
	m_PhysXCollision.ReCookTerrain(m_pMainTerrain->GetHeightData(), m_pMainTerrain->GetTerrainSize(), m_pMainTerrain->GetTerrainSize(), m_pMainTerrain->GetCellWidth());
}

void GLWidget::SelectToolToggled()
{
	m_currentAction = SELECT_OBJECT;
	m_pMainTerrain->ShowCircleBrush(false);
}

void GLWidget::TranslationToolToggled()
{
	m_currentAction = TRANSLATE_OBJECT;
	m_pMainTerrain->ShowCircleBrush(false);

	if (m_ScreenMgr.selectedStaticMesh != nullptr)
	{
		translationTool.SetPosition(m_ScreenMgr.selectedStaticMesh->GetPosition());
	}
}

void GLWidget::AddWaypointToggled(bool val)
{
	if (val)
	{
		m_currentAction = ADD_WAYPOINT;
	}
	else
	{
		if (m_tempPath.GetWaypoints().size() != 0)
		{
			m_ScreenMgr.AddAIPAth(m_tempPath);
			m_tempPath.GetWaypoints().clear();
			
		}
		emit ListWaypoints(m_ScreenMgr.GetAIPath());

		m_currentAction = ACTION_NONE;
	}
}

void GLWidget::SetTerrainPaintBrushRadius(int value)
{
	m_pMainTerrain->SetPaintBrushRadius(value);
	m_pMainTerrain->SetCircleBrushSize((float)value / m_pMainTerrain->GetTerrainSize());
}

void GLWidget::SetTerrainPaintBrushStrength(int value)
{
	m_pMainTerrain->SetPaintBrushStrength(value);
}

void GLWidget::AddFoliageToggled()
{
	m_pMainTerrain->ShowCircleBrush(true);
	m_pMainTerrain->SetCircleBrushSize((float)m_FoliageBrushSize / (m_pMainTerrain->GetTerrainSize()*m_pMainTerrain->GetTerrainCellSize()));
	m_currentAction = ADD_FOLIAGE;
	m_CurrentSubAction = SUB_AC_ADD_FOLIAGE;
}

TerrainV2* GLWidget::GetTerrain()
{
	return m_pMainTerrain;
}

void GLWidget::Texture_1_RepeatChanged(int value)
{
	m_pMainTerrain->SetTextureLayer_1_Repeat(value);
}

void GLWidget::Texture_2_RepeatChanged(int value)
{
	m_pMainTerrain->SetTextureLayer_2_Repeat(value);
}

void GLWidget::Texture_3_RepeatChanged(int value)
{
	m_pMainTerrain->SetTextureLayer_3_Repeat(value);
}

void GLWidget::Texture_4_RepeatChanged(int value)
{
	m_pMainTerrain->SetTextureLayer_4_Repeat(value);
}

void GLWidget::SetWaypointVisibility(unsigned int id, bool val)
{
	m_ScreenMgr.SetWaypointVisibliy(id, val);
}

void GLWidget::SetWaypointClosed(bool val)
{
	m_tempPath.SetClosed(val);
}

void GLWidget::CreateTerrain(unsigned int TerrainSize, unsigned int CellWidth)
{
	if (m_pMainTerrain != nullptr)
	{
		m_pMainTerrain->Release();
		delete m_pMainTerrain;
	}

	m_pMainTerrain = new TerrainV2(TerrainSize, 128, 128, CellWidth, CellWidth);
	m_pMainTerrain->Setup();
	m_pMainTerrain->SetViewportSize(glm::tvec4<double>(0, 0, m_iScreenWidth, m_iScreenHeight));
	m_PhysXCollision.ReCookTerrain(m_pMainTerrain->GetHeightData(), m_pMainTerrain->GetTerrainSize(), m_pMainTerrain->GetTerrainSize(), m_pMainTerrain->GetCellWidth());
	m_Water.Resize(m_pMainTerrain->GetTerrainSize(), m_pMainTerrain->GetCellWidth());
	m_ScreenMgr.ClearScene();
	m_ScreenMgr.SetTerrain(m_pMainTerrain);

	translationTool.SetGridSnapWidth(8);
	translationTool.SetGridSnapHeight(8);
	translationTool.SetGridSnapDepth(8);

	m_Water.SetDefaultValues();
}

void GLWidget::PlayToggled(bool val)
{
	if (val == true)
	{
		m_currentAction = ACTION_PLAY;
		m_PlayerTempSpeed = m_pMainCamera->GetCameraSpeed();
		m_pMainCamera->SetCameraSpeed(200);
		if (m_pMainTerrain->GetHeightModified())
		{
			m_PhysXCollision.ReCookTerrain(m_pMainTerrain->GetHeightData(), m_pMainTerrain->GetTerrainSize(), m_pMainTerrain->GetTerrainSize(), m_pMainTerrain->GetCellWidth());
			m_pMainTerrain->SetHeightModified(false);
		}
		
		m_pMainTerrain->ShowCircleBrush(false);

		if (m_ScreenMgr.selectedStaticMesh != nullptr)
			m_ScreenMgr.selectedStaticMesh->m_bDrawBoundingBox = false;
	}
	else
	{
		m_currentAction = ACTION_NONE;
		m_pMainCamera->SetCameraSpeed(m_PlayerTempSpeed);
	}
}

void GLWidget::AddMultipleObject(bool val)
{
	if (val)
	{
		m_CurrentSubAction = ADD_MULTIPLE_OBJECT;
	}
	else
	{
		m_CurrentSubAction = SUB_ACTION_NONE;
		m_pDropInStaticMesh = nullptr;
	}
}

bool GLWidget::AddFoliage(std::string filePath, unsigned int& id)
{
	InstancedStaticMesh* instMesh = ModelManager::LoadInstancedStaticModel(filePath.c_str());
	if (instMesh != nullptr)
	{
		id = m_ScreenMgr.AddInstancedModel(instMesh);
		return true;
	}

	return false;
}

void GLWidget::SelectInstancedMesh(unsigned int index, bool& cull, bool& zwrite, bool& blend)
{
	m_ScreenMgr.SelectInstancedStaticMesh(index);
	if (index != -1)
	{
		cull = m_ScreenMgr.GetCurrentInstancedStaticMesh()->CullFaceEnabled;
		zwrite = m_ScreenMgr.GetCurrentInstancedStaticMesh()->ZWriteEnabled;
		blend = m_ScreenMgr.GetCurrentInstancedStaticMesh()->BlendEnabled;
	}
}

void GLWidget::SetInstancedMeshCulling(bool val)
{
	InstancedStaticMesh* statMesh = m_ScreenMgr.GetCurrentInstancedStaticMesh();
	if (statMesh != nullptr)
	{
		statMesh->CullFaceEnabled = val;
	}
}

void GLWidget::SetInstancedMeshZWrite(bool val)
{
	InstancedStaticMesh* statMesh = m_ScreenMgr.GetCurrentInstancedStaticMesh();
	if (statMesh != nullptr)
	{
		statMesh->ZWriteEnabled = val;
	}
}

void GLWidget::SetInstancedMeshBlend(bool val)
{
	InstancedStaticMesh* statMesh = m_ScreenMgr.GetCurrentInstancedStaticMesh();
	if (statMesh != nullptr)
	{
		statMesh->BlendEnabled = val;
	}
}

void GLWidget::SaveMap(QString ChoosenPath)
{
	QFileInfo fileInfo(ChoosenPath);

	QString pathName = fileInfo.path();
	QString fileName = fileInfo.fileName();

	std::string pathString = pathName.append("/").toUtf8();
	std::string fileString = fileName.toUtf8();

	QDir dirr(pathName);
	QString dataDirPath = fileName.split(".").first().append("_Data");
	dirr.mkdir(dataDirPath);
	dataDirPath = dataDirPath.append("/");

	std::string DataPathStr = (pathName + dataDirPath).toUtf8();
	m_pMainTerrain->SaveTerrain(DataPathStr.c_str(), fileString.c_str());

	std::vector<ModelData> data = m_ScreenMgr.GetStaticModelData();

	QFile file(ChoosenPath);
	file.open(QIODevice::WriteOnly);

	QXmlStreamWriter xmlWriter(&file);
	xmlWriter.setAutoFormatting(true);
	xmlWriter.writeStartDocument();
	xmlWriter.writeStartElement("DATA");
	xmlWriter.writeTextElement("DATA_DIR", dataDirPath.split("/").first());

	xmlWriter.writeStartElement("TERRAIN_CELL_SIZE");
	xmlWriter.writeAttribute("SIZE", std::to_string(m_pMainTerrain->GetCellWidth()).c_str());
	xmlWriter.writeEndElement();

	xmlWriter.writeStartElement("STATIC_MODEL_DATA");

	for (int i = 0; i < data.size(); i++)
	{
		xmlWriter.writeStartElement("STATIC_MODEL");
		xmlWriter.writeAttribute("PATH", data[i].path.c_str());
		xmlWriter.writeAttribute("POS", (std::to_string(data[i].pos.x) + " " + std::to_string(data[i].pos.y) + " " + std::to_string(data[i].pos.z)).c_str());
		xmlWriter.writeAttribute("ROT", (std::to_string(data[i].rot.x) + " " + std::to_string(data[i].rot.y) + " " + std::to_string(data[i].rot.z)).c_str());
		xmlWriter.writeEndElement();
	}

	xmlWriter.writeEndElement();
	xmlWriter.writeStartElement("INSTANCED_MODEL_DATA");
	std::vector<InstancedStaticMesh*> instancedModels = m_ScreenMgr.GetInstancedMeshes();

	for (int i = 0; i < instancedModels.size(); i++)
	{
		xmlWriter.writeStartElement("INSTANCED_MODEL");
		xmlWriter.writeAttribute("PATH", instancedModels[i]->FilePath.c_str());
		for (int j = 0; j < instancedModels[i]->m_PositionsRotations.size(); j++)
		{
			glm::vec3 pos = instancedModels[i]->m_PositionsRotations[j].position;
			xmlWriter.writeStartElement("TRANSFORM");
			xmlWriter.writeAttribute("POS", (std::to_string(pos.x) + " " + std::to_string(pos.y) + " " + std::to_string(pos.z)).c_str());
			xmlWriter.writeAttribute("ANGLE", (std::to_string(instancedModels[i]->m_PositionsRotations[j].angle)).c_str());
			xmlWriter.writeEndElement();
		}
		//xmlWriter.writeAttribute("POS", (std::to_string(data[i].pos.x) + " " + std::to_string(data[i].pos.y) + " " + std::to_string(data[i].pos.z)).c_str());
		xmlWriter.writeEndElement();
	}
	xmlWriter.writeEndElement();

	xmlWriter.writeStartElement("TERRAIN_LAYERS");

	xmlWriter.writeAttribute("L1_PATH", m_pMainTerrain->m_TerrainLayerPath[0].c_str());
	xmlWriter.writeAttribute("L1_REPEAT", std::to_string(m_pMainTerrain->GetLayerRepeat(0)).c_str());

	xmlWriter.writeAttribute("L2_PATH", m_pMainTerrain->m_TerrainLayerPath[1].c_str());
	xmlWriter.writeAttribute("L2_REPEAT", std::to_string(m_pMainTerrain->GetLayerRepeat(1)).c_str());

	xmlWriter.writeAttribute("L3_PATH", m_pMainTerrain->m_TerrainLayerPath[2].c_str());
	xmlWriter.writeAttribute("L3_REPEAT", std::to_string(m_pMainTerrain->GetLayerRepeat(2)).c_str());

	xmlWriter.writeAttribute("L4_PATH", m_pMainTerrain->m_TerrainLayerPath[3].c_str());
	xmlWriter.writeAttribute("L4_REPEAT", std::to_string(m_pMainTerrain->GetLayerRepeat(3)).c_str());
	xmlWriter.writeEndElement();

	// animated models
	std::vector<SkinnedMesh*> skinnedM = m_ScreenMgr.GetSkinnedMeshes();
	xmlWriter.writeStartElement("ANIMATED_MODEL_DATA");
	for (int i = 0; i < skinnedM.size(); i++)
	{
		xmlWriter.writeStartElement("ANIMATED_MODEL");
		xmlWriter.writeAttribute("PATH", skinnedM[i]->filePath.c_str());
		glm::vec3 pos = skinnedM[i]->GetPositon();
		xmlWriter.writeAttribute("POS", (std::to_string(pos.x) + " " + std::to_string(pos.y) + " " + std::to_string(pos.z)).c_str());
		xmlWriter.writeAttribute("WAY", std::to_string(skinnedM[i]->aipathID).c_str());
		xmlWriter.writeEndElement();
	}
	xmlWriter.writeEndElement(); // end animated models

	// aipath
	xmlWriter.writeStartElement("AIPATH_DATA");
	std::vector<AIPath> paths = m_ScreenMgr.GetAiPaths();

	for (int i = 0; i < paths.size(); ++i)
	{
		xmlWriter.writeStartElement("AIPATH");
		xmlWriter.writeAttribute("ID", std::to_string(paths[i].GetID()).c_str());
		std::vector<glm::vec3> positions = paths[i].GetWaypoints();
		for (int j = 0; j < positions.size(); ++j)
		{
			glm::vec3 pos = positions[j];
			xmlWriter.writeStartElement("POSITION");
			xmlWriter.writeAttribute("POS", (std::to_string(pos.x) + " " + std::to_string(pos.y) + " " + std::to_string(pos.z)).c_str());
			xmlWriter.writeEndElement();
		}
		xmlWriter.writeEndElement();
	}

	xmlWriter.writeEndElement(); // end aipath

	xmlWriter.writeStartElement("WATER_DATA"); // water data
	xmlWriter.writeAttribute("RGB", (std::to_string(m_Water.GetWaterRedColor()) + " " + std::to_string(m_Water.GetWaterGreenColor()) + " " + std::to_string(m_Water.GetWaterBlueColor())).c_str());
	xmlWriter.writeAttribute("LEVEL", (std::to_string(m_Water.GetWaterLevel())).c_str());
	xmlWriter.writeAttribute("OPACITY", (std::to_string(m_Water.GetOpacity())).c_str());
	xmlWriter.writeAttribute("SPECULAR_POWER", (std::to_string(m_Water.GetSpecularPower())).c_str());

	xmlWriter.writeEndElement(); // end water data

	xmlWriter.writeEndElement();

	xmlWriter.writeEndDocument();
	file.close();
}

void GLWidget::LoadMap(QString FileName, std::vector<std::string>& layerPaths, std::vector<unsigned int>& layersRepeat, 
	float& RColor, float& GColor, float& BColor, float& OpacityVal, float& WaterLevelVal, float& specularPower)
{

	QFile f(FileName);
	f.open(QIODevice::ReadOnly);

	QDomDocument xmlBOM;
	xmlBOM.setContent(&f);
	f.close();

	QDomElement root = xmlBOM.documentElement();

	QDomNode arg = root.firstChild().firstChild();
	QString dataPath = arg.nodeValue();

	QDomElement terrainCellSizeElement = root.firstChild().nextSibling().toElement();
	QString terrainCellSizeStr = terrainCellSizeElement.attribute("SIZE");

	QDomNodeList nodes = root.elementsByTagName("STATIC_MODEL");

	QFileInfo fileInfo(FileName);
	QString terrainFilePath = fileInfo.path().append("/").append(dataPath);
	terrainFilePath.append("/");

	QDomNodeList l1node = root.elementsByTagName("TERRAIN_LAYERS");
	QDomNode elm = l1node.at(0);
	QDomElement e = elm.toElement();
	QString L1_PATH = e.attribute("L1_PATH");
	QString L2_PATH = e.attribute("L2_PATH");
	QString L3_PATH = e.attribute("L3_PATH");
	QString L4_PATH = e.attribute("L4_PATH");

	QString L1_Repeat = e.attribute("L1_REPEAT");
	QString L2_Repeat = e.attribute("L2_REPEAT");
	QString L3_Repeat = e.attribute("L3_REPEAT");
	QString L4_Repeat = e.attribute("L4_REPEAT");
	layersRepeat.push_back(L1_Repeat.toInt());
	layersRepeat.push_back(L2_Repeat.toInt());
	layersRepeat.push_back(L3_Repeat.toInt());
	layersRepeat.push_back(L4_Repeat.toInt());

	std::vector<string> layersPAth;
	layersPAth.push_back(std::string(L1_PATH.toUtf8()));
	layersPAth.push_back(std::string(L2_PATH.toUtf8()));
	layersPAth.push_back(std::string(L3_PATH.toUtf8()));
	layersPAth.push_back(std::string(L4_PATH.toUtf8()));

	layerPaths.push_back(std::string(L1_PATH.toUtf8()));
	layerPaths.push_back(std::string(L2_PATH.toUtf8()));
	layerPaths.push_back(std::string(L3_PATH.toUtf8()));
	layerPaths.push_back(std::string(L4_PATH.toUtf8()));


	if (m_pMainTerrain != nullptr)
	{
		m_pMainTerrain->Release();
		delete m_pMainTerrain;
		 
		m_pMainTerrain = new TerrainV2();
		std::string t = terrainFilePath.toUtf8();
		m_pMainTerrain->Setup(t, 128, 128, terrainCellSizeStr.toInt(), terrainCellSizeStr.toInt(), layersPAth);
		m_pMainTerrain->SetViewportSize(glm::tvec4<double>(0, 0, m_iScreenWidth, m_iScreenHeight));
		m_pMainTerrain->SetTextureLayer_1_Repeat(L1_Repeat.toInt());
		m_pMainTerrain->SetTextureLayer_2_Repeat(L2_Repeat.toInt());
		m_pMainTerrain->SetTextureLayer_3_Repeat(L3_Repeat.toInt());
		m_pMainTerrain->SetTextureLayer_4_Repeat(L4_Repeat.toInt());

		m_Water.Resize(m_pMainTerrain->GetTerrainSize() - 1, m_pMainTerrain->GetCellWidth());
		m_ScreenMgr.SetTerrain(m_pMainTerrain);
	}

	m_ScreenMgr.ClearScene();
	for (int i = 0; i < nodes.count(); i++)
	{
		QDomNode elm = nodes.at(i);
		if (elm.isElement())
		{
			QDomElement e = elm.toElement();
			QString path =  e.attribute("PATH");
			QString pos = e.attribute("POS");
			QString rot = e.attribute("ROT");
			QStringList positions = pos.split(" ");
			QStringList rotations = rot.split(" ");

			glm::vec3 meshPos;
			meshPos.x = positions.at(0).toFloat();
			meshPos.y = positions.at(1).toFloat();
			meshPos.z = positions.at(2).toFloat();

			glm::vec3 meshRot;
			meshRot.x = rotations.at(0).toFloat();
			meshRot.y = rotations.at(1).toFloat();
			meshRot.z = rotations.at(2).toFloat();

			StaticMesh* smesh = ModelManager::LoadStaticModel(path.toUtf8());
			if (smesh != nullptr)
			{
				smesh->SetPosition(meshPos);
				smesh->SetRotation(meshRot);
				m_ScreenMgr.AddObject(smesh);
				m_PhysXCollision.CookMesh(
					(void*)&smesh->GetModelData()->GetVertices(),
					(void*)&smesh->GetModelData()->GetIndices(),
					smesh->GetModelData()->GetVertexCount(),
					smesh->GetModelData()->GetIndexCount(),
					smesh->GetPosition(), smesh->GetId(), smesh->GetRotation());
			}
		}
	}

	QDomNodeList aiPathNodes = root.elementsByTagName("AIPATH");
	for (int i = 0; i < aiPathNodes.count(); i++)
	{
		QDomNode elm = aiPathNodes.at(i);
		QString aiPathID = elm.toElement().attribute("ID");
		QDomElement ft = elm.firstChildElement();

		AIPath aipath;
		aipath.SetID(aiPathID.toUInt());
		while (!ft.isNull())
		{
			QString pos = ft.attribute("POS");
			QStringList positions = pos.split(" ");
			glm::vec3 meshPos;

			meshPos.x = positions.at(0).toFloat();
			meshPos.y = positions.at(1).toFloat();
			meshPos.z = positions.at(2).toFloat();
			aipath.AddWaypoint(meshPos);
			ft = ft.nextSibling().toElement();
		}

		m_ScreenMgr.AddAIPathFromFile(aipath);
	}

	QDomNodeList animatedModelNodes = root.elementsByTagName("ANIMATED_MODEL"); 
	for (int i = 0; i < animatedModelNodes.count(); i++)
	{
		QDomNode elm = animatedModelNodes.at(i);
		QString modelPath = elm.toElement().attribute("PATH");
		QString modelPos = elm.toElement().attribute("POS");
		QStringList positions = modelPos.split(" ");
		glm::vec3 meshPos;

		meshPos.x = positions.at(0).toFloat();
		meshPos.y = positions.at(1).toFloat();
		meshPos.z = positions.at(2).toFloat();

		int modelaiId = elm.toElement().attribute("WAY").toInt();

		SkinnedMesh* loadedSkinnedMesh = ModelManager::LoadAnimatedMesh(modelPath.toUtf8());
		loadedSkinnedMesh->aipathID = modelaiId;

		if (modelaiId != -1)
			loadedSkinnedMesh->aipath = m_ScreenMgr.GetPath(modelaiId);

		loadedSkinnedMesh->SetPosition(meshPos);
		m_ScreenMgr.AddAnimatedmesh(loadedSkinnedMesh);
	}

	// load water
	QDomNodeList waterDataNode = root.elementsByTagName("WATER_DATA");
	QDomNode waterDataElement = waterDataNode.at(0);
	QString waterRGBColor = waterDataElement.toElement().attribute("RGB");
	QStringList rgbColors = waterRGBColor.split(" ");
	m_Water.SetWaterRedColor(rgbColors.at(0).toFloat());
	m_Water.SetWaterGreenColor(rgbColors.at(1).toFloat());
	m_Water.SetWaterBlueColor(rgbColors.at(2).toFloat());

	QString waterLevel = waterDataElement.toElement().attribute("LEVEL");
	m_Water.SetWaterLevel(waterLevel.toFloat());

	QString waterOpacity = waterDataElement.toElement().attribute("OPACITY");
	m_Water.SetOpacity(waterOpacity.toFloat());

	RColor = rgbColors.at(0).toFloat();
	GColor = rgbColors.at(1).toFloat();
	BColor = rgbColors.at(2).toFloat();

	WaterLevelVal = waterLevel.toFloat();
	OpacityVal = waterOpacity.toFloat();

	QString specularPowerVal = waterDataElement.toElement().attribute("SPECULAR_POWER");
	specularPower = specularPowerVal.toFloat();
}

void GLWidget::SetDeleteFoliage(bool val)
{
	if (val)
	{
		m_CurrentSubAction = SUB_AC_DELETE_FOLIAGE;
	}
	else
	{
		m_CurrentSubAction = SUB_AC_ADD_FOLIAGE;
	}
}

void GLWidget::SetFoliageBrushSize(int val)
{
	m_FoliageBrushSize = val;
	m_pMainTerrain->SetCircleBrushSize((float)val / (m_pMainTerrain->GetTerrainSize()*m_pMainTerrain->GetTerrainCellSize()));
}

void GLWidget::SelectedWaypoint(int id)
{
	m_SelectedWaypointID = id;
}

void GLWidget::AddToWaypoint(bool val)
{
	if (val)
	{
		m_currentAction = ADD_TO_WAYPOINT;
	}
	else
	{
		m_currentAction = ACTION_NONE;
	}
}

void GLWidget::AddObjectToggled()
{
	m_pMainTerrain->ShowCircleBrush(false);
}

void GLWidget::SettingsToggled()
{
	m_pMainTerrain->ShowCircleBrush(false);
}

void GLWidget::Init(Engine* engine)
{
	m_Engine = engine;
}

void GLWidget::RotationToolToggled()
{
	m_currentAction = ROTATE_OBJECT;
	m_pMainTerrain->ShowCircleBrush(false);

	if (m_ScreenMgr.selectedStaticMesh != nullptr)
	{
		m_ScreenMgr.selectedStaticMesh->m_bDrawBoundingBox = false;
		rotationTool.SetPosition(m_ScreenMgr.selectedStaticMesh->GetPosition());
	}
}

void GLWidget::SetObjectRotationX(double val)
{
	if (m_ScreenMgr.selectedStaticMesh != nullptr)
	{
		m_ScreenMgr.selectedStaticMesh->SetRotationX(val);
		m_PhysXCollision.ChangeRigidBodyRot(m_ScreenMgr.selectedStaticMesh->GetId(), m_ScreenMgr.selectedStaticMesh->GetRotation());
	}
}

void GLWidget::SetObjectRotationY(double val)
{
	if (m_ScreenMgr.selectedStaticMesh != nullptr)
	{
		m_ScreenMgr.selectedStaticMesh->SetRotationY(val);
		m_PhysXCollision.ChangeRigidBodyRot(m_ScreenMgr.selectedStaticMesh->GetId(), m_ScreenMgr.selectedStaticMesh->GetRotation());
	}

	if (m_pDropInStaticMesh != nullptr)
	{
		m_pDropInStaticMesh->SetRotationY(val);
	}
}

void GLWidget::SetObjectRotationZ(double val)
{
	if (m_ScreenMgr.selectedStaticMesh != nullptr)
	{
		m_ScreenMgr.selectedStaticMesh->SetRotationZ(val);
		m_PhysXCollision.ChangeRigidBodyRot(m_ScreenMgr.selectedStaticMesh->GetId(), m_ScreenMgr.selectedStaticMesh->GetRotation());
	}	
}

void GLWidget::SetPositionX(double val)
{
	if (m_ScreenMgr.selectedStaticMesh != nullptr)
	{
		m_ScreenMgr.selectedStaticMesh->SetPositionX(val);
		m_PhysXCollision.ChangeRigidBodyPos(m_ScreenMgr.selectedStaticMesh->GetId(), m_ScreenMgr.selectedStaticMesh->GetPosition());
		
		if (m_currentAction == TRANSLATE_OBJECT)
		{
			translationTool.SetPositionX(val);
		}
	}
}

void GLWidget::SetPositionY(double val)
{
	if (m_ScreenMgr.selectedStaticMesh != nullptr)
	{
		m_ScreenMgr.selectedStaticMesh->SetPositionY(val);
		m_PhysXCollision.ChangeRigidBodyPos(m_ScreenMgr.selectedStaticMesh->GetId(), m_ScreenMgr.selectedStaticMesh->GetPosition());
	
		if (m_currentAction == TRANSLATE_OBJECT)
		{
			translationTool.SetPositionY(val);
		}
	}
}

void GLWidget::SetPositionZ(double val)
{
	if (m_ScreenMgr.selectedStaticMesh != nullptr)
	{
		m_ScreenMgr.selectedStaticMesh->SetPositionZ(val);
		m_PhysXCollision.ChangeRigidBodyPos(m_ScreenMgr.selectedStaticMesh->GetId(), m_ScreenMgr.selectedStaticMesh->GetPosition());
	
		if (m_currentAction == TRANSLATE_OBJECT)
		{
			translationTool.SetPositionZ(val);
		}
	}
}

void GLWidget::LeftMouseButtonPressed(QMouseEvent * event)
{
	if (m_currentAction == SELECT_OBJECT)
	{
		m_ScreenMgr.PickObject(m_pMainCamera->GetView(), m_pMainCamera->GetProjectionMatrix(), event->x(), event->y(), m_iScreenWidth, m_iScreenHeight);

		if (m_ScreenMgr.selectedStaticMesh != nullptr)
			emit PositionChanged(m_ScreenMgr.selectedStaticMesh->GetPosition());
	}

	if (m_currentAction == TRANSLATE_OBJECT)
	{
		if (!translationTool.RayToLineIsInBounds(m_pMainCamera->GetView(), m_pMainCamera->GetProjectionMatrix(), event->x(), event->y(), m_iScreenWidth, m_iScreenHeight))
		{
			m_ScreenMgr.PickObject(m_pMainCamera->GetView(), m_pMainCamera->GetProjectionMatrix(), event->x(), event->y(), m_iScreenWidth, m_iScreenHeight);
		}

		if (m_ScreenMgr.selectedStaticMesh != nullptr)
		{
			emit RotationChanged(m_ScreenMgr.selectedStaticMesh->GetRotation());

			translationTool.SetVisible(true);
			m_ScreenMgr.selectedStaticMesh->m_bDrawBoundingBox = true;
			translationTool.SetPosition(m_ScreenMgr.selectedStaticMesh->GetPosition());
			translationTool.SetRay(m_ScreenMgr.raypicker.GetRay());
			translationTool.RayToLineIsInBounds(m_pMainCamera->GetView(), m_pMainCamera->GetProjectionMatrix(), event->x(), event->y(), m_iScreenWidth, m_iScreenHeight);
			//translationTool.RayToLineIsInBounds(m_pMainCamera->GetView(), m_pMainCamera->GetProjectionMatrix(), event->x(), event->y(), m_iScreenWidth, m_iScreenHeight);
			
		}
		else
		{
			translationTool.SetVisible(false);
		}
	}

	if (m_currentAction == ROTATE_OBJECT)
	{
		if (!rotationTool.RayToLineIsInBounds(m_pMainCamera->GetView(), m_pMainCamera->GetProjectionMatrix(), event->x(), event->y(), m_iScreenWidth, m_iScreenHeight))
		{
			m_ScreenMgr.PickObject(m_pMainCamera->GetView(), m_pMainCamera->GetProjectionMatrix(), event->x(), event->y(), m_iScreenWidth, m_iScreenHeight);

		}

		if (m_ScreenMgr.selectedStaticMesh != nullptr)
		{
			rotationTool.SetVisible(true);
			m_ScreenMgr.selectedStaticMesh->m_bDrawBoundingBox = true;
			rotationTool.SetPosition(m_ScreenMgr.selectedStaticMesh->GetPosition());
			rotationTool.SetRay(m_ScreenMgr.raypicker.GetRay());
			rotationTool.RayToLineIsInBounds(m_pMainCamera->GetView(), m_pMainCamera->GetProjectionMatrix(), event->x(), event->y(), m_iScreenWidth, m_iScreenHeight);
			if (!rotationTool.RayToLineIsInBounds(m_pMainCamera->GetView(), m_pMainCamera->GetProjectionMatrix(), event->x(), event->y(), m_iScreenWidth, m_iScreenHeight))
			{

			}
		}
		else
		{
			rotationTool.SetVisible(false);
		}
	}

	if (m_currentAction == ADD_WAYPOINT)
	{
		m_tempPath.AddWaypoint(m_dTerrainPos);
	}

	if (ADD_TO_WAYPOINT == m_currentAction)
	{
		if (m_ScreenMgr.intersectRaySphere(m_pMainCamera->GetView(), m_pMainCamera->GetProjectionMatrix(), event->x(), event->y(), m_iScreenWidth, m_iScreenHeight) != -1)
		{
			if (m_SelectedWaypointID != -1)
			{
				m_ScreenMgr.selectedAnimObject.skinnedMesh->aipath = m_ScreenMgr.GetPath(m_SelectedWaypointID);
				m_ScreenMgr.selectedAnimObject.skinnedMesh->aipathID = m_SelectedWaypointID;
			}
		}
	}


	if (m_CurrentSubAction == ADD_MULTIPLE_OBJECT)
	{
		if (m_pDropInStaticMesh != nullptr)
		{
			StaticMesh* pNewMesh = new StaticMesh(m_pDropInStaticMesh->GetModelData());
			pNewMesh->SetFileName(m_pDropInStaticMesh->GetFileName());
			pNewMesh->SetRotation(m_pDropInStaticMesh->GetRotation());
			
			glm::vec3 gridPos = m_dTerrainPos;

			if (m_SnappingActive)
				gridPos = SnapCoordToGridPoint(m_dTerrainPos);
			
			pNewMesh->SetPosition(gridPos);
			m_ScreenMgr.AddObject(pNewMesh);
			m_PhysXCollision.CookMesh(
				(void*)&pNewMesh->GetModelData()->GetVertices(),
				(void*)&pNewMesh->GetModelData()->GetIndices(),
				pNewMesh->GetModelData()->GetVertexCount(),
				pNewMesh->GetModelData()->GetIndexCount(),
				pNewMesh->GetPosition(), pNewMesh->GetId(), pNewMesh->GetRotation());

		}
	}
}

void GLWidget::CreateSkybox()
{
	std::vector<char*> faces;
	faces.push_back("ely_cloudtop/right.jpg");
	faces.push_back("ely_cloudtop/left.jpg");
	faces.push_back("ely_cloudtop/top.jpg");
	faces.push_back("ely_cloudtop/bottom.jpg");
	faces.push_back("ely_cloudtop/back.jpg");
	faces.push_back("ely_cloudtop/front.jpg");
	cubetex = new CubeTexture(faces);
	cubetex->Create();
	skybox.Setup(cubetex->GetID());
}

void GLWidget::SetGridSnapWidth(int val)
{
	translationTool.SetGridSnapWidth(val);
}

void GLWidget::SetGridSnapHeight(int val)
{
	translationTool.SetGridSnapHeight(val);
}

void GLWidget::SetGridSnapDepth(int val)
{
	translationTool.SetGridSnapDepth(val);
}

glm::vec3 GLWidget::SnapCoordToGridPoint(const glm::vec3& point)
{
	unsigned int gridSnapWidth = translationTool.GetGridSnapWidth();
	unsigned int gridSnapHeight = translationTool.GetGridSnapHeight();
	unsigned int gridSnapDepth = translationTool.GetGridSnapDepth();

	glm::vec3 gridPos;
	gridPos.x = round(point.x / gridSnapWidth) * gridSnapWidth;
	gridPos.y = round(point.y / gridSnapHeight) * gridSnapHeight;
	gridPos.z = round(point.z / gridSnapDepth) * gridSnapDepth;

	return gridPos;
}

void GLWidget::ToggleSnapTool(bool val)
{
	m_SnappingActive = val;
}

void GLWidget::SetWaterLevel(int waterLevel)
{
	m_Water.SetWaterLevel(waterLevel);
}

void GLWidget::SetWaterRedColor(int val)
{
	m_Water.SetWaterRedColor(val);
}

void GLWidget::SetWaterGreenColor(int val)
{
	m_Water.SetWaterGreenColor(val);
}

void GLWidget::SetWaterBlueColor(int val)
{
	m_Water.SetWaterBlueColor(val);
}

void GLWidget::SetSpecularPower(int val)
{
	m_Water.SetSpecularPower(val);
}

void GLWidget::SetOpacity(int val)
{
	m_Water.SetOpacity(val / 100.0f);
}

void GLWidget::Release()
{
	delete m_pMainCamera;
	cubetex->Release();

	if (m_pMainTerrain != nullptr)
	{
		m_pMainTerrain->Release();
		delete m_pMainTerrain;
	}

	m_ScreenMgr.ClearScene();

}

void GLWidget::ShowStatisctic(bool val)
{
	m_ShowStatistic = val;
}



