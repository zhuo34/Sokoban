#include "model.h"
#include <fstream>
using namespace Sokoban;

Map::Map(int x_size, int y_size, int z_size) : size({ x_size, y_size, z_size })
{
	cube_len = 1;
	map_data = new Object***[size[2]];
	for (int i = 0; i < size[2]; i++) {
		map_data[i] = new Object * *[size[1]];
		for (int j = 0; j < size[1]; j++) {
			map_data[i][j] = new Object *[size[0]];
			for (int k = 0; k < size[0]; k++) {
				map_data[i][j][k] = new EmptyCube({ k, j, i });
			}
		}
	}

	o = { 0, 0, 0 };
	
	x = { 1, 0, 0 };
	y = { 0, 0, -1 };
	z = { 0, 1, 0 };

	x_r = { 1, 0, 0 };
	y_r = { 0, 0, 1 };
	z_r = { 0, -1, 0 };

	//x = { 1, 0, 0 };
	//y = { 0, 1, 0 };
	//z = { 0, 0, 1 };

	//x_r = { 1, 0, 0 };
	//y_r = { 0, 1, 0 };
	//z_r = { 0, 0, 1 };

	dstNum = 0;
	completeNum = 0;
}

Map::~Map()
{
	cout << "~Map" << endl;
	for (int i = 0; i < size[2]; i++) {
		for (int j = 0; j < size[1]; j++) {
			for (int k = 0; k < size[0]; k++) {
				if (map_data[i][j][k]) {
					delete map_data[i][j][k];
				}
				cout << "free " << k << j << i << endl;
			}
			delete[] map_data[i][j];
			cout << "free " << j << i << endl;
		}
		delete[] map_data[i];
		cout << "free " << i << endl;
	}
	delete[] map_data;
	cout << "~Map" << endl;
}

const vector<float> Map::real_position(const vector<float>& position) const
{
	assert(position.size() == 3);
	vector<float> ret(3);
	ret[0] = o[0] + position[0] * x[0] + position[1] * y[0] + position[2] * z[0];
	ret[1] = o[1] + position[0] * x[1] + position[1] * y[1] + position[2] * z[1];
	ret[2] = o[2] + position[0] * x[2] + position[1] * y[2] + position[2] * z[2];
	return ret;
}

const vector<float> Map::map_position(const vector<float>& position) const
{
	assert(position.size() == 3);
	vector<float> ret(3);
	ret[0] = o[0] + position[0] * x_r[0] + position[1] * y_r[0] + position[2] * z_r[0];
	ret[1] = o[1] + position[0] * x_r[1] + position[1] * y_r[1] + position[2] * z_r[1];
	ret[2] = o[2] + position[0] * x_r[2] + position[1] * y_r[2] + position[2] * z_r[2];
	return ret;
}

void Map::set_object(Object* object, int x, int y, int z)
{
	assert(x < size[0] && x >= 0);
	assert(y < size[1] && y >= 0);
	assert(z < size[2] && z >= 0);
	map_data[z][y][x] = object;
}

Object* Map::get_object(int x, int y, int z) const
{
	bool x_cond = x < size[0] && x >= 0;
	bool y_cond = y < size[1] && y >= 0;
	bool z_cond = z < size[2] && z >= 0;
	if (x_cond && y_cond && z_cond) {
		return map_data[z][y][x];
	} else {
		return nullptr;
	}
	
}

void Map::register_disp_floor()
{
	float half = Sokoban::map.cube_len / 2;
	glPushMatrix();
	glTranslatef(0, 0, -half);

	float ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	float specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	float diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };

	glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);

	// enable texture
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textures[TextureID::Floor]);
	// set texture coordinary
	GLint borderPoint[4][2] = {
		{1, 1}, {1, 0}, {0, 0}, {0, 1}
	};

	// set real position vertex
	GLfloat Vertex[4][3] = {
		{ half,  half, 0}, { half, -half, 0}, {-half, -half, 0}, {-half,  half, 0}
	};
	GLfloat normal[3] = { 0, 0, 1 };

	glBegin(GL_QUADS);
	for (int k = 0; k < 4; k++)
	{
		glTexCoord2iv(borderPoint[k]);
		glNormal3fv(normal);
		glVertex3fv(Vertex[k]);
	}
	glEnd();

	// disable texture
	glDisable(GL_TEXTURE_2D);

	glPopMatrix();
}

