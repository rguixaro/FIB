// MyGLWidget.cpp
#include "MyGLWidget.h"
#include <iostream>
#include <stdio.h>

#define printOpenGLError() printOglError(__FILE__, __LINE__)
#define CHECK() printOglError(__FILE__, __LINE__,__FUNCTION__)
#define DEBUG() std::cout << __FILE__ << " " << __LINE__ << " " << __FUNCTION__ << std::endl;

void MyGLWidget::initializeGL() {
	// Cal inicialitzar l'ús de les funcions d'OpenGL
	initializeOpenGLFunctions();  

	glClearColor(0.5, 0.7, 1.0, 1.0); // defineix color de fons (d'esborrat)
	carregaShaders();
	creaBuffers();
	glEnable(GL_DEPTH_TEST);
	escala = 1.0f;
	rotate = 0.0f;
	OBSy = .0f;
	OBSx = .0f;
	
  
	projectTransform();
	viewTransform();

}

void MyGLWidget::creaBuffers() {
	m.load("models/HomerProves.obj");
	glGenVertexArrays(1, &VAO_model);
	glBindVertexArray(VAO_model);

	GLuint VBO_modelPos;
	glGenBuffers(1, &VBO_modelPos);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_modelPos);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*m.faces().size()*3*3, m.VBO_vertices(), GL_STATIC_DRAW);

	glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vertexLoc);
	 
	GLuint VBO_modelCol;
	glGenBuffers(1, &VBO_modelCol);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_modelCol);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*m.faces().size()*3*3, m.VBO_matdiff(), GL_STATIC_DRAW);

	glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(colorLoc);

	glm::vec3 pos[6] = {
		glm::vec3(1.,-1.,1.),
		glm::vec3(1.,-1.,-1.),
		glm::vec3(-1.,-1.,-1.),
		glm::vec3(1.,-1.,1.),
		glm::vec3(-1.,-1.,-1.),
		glm::vec3(-1.,-1.,1.)
	};
	
	glm::vec3 col[6] = {
		glm::vec3(1,0,0),
		glm::vec3(0,0,1),
		glm::vec3(0,1,0),
		glm::vec3(1,0,0),
		glm::vec3(0,1,0),
		glm::vec3(0,0,1),
	};

	glGenVertexArrays(1, &VAO_ground);
	glBindVertexArray(VAO_ground);

	GLuint VBO_groundPos;
	glGenBuffers(1, &VBO_groundPos);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_groundPos);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pos), pos, GL_STATIC_DRAW);
	
	glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vertexLoc);
	
	GLuint VBO_groundCol;
	glGenBuffers(1, &VBO_groundCol);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_groundCol);
	glBufferData(GL_ARRAY_BUFFER, sizeof(col), col, GL_STATIC_DRAW);

	glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(colorLoc);
	
	glBindVertexArray(0);
}

void MyGLWidget::modelTransform_model() {
	// Matriu de transformació de model
  	glm::mat4 transform (1.0f);
	transform = glm::rotate(transform, rotate, glm::vec3(0.0, 1.0, 0.0));
	transform = glm::scale(transform, glm::vec3(escala));
	glUniformMatrix4fv(transLoc, 1, GL_FALSE, &transform[0][0]);
}

void MyGLWidget::modelTransform_ground() {
  	// Matriu de transformació de model
  	glm::mat4 transform (1.0f);
  	transform = glm::scale(transform, glm::vec3(escala));
  	transform = glm::translate(transform, glm::vec3(Xground, .0, Zground)); 
  	glUniformMatrix4fv(transLoc, 1, GL_FALSE, &transform[0][0]);
}

