//Chương trình vẽ 1 hình lập phương đơn vị theo mô hình lập trình OpenGL hiện đại

#include "Angel.h"  /* Angel.h là file tự phát triển (tác giả Prof. Angel), có chứa cả khai báo includes glew và freeglut*/


// remember to prototype
void generateGeometry( void );
void initGPUBuffers( void );
void shaderSetup( void );
void display( void );
void keyboard( unsigned char key, int x, int y );


typedef vec4 point4;
typedef vec4 color4;
using namespace std;

// Số các đỉnh của các tam giác
const int NumPoints = 36;

point4 points[NumPoints]; /* Danh sách các đỉnh của các tam giác cần vẽ*/
color4 colors[NumPoints]; /* Danh sách các màu tương ứng cho các đỉnh trên*/


point4 vertices[8]; /* Danh sách 8 đỉnh của hình lập phương*/
color4 vertex_colors[8]; /*Danh sách các màu tương ứng cho 8 đỉnh hình lập phương*/

GLuint program;

GLuint model_view_loc;
GLuint projection_loc;

void initCube()
{
	// Gán giá trị tọa độ vị trí cho các đỉnh của hình lập phương
	vertices[0] = point4(-0.5, -0.5, 0.5, 1.0);
	vertices[1] = point4(-0.5, 0.5, 0.5, 1.0);
	vertices[2] = point4(0.5, 0.5, 0.5, 1.0);
	vertices[3] = point4(0.5, -0.5, 0.5, 1.0);
	vertices[4] = point4(-0.5, -0.5, -0.5, 1.0);
	vertices[5] = point4(-0.5, 0.5, -0.5, 1.0);
	vertices[6] = point4(0.5, 0.5, -0.5, 1.0);
	vertices[7] = point4(0.5, -0.5, -0.5, 1.0);

	// Gán giá trị màu sắc cho các đỉnh của hình lập phương	
	vertex_colors[0] = color4(0.0, 0.0, 0.0, 1.0); // black
	vertex_colors[1] = color4(1.0, 0.0, 0.0, 1.0); // red
	vertex_colors[2] = color4(1.0, 1.0, 0.0, 1.0); // yellow
	vertex_colors[3] = color4(0.0, 1.0, 0.0, 1.0); // green
	vertex_colors[4] = color4(0.0, 0.0, 1.0, 1.0); // blue
	vertex_colors[5] = color4(1.0, 0.0, 1.0, 1.0); // magenta
	vertex_colors[6] = color4(1.0, 1.0, 1.0, 1.0); // white
	vertex_colors[7] = color4(0.0, 1.0, 1.0, 1.0); // cyan
}
int Index = 0;
void quad(int a, int b, int c, int d)  /*Tạo một mặt hình lập phương = 2 tam giác, gán màu cho mỗi đỉnh tương ứng trong mảng colors*/
{
	colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
	colors[Index] = vertex_colors[b]; points[Index] = vertices[b]; Index++;
	colors[Index] = vertex_colors[c]; points[Index] = vertices[c]; Index++;
	colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
	colors[Index] = vertex_colors[c]; points[Index] = vertices[c]; Index++;
	colors[Index] = vertex_colors[d]; points[Index] = vertices[d]; Index++;
}
void makeColorCube(void)  /* Sinh ra 12 tam giác: 36 đỉnh, 36 màu*/

{
	quad(1, 0, 3, 2);
	quad(2, 3, 7, 6);
	quad(3, 0, 4, 7);
	quad(6, 5, 1, 2);
	quad(4, 5, 6, 7);
	quad(5, 4, 0, 1);
}
void generateGeometry( void )
{
	initCube();
	makeColorCube();
}


void initGPUBuffers( void )
{
	// Tạo một VAO - vertex array object
	GLuint vao;
    glGenVertexArrays( 1, &vao );     
    glBindVertexArray( vao );

    // Tạo và khởi tạo một buffer object
    GLuint buffer;
    glGenBuffers( 1, &buffer );
    glBindBuffer( GL_ARRAY_BUFFER, buffer );
    glBufferData( GL_ARRAY_BUFFER, sizeof(points)+sizeof(colors), NULL, GL_STATIC_DRAW );

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points), sizeof(colors), colors); 
	
}