void Map::draw() const
{
	glPushMatrix();
	float matrix[] = {
		x_r[0], y_r[0], z_r[0], o[0],
		x_r[1], y_r[1], z_r[1], o[1],
		x_r[2], y_r[2], z_r[2], o[2],
		0, 0, 0, 1
	};
	glMultMatrixf(matrix);

	// draw floor
	drawFloor();
	// draw border
	drawBorder();
	// draw each grid
	for (int i = 0; i < size[0]; i++) {
		for (int j = 0; j < size[1]; j++) {
			for (int k = 0; k < size[2]; k++) {
				if (map.get_object(i, j, k)) {
					map.get_object(i, j, k)->draw();
				}
			}
		}
	}
	glPopMatrix();
}

void Map::drawFloor() const
{
	glPushMatrix();

	for (int i = 0; i < size[1]; i++) {
		for (int j = 0; j < size[0]; j++)
		{
			glPushMatrix();
			glTranslatef(j, i, 0);

			glCallList(Sokoban::display_list[Floor_Disp]);

			glPopMatrix();
		}
	}

	glPopMatrix();
}

void Map::drawBorder() const
{
	glPushMatrix();

	// translate to the bottom
	float half = cube_len / 2;
	glTranslatef(-half, -half, -half);

	// enable texture
	glEnable(GL_TEXTURE_2D);
	// set texture coordinary
	GLint textureVertex[4][2] = {
		{0, 0}, {0, 1}, {1, 1}, {1, 0}
	};

	GLfloat bound_h = 6;
	GLfloat bound_w = cube_len * size[0];
	// border
	GLfloat Border[4][4][3] = {
		{ { 0, 0, 0 }, { 0, 0, bound_h }, { 0, bound_w, bound_h }, { 0, bound_w, 0 } },
		{ { bound_w, 0, 0 }, { bound_w, 0, bound_h }, { 0, 0, bound_h }, { 0, 0, 0 } },
		{ { 0, bound_w, 0 }, { 0, bound_w, bound_h }, { bound_w, bound_w, bound_h }, { bound_w, bound_w, 0 } },
		{ { bound_w, bound_w, 0 }, { bound_w, bound_w, bound_h }, { bound_w, 0, bound_h }, { bound_w, 0, 0 } }
	};
	GLfloat Border_norm[4][3] = {
		{ 1, 0, 0 }, { 0, 1, 0 }, { 0, -1, 0 }, { -1, 0, 0 }
	};

	// upper
	GLfloat Upper[4][3] = {
		{ 0, 0, bound_h }, { 0, bound_w, bound_h }, { bound_w, bound_w, bound_h }, { bound_w, 0, bound_h }
	};
	GLfloat Upper_norm[3] = {
		 0, 0, -1
	};
	
	// draw border
	glBindTexture(GL_TEXTURE_2D, textures[TextureID::Border]);
	glBegin(GL_QUADS);
	for (int i = 0; i < 4; i++) 
	{
		for (int k = 0; k < 4; k++)
		{
			glTexCoord2iv(textureVertex[k]);
			glNormal3fv(Border_norm[i]);
			glVertex3fv(Border[i][k]);
		}
	}
	glEnd();

	// draw upper
	glBindTexture(GL_TEXTURE_2D, textures[TextureID::Sky]);
	glBegin(GL_QUADS);
	for (int k = 0; k < 4; k++)
	{
		glTexCoord2iv(textureVertex[k]);
		glNormal3fv(Upper_norm);
		glVertex3fv(Upper[k]);
	}
	glEnd();

	// disable texture
	glDisable(GL_TEXTURE_2D);

	glPopMatrix();
}

void Map::load(string filename)
{
	ifstream in(filename);
	if (!in)
		cout << "fail to load map." << endl;

	int buffer;
	// read grids
	for (int i = size[1] - 1; i >= 0; i--) {
		for (int j = 0; j < size[0]; j++)
		{
			in >> buffer;
			ObjectID id = (ObjectID)buffer;

			delete map_data[0][i][j];
			if (id == EMPTY_ID)
				map_data[0][i][j] = new EmptyCube({ j, i, 0 });
			else if (id == CUBE_ID)
				map_data[0][i][j] = new SolidCube({ j, i, 0 });
			else if (id == WALL_ID)
				map_data[0][i][j] = new WallCube({ j, i, 0 });
			else if (id == DST_ID)
				map_data[0][i][j] = new DstCube({ j, i, 0 });
			else if (id == COMPLETE_ID)
				map_data[0][i][j] = new CompleteCube({ j, i, 0 });
		}
	}

	// read dst number
	in >> this->dstNum;
	in >> this->completeNum;

	// read init position
	int x, y;
	in >> x >> y;

	Sokoban::eye = this->real_position({ float(x), float(y), Sokoban::eye_h});

	in.close();
}