#include "Water.h"

Water::Water()
{
	accumulator = 0;
	m_WaterLevel = 20;
	m_sign = 1;

	m_WaterRColor = 0;
	m_WaterGColor = 0.73f;
	m_WaterBColor = 0.86f;
	m_SpecularPower = 20.0f;
	m_Opacity = 0.5f;
}

void Water::Setup(unsigned int terrainSize, unsigned int cellWidth)
{
	normalMaps[0].CreateTexture("../Data/Textures/Skybox/NormalMaps/Ocean1_N.png");
	normalMaps[1].CreateTexture("../Data/Textures/Skybox/NormalMaps/Ocean1_N.png");
	normalMaps[2].CreateTexture("../Data/Textures/Skybox/NormalMaps/Ocean2_N.png");
	normalMaps[3].CreateTexture("../Data/Textures/Skybox/NormalMaps/Ocean3_N.png");

	float sizeX = terrainSize * cellWidth;
	GLfloat vertices[] = 
	{
		// Positions          // Colors           // Texture Coords
		sizeX, 0.0f, sizeX, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Top Right
		sizeX, 0.0f, -0, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,// Bottom Right
		-0, 0.0f, -0, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,// Bottom Left
		-0, 0.0f, sizeX, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f// Top Left 
	};
	GLuint indices[] = {  // Note that we start from 0!
		0, 1, 3, // First Triangle
		1, 2, 3  // Second Triangle
	};

	
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(5 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	

	glBindVertexArray(0); 

	shader.CreateShader("water2.vs", "water2.ps");
	worldLoc = glGetUniformLocation(shader.GetShaderProgram(), "world");
	viewLoc = glGetUniformLocation(shader.GetShaderProgram(), "view");
	projectionLoc = glGetUniformLocation(shader.GetShaderProgram(), "projection");
	cameraLoc = glGetUniformLocation(shader.GetShaderProgram(), "cameraPos");
	textureLerpLoc = glGetUniformLocation(shader.GetShaderProgram(), "textureLerp");
	timeLoc = glGetUniformLocation(shader.GetShaderProgram(), "time");
	t1Location = glGetUniformLocation(shader.GetShaderProgram(), "normalTex");
	t2Location = glGetUniformLocation(shader.GetShaderProgram(), "normalT");

	RColorLocation = glGetUniformLocation(shader.GetShaderProgram(), "RColor");
	GColorLocation = glGetUniformLocation(shader.GetShaderProgram(), "GColor");
	BColorLocation = glGetUniformLocation(shader.GetShaderProgram(), "BColor");

	SpecularPowerLocation = glGetUniformLocation(shader.GetShaderProgram(), "specularPower");;
	OpacityLocation = glGetUniformLocation(shader.GetShaderProgram(), "opacity");;

}


void Water::Draw(glm::mat4& view, glm::mat4& proj, GLuint skyboxID, glm::vec3& camerapos, float deltatime, int sec )
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	int oceanTexIndex = (sec % 4);
	shader.Begin();
	glActiveTexture(GL_TEXTURE0);
	
	glUniform1i(t1Location, 0);

	glBindTexture(GL_TEXTURE_2D, normalMaps[(oceanTexIndex + 1) % 4].GetID());

	glActiveTexture(GL_TEXTURE1);
	
	glUniform1i(t1Location, 1);
	glBindTexture(GL_TEXTURE_2D, normalMaps[(oceanTexIndex) % 4].GetID());
	
	if (accumulator >= 1)
		m_sign = -1;

	if (accumulator <= 0)
		m_sign = 1;

	accumulator += (deltatime*0.000005)*m_sign;
	
	glUniform1f(textureLerpLoc, accumulator);
	glUniform1f(timeLoc, deltatime*0.008f);

	glUniform1f(RColorLocation, m_WaterRColor);
	glUniform1f(GColorLocation, m_WaterGColor);
	glUniform1f(BColorLocation, m_WaterBColor);
	glUniform1f(SpecularPowerLocation, m_SpecularPower);
	glUniform1f(OpacityLocation, m_Opacity);


	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxID);
	GLuint skyboxLoc = glGetUniformLocation(shader.GetShaderProgram(), "skybox");
	glUniform1i(skyboxLoc, 2);
	world = glm::mat4();
	world = glm::translate(world, glm::vec3(0, m_WaterLevel, 0));
	glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(world));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(proj));
	glUniform3f(cameraLoc, camerapos.x, camerapos.y, camerapos.z);
	
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	glDisable(GL_BLEND);
}

void Water::Resize(unsigned int terrainSize, unsigned int cellWidth)
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	float sizeX = terrainSize * cellWidth;

	GLfloat vertices[] =
	{
		sizeX, 0.0f, sizeX, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 
		sizeX, 0.0f, -0, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		-0, 0.0f, -0, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		-0, 0.0f, sizeX, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f
	};

	GLuint indices[] = 
	{ 
		0, 1, 3,
		1, 2, 3  
	};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(5 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0); // Unbind VAO
}

void Water::SetWaterLevel(int waterLevel)
{
	m_WaterLevel = waterLevel;
}

void Water::SetWaterRedColor(unsigned int val)
{
	m_WaterRColor = val / 255.0f;
}

void Water::SetWaterGreenColor(unsigned int val)
{
	m_WaterGColor = val / 255.0f;
}

void Water::SetWaterBlueColor(unsigned int val)
{
	m_WaterBColor = val / 255.0f;
}

void Water::SetSpecularPower(float val)
{
	m_SpecularPower = val;
}

void Water::SetOpacity(float val)
{
	m_Opacity = val;
}

int Water::GetWaterLevel() const
{
	return m_WaterLevel;
}

float Water::GetWaterRedColor() const
{
	return m_WaterRColor;
}

float Water::GetWaterGreenColor() const
{
	return m_WaterGColor;
}

float Water::GetWaterBlueColor() const
{
	return m_WaterBColor;
}

float Water::GetSpecularPower() const
{
	return m_SpecularPower;
}

float Water::GetOpacity() const
{
	return m_Opacity;
}

void Water::SetDefaultValues()
{
	accumulator = 0;
	m_WaterLevel = 20;
	m_sign = 1;

	m_WaterRColor = 0;
	m_WaterGColor = 0.73f;
	m_WaterBColor = 0.86f;
	m_SpecularPower = 20.0f;
	m_Opacity = 0.5f;
}