void shaderSetup( void )
{
	// Nạp các shader và sử dụng chương trình shader
    program = InitShader( "vshader1.glsl", "fshader1.glsl" );   // hàm InitShader khai báo trong Angel.h
    glUseProgram( program );

    // Khởi tạo thuộc tính vị trí đỉnh từ vertex shader
    GLuint loc_vPosition = glGetAttribLocation( program, "vPosition" );
	glEnableVertexAttribArray( loc_vPosition );
    glVertexAttribPointer(loc_vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );

	GLuint loc_vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(loc_vColor);
	glVertexAttribPointer(loc_vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points)));
	/* Khởi tạo các tham số chiếu sáng - tô bóng*/
	point4 light_position(0.0, 0.0, 1.0, 0.0);
	color4 light_ambient(0.2, 0.2, 0.2, 1.0);
	color4 light_diffuse(1.0, 1.0, 1.0, 1.0);
	color4 light_specular(1.0, 1.0, 1.0, 1.0);

	color4 material_ambient(1.0, 0.0, 1.0, 1.0);
	color4 material_diffuse(1.0, 0.8, 0.0, 1.0);
	color4 material_specular(1.0, 0.8, 0.0, 1.0);
	float material_shininess = 100.0;

	color4 ambient_product = light_ambient * material_ambient;
	color4 diffuse_product = light_diffuse * material_diffuse;
	color4 specular_product = light_specular * material_specular;

	glUniform4fv(glGetUniformLocation(program, "AmbientProduct"), 1, ambient_product);
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);
	glUniform4fv(glGetUniformLocation(program, "SpecularProduct"), 1, specular_product);
	glUniform4fv(glGetUniformLocation(program, "LightPosition"), 1, light_position);
	glUniform1f(glGetUniformLocation(program, "Shininess"), material_shininess);

	model_view_loc = glGetUniformLocation(program , "model_view");
	projection_loc = glGetUniformLocation(program, "projection");
	glEnable(GL_DEPTH_TEST);
    glClearColor( 1.0, 1.0, 1.0, 1.0 );        /* Thiết lập màu trắng là màu xóa màn hình*/
}

GLfloat l = -1.0, r = 1.0, t = 1.0, b = -1.0 , zNear  = 0.5 , zFar = 3.0;

GLfloat radius = 1, thetal = 0, phi = 0;
GLfloat dr = 5 * M_PI / 180;
GLfloat theta[3] = { 0,0,0 };
mat4 ins;
mat4 quayBase;
GLfloat matx = 1;
GLfloat maty = 0;
GLfloat matz = 0;

void MatPhang(GLfloat x , GLfloat y , GLfloat z , mat4 mt) {
	//point4 eye(matx, maty, matz,1.0);
	point4 eye(radius*sin(thetal)*cos(phi),
			   radius*sin(thetal)*sin(phi),
			   radius*cos(thetal),
			   1.0);
	point4 at(0,0,0,1.0);
	vec4 up(0,1,0,1.0);

	mat4 v = LookAt(eye, at, up);
	mat4 in = Scale(x,y,z);
	glUniformMatrix4fv(model_view_loc,1,GL_TRUE, v * quayBase * mt * in);

	mat4 p = Frustum(l, r, b, t, zNear, zFar );

	glUniformMatrix4fv(projection_loc, 1, GL_TRUE, p);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}


GLfloat quayCanh = 0;

GLfloat zKhungTD = 0.05, yKhungTD = 0.05, xKhungTD = 0.6;
GLfloat zKhungTP = 0.05, yKhungTP = 0.6, xKhungTP = 0.05;
GLfloat daiCanh = 0.25, rongCanh = 0.1, dayCanh = 0.05;
GLfloat yTru = 0.8, zTru = 0.1, xTru = 0.1;



void khungQuat() {
	//Thanh ngang tren
	ins = Translate(0, 0.5 * yKhungTD + 0.5 * yKhungTP, 0.5 * zTru);
	MatPhang(xKhungTD, yKhungTD, zKhungTD, ins);
	//Thanh ngang duoi
	ins = Translate(0, -0.5 * yKhungTD + -0.5 * yKhungTP, 0.5 * zTru);
	MatPhang(xKhungTD, yKhungTD, zKhungTD, ins);
	//thanh doc ben phai
	ins = Translate(0.5 * yKhungTP - 0.5 * xKhungTP, 0, 0.5 * zTru);
	MatPhang(xKhungTP, yKhungTP, zKhungTP, ins);
	//thanh doc ben trai
	ins = Translate(-0.5 * yKhungTP + 0.5* xKhungTP , 0, 0.5 * zTru);
	MatPhang(xKhungTP, yKhungTP, zKhungTP, ins);
}
void _3CanhQuat() {
	// cánh1
	ins = RotateZ(quayCanh) * Translate(0.5 * daiCanh, 0, 0.5 * zTru);
	MatPhang(daiCanh, rongCanh , dayCanh , ins);
	// cánh2
	ins = RotateZ(quayCanh) * RotateZ(120) * Translate(0.5 * daiCanh, 0,  0.5 * zTru);
	MatPhang(daiCanh, rongCanh, dayCanh, ins);
	// cánh2
	ins = RotateZ(quayCanh) *  RotateZ(240) * Translate(0.5 * daiCanh, 0, 0.5 * zTru);
	MatPhang(daiCanh, rongCanh, dayCanh, ins);
}

void truQuat() {
	ins = Translate(0, - 0.5 * yKhungTP + - 0.6 * rongCanh ,0);
	MatPhang(xTru, yTru, zTru, ins);
}
void deQuat() {
	ins = RotateZ(90) * Translate(-yTru - 0.5 * xTru, 0, 0);
	MatPhang(xTru, yTru -0.2, zTru, ins);
}
void Quat() {
	khungQuat();
	_3CanhQuat();
	truQuat();
	deQuat();
}
void Quat2() {
	khungQuat();
	_3CanhQuat();
	truQuat();
	deQuat();
}
void Quat3() {
	khungQuat();
	_3CanhQuat();
	truQuat();
	deQuat();
}

void giaDo() {

}
void display( void )
{
    glClear( GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT ); 
	quayBase = RotateX(theta[0]) * RotateY(theta[1]) * RotateZ(theta[2]);
    Quat();
	quayBase *= Translate(0.1 + xKhungTD , 0, 0);
	Quat2();
	quayBase *= Translate(-xKhungTD * 2 - 0.2 , 0, 0);
    Quat3();

	glutSwapBuffers();									   
}



void keyboard(unsigned char key, int x, int y)
{
	// keyboard handler

	switch (key) {
		case 'x':{
			theta[0] += 5;
			break;
		}
		case 'y':{
			theta[1] += 5;
			break;		
		}
		case 'z':{
			theta[2] += 5;
			break;	
		}
		case 'q': {
			quayCanh += 160;
			break;
		}
		case '1': l *= 1.1; r *= 1.1; break;
		case '2': l *= 0.9; r *= 0.9; break;
		case '3': b *= 1.1; t *= 1.1; break;
		case '4': b *= 0.9; t *= 0.9; break;
		case '5': zNear *= 1.1; zFar *= 1.1; break;
		case '6': zNear *= 0.9; zFar *= 0.9; break;
		case 'w': {
			matx += 0.02;
			break;
		}
		case 'r': radius *= 2.0; break;
		case 'R': radius *= 0.5; break;
		case 't': thetal += dr; break;
		case 'T': thetal -= dr; break;
		case 'p': phi += dr; break;
		case 'P': phi -= dr; break;
		case ' ': // reset values to their defaults
			l = -1.0;
			r = 1.0;
			b = -1.0;
			t = 1.0;
			zNear = 0.5;
			zFar = 3.0;
			radius = 1.0;
			thetal = 0.0;
			phi = 0.0;
			break;
	}
	glutPostRedisplay();
}


int main( int argc, char **argv )
{
	// main function: program starts here

    glutInit( &argc, argv );                       
    glutInitDisplayMode( GLUT_DOUBLE|GLUT_RGBA);
    glutInitWindowSize( 640, 640 );                 
	glutInitWindowPosition(100,150);               
    glutCreateWindow( "Drawing a Cube" );            

   
	glewInit();										

    generateGeometry( );                          
    initGPUBuffers( );							   
    shaderSetup( );                               

    glutDisplayFunc( display );                   
    glutKeyboardFunc( keyboard );                  

	glutMainLoop();
    return 0;
}