void MyGLWidget::paintGL() {
	// En cas de voler canviar els paràmetres del viewport, descomenteu la crida següent i
	// useu els paràmetres que considereu (els que hi ha són els de per defecte)
	//glViewport (0, 0, ample, alt);
  
	//Esborrem el frame-buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Carreguem la transformació de model
	modelTransform_model();

	// Activem el VAO per a pintar la caseta 
	glBindVertexArray(VAO_model);

	// pintem
	glDrawArrays(GL_TRIANGLES, 0, m.faces().size()*3);

	modelTransform_ground();

	glBindVertexArray(VAO_ground);
	
	glDrawArrays(GL_TRIANGLES, 0, 6);
	
	glBindVertexArray(0);
	
}

void MyGLWidget::carregaShaders() {
	BL2GLWidget::carregaShaders();
	projLoc = glGetUniformLocation(program->programId(), "proj");
	viewLoc = glGetUniformLocation(program->programId(), "view");
	ini_camera();
}

void MyGLWidget::projectTransform() {
	glm::mat4 Proj = glm::perspective(FOV, ra, znear, zfar);
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, &Proj[0][0]);
}

void MyGLWidget::viewTransform() {
	glm::mat4 View = glm::lookAt(OBS, VRP, UP);
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &View[0][0]);
}

void MyGLWidget::ini_camera() {
	//project matrix
	FOV = float(M_PI)/2.0f;
	ra = 1.0f;
	znear = 0.4f;
	zfar = 3.0f;
	//projectTransform(); --> initializeGL();
	//view matrix
	OBS = glm::vec3(OBSx,OBSy,1);
	VRP = glm::vec3(0,0,0);
	UP = glm::vec3(0,1,0);
	//viewTransform(); --> initializeGL();
	
}

void MyGLWidget::keyPressEvent(QKeyEvent* event) {
	makeCurrent();
	switch (event->key()) {
		case Qt::Key_S: { // escalar a més gran
			escala += 0.05;
			break;
		}
		case Qt::Key_D: { // escalar a més petit
			escala -= 0.05;
			break;
		}
		case Qt::Key_R: { //rotació 45degrees
			rotate += float(M_PI)/4.0f;
			break;
		}
		case Qt::Key_H: { //traslació ground
			Xground += .1;
			break;
		}
		case Qt::Key_F: {
			Xground -= .1;
			break;
		}
		case Qt::Key_T: {
			Zground -= .1;
			break;
		}
		case Qt::Key_G: {
			Zground += .1;
			break;
		}
		case Qt::Key_Up: {
			OBSy += .1;
			OBS = glm::vec3(OBSx,OBSy,1);
			viewTransform();
			break;
		}
		case Qt::Key_Down: {
			OBSy -= .1;
			OBS = glm::vec3(OBSx,OBSy,1);
			viewTransform();
			break;
		}
		case Qt::Key_Right: {
			OBSx += .1;
			OBS = glm::vec3(OBSx,OBSy,1);
			viewTransform();
			break;
		}
		case Qt::Key_Left: {
			OBSx -= .1;
			OBS = glm::vec3(OBSx,OBSy,1);
			viewTransform();
			break;
		}		
		default: event->ignore(); break;
	}
	update();
}

int MyGLWidget::printOglError(const char file[], int line, const char func[]) 
{
    GLenum glErr;
    int    retCode = 0;

    glErr = glGetError();
    const char * error = 0;
    switch (glErr)
    {
        case 0x0500:
            error = "GL_INVALID_ENUM";
            break;
        case 0x501:
            error = "GL_INVALID_VALUE";
            break;
        case 0x502: 
            error = "GL_INVALID_OPERATION";
            break;
        case 0x503:
            error = "GL_STACK_OVERFLOW";
            break;
        case 0x504:
            error = "GL_STACK_UNDERFLOW";
            break;
        case 0x505:
            error = "GL_OUT_OF_MEMORY";
            break;
        default:
            error = "unknown error!";
    }
    if (glErr != GL_NO_ERROR)
    {
        printf("glError in file %s @ line %d: %s function: %s\n",
                             file, line, error, func);
        retCode = 1;
    }
    return retCode;
}

MyGLWidget::~MyGLWidget() {
}


