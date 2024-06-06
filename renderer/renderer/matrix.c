#include <math.h>
#include "matrix.h"

mat4_t mat4_identity(void) {
	// | 1 0 0 0 |
	// | 0 1 0 0 |
	// | 0 0 1 0 |
	// | 0 0 0 1 |

	mat4_t m = { {
		{1, 0, 0, 0},
		{0, 1, 0, 0},
		{0, 0, 1, 0},
		{0, 0, 0, 1}
	} };
	return m;
}

mat4_t mat4_make_scale(float sx, float sy, float sz) {
	// | sx 0  0  0 |
	// | 0  sy 0  0 |
	// | 0  0  sz 0 |
	// | 0  0  0  1 |

	mat4_t m = mat4_identity();
	
	m.m[0][0] = sx;
	m.m[1][1] = sy;
	m.m[2][2] = sz;

	return m;
}

mat4_t mat4_make_rotation_x(float angle) {
	float c = cos(angle);
	float s = sin(angle);

	// | 1  0  0  0 | |x|
	// | 0  c -s  0 | |y|
	// | 0  s  c  0 | |z|
	// | 0  0  0  1 | |1|

	mat4_t m = mat4_identity();
	m.m[1][1] = c;
	m.m[1][2] = -s;
	m.m[2][1] = s;
	m.m[2][2] = c;

	return m;
}


mat4_t mat4_make_rotation_y(float angle) {
	float c = cos(angle);
	float s = sin(angle);

	// | c  0  s  0 | |x|
	// | 0  1  0  0 | |y|
	// |-s  0  c  0 | |z|
	// | 0  0  0  1 | |1|

	mat4_t m = mat4_identity();
	m.m[0][0] = c;
	m.m[0][2] = s;
	m.m[2][0] = -s;
	m.m[2][2] = c;

	return m;
}


mat4_t mat4_make_rotation_z(float angle) {
	float c = cos(angle);
	float s = sin(angle);

	// | c -s  0  0 | |x|
	// | s  c  0  0 | |y|
	// | 0  0  1  0 | |z|
	// | 0  0  0  1 | |1|

	mat4_t m = mat4_identity();
	m.m[0][0] = c;
	m.m[0][1] = -s;
	m.m[1][0] = s;
	m.m[1][1] = c;

	return m;
}



mat4_t mat4_make_translation(float tx, float ty, float tz) {
	// | 1 0 0 tx |
	// | 0 1 0 ty |
	// | 0 0 1 tz |
	// | 0 0 0 1  |

	mat4_t m = mat4_identity();

	m.m[0][3] = tx;
	m.m[1][3] = ty;
	m.m[2][3] = tz;

	return m;

}

mat4_t mat4_mul_mat4(mat4_t a, mat4_t b) {

	mat4_t result;
	int rows = 4;
	int columns = 4;
	////1st column
	//result.m[0][0] = a.m[0][0] * b.m[0][0] + a.m[0][1] * b.m[1][0] + a.m[0][2] * b.m[2][0] + a.m[0][3] * b.m[3][0];
	//result.m[1][0] = a.m[1][0] * b.m[0][0] + a.m[1][1] * b.m[1][0] + a.m[1][2] * b.m[2][0] + a.m[1][3] * b.m[3][0];
	//result.m[2][0] = a.m[2][0] * b.m[0][0] + a.m[2][1] * b.m[1][0] + a.m[2][2] * b.m[2][0] + a.m[2][3] * b.m[3][0];
	//result.m[3][0] = a.m[3][0] * b.m[0][0] + a.m[3][1] * b.m[1][0] + a.m[3][2] * b.m[2][0] + a.m[3][3] * b.m[3][0];

	////2nd column
	//result.m[0][1] = a.m[0][0] * b.m[0][1] + a.m[0][1] * b.m[1][1] + a.m[0][2] * b.m[2][1] + a.m[0][3] * b.m[3][1];
	//result.m[1][1] = a.m[1][0] * b.m[0][1] + a.m[1][1] * b.m[1][1] + a.m[1][2] * b.m[2][1] + a.m[1][3] * b.m[3][1];
	//result.m[2][1] = a.m[2][0] * b.m[0][1] + a.m[2][1] * b.m[1][1] + a.m[2][2] * b.m[2][1] + a.m[2][3] * b.m[3][1];
	//result.m[3][1] = a.m[3][0] * b.m[0][1] + a.m[3][1] * b.m[1][1] + a.m[3][2] * b.m[2][1] + a.m[3][3] * b.m[3][1];

	////3rd column
	//result.m[0][2] = a.m[0][0] * b.m[0][2] + a.m[0][1] * b.m[1][2] + a.m[0][2] * b.m[2][2] + a.m[0][3] * b.m[3][2];
	//result.m[1][2] = a.m[1][0] * b.m[0][2] + a.m[1][1] * b.m[1][2] + a.m[1][2] * b.m[2][2] + a.m[1][3] * b.m[3][2];
	//result.m[2][2] = a.m[2][0] * b.m[0][2] + a.m[2][1] * b.m[1][2] + a.m[2][2] * b.m[2][2] + a.m[2][3] * b.m[3][2];
	//result.m[3][2] = a.m[3][0] * b.m[0][2] + a.m[3][1] * b.m[1][2] + a.m[3][2] * b.m[2][2] + a.m[3][3] * b.m[3][2];

	////4th column
	//result.m[0][3] = a.m[0][0] * b.m[0][3] + a.m[0][1] * b.m[1][3] + a.m[0][2] * b.m[2][2] + a.m[0][3] * b.m[3][3];
	//result.m[1][3] = a.m[1][0] * b.m[0][3] + a.m[1][1] * b.m[1][3] + a.m[1][2] * b.m[2][2] + a.m[1][3] * b.m[3][3];
	//result.m[2][3] = a.m[2][0] * b.m[0][3] + a.m[2][1] * b.m[1][3] + a.m[2][2] * b.m[2][2] + a.m[2][3] * b.m[3][3];
	//result.m[3][3] = a.m[3][0] * b.m[0][3] + a.m[3][1] * b.m[1][3] + a.m[3][2] * b.m[2][2] + a.m[3][3] * b.m[3][3];

	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < columns; j++)
		{
			result.m[i][j] = a.m[i][0] * b.m[0][j] + a.m[i][1] * b.m[1][j] + a.m[i][2] * b.m[2][j] + a.m[i][3] * b.m[3][j];
		}
	}
	return result;

}


vect4_t mat4_mul_vect4(mat4_t m, vect4_t v) {

	vect4_t result;

	result.x = m.m[0][0] * v.x + m.m[0][1] * v.y + m.m[0][2] * v.z + m.m[0][3] * v.w;
	result.y = m.m[1][0] * v.x + m.m[1][1] * v.y + m.m[1][2] * v.z + m.m[1][3] * v.w;
	result.z = m.m[2][0] * v.x + m.m[2][1] * v.y + m.m[2][2] * v.z + m.m[2][3] * v.w;
	result.w = m.m[3][0] * v.x + m.m[3][1] * v.y + m.m[3][2] * v.z + m.m[3][3] * v.w;

	return result;

}

mat4_t mat4_make_perspective(float fov, float aspect, float znear, float zfar){

// | (h/w)*1/tan(fov/2)             0            0                 0 |
// |                  0  1/tan(fov/2)            0                 0 |
// |                  0             0   zf/(zf-zn)  (-zf*zn)/(zf-zn) |
// |                  0             0            1                 0 |

	mat4_t m = { {{0}} };
	m.m[0][0] = aspect * (1 / tan(fov / 2));
	m.m[1][1] = 1 / tan(fov / 2);
	m.m[2][2] = zfar / (zfar - znear);
	m.m[2][3] = (-zfar * znear) / (zfar - znear);
	m.m[3][2] = 1.0;

	return m;
}

vect4_t mat4_mul_vect4_project(mat4_t mat_proj, vect4_t v) {
	//multiply the projection matrix by our original vector
	vect4_t result = mat4_mul_vect4(mat_proj, v);

	//perform perspective divide with orginal z-value that is now stored in w
	if (result.w != 0.0) {
		
		result.x = result.x / result.w;
		result.y = result.y / result.w;
		result.z = result.z / result.w;
	}

	return result;

}

mat4_t mat4_look_at(vect3_t eye, vect3_t target, vect3_t up) {

	vect3_t z = vect3_sub(target, eye);  //forward direction
	vect3_normalize(&z);

	vect3_t x = vect3_cross(up, z); //right direction -> left handed system
	vect3_normalize(&x);

	vect3_t y = vect3_cross(z, x); // up direction -> left handed system

	// | x.x     x.y     x.z     -dot(x,eye) |
	// | y.x     y.y     y.z     -dot(y,eye) |
	// | z.x     z.y     z.z     -dot(z,eye) |
	// |   0       0       0              1  |

	mat4_t view_matrix = { {
		{x.x, x.y, x.z, -vect3_dot(x, eye)},
		{y.x, y.y, y.z, -vect3_dot(y, eye)},
		{z.x, z.y, z.z, -vect3_dot(z, eye)},
		{  0,   0,   0,                  1}

	} };

	return view_matrix;
}