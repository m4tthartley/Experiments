
#pragma warning(disable: 4244 4047 4024 4267 4133)

#include "w:/libs/platform.c"

#include "graphics.c"

int2 make_int2(int x, int y) {
	int2 result = {x, y};
	return result;
}

float2 make_float2(float x, float y) {
	float2 result = {x, y};
	return result;
}

float3 make_float3(float x, float y, float z) {
	float3 result = {x, y, z};
	return result;
}

float4 make_float4(float x, float y, float z, float w) {
	float4 result = {x, y, z, w};
	return result;
}

float4 make_float4_from_float3(float3 f, float w) {
	float4 result = {f.x, f.y, f.z, w};
	return result;
}

float float2_dist(float2 a, float2 b) {
	float2 f = {a.x-b.x, a.y-b.y};
	float result = sqrtf(f.x*f.x + f.y*f.y);
	return result;
}

#define PI 3.14159265359f
#define PI2 (3.14159265359f*2.0f)
void make_perspective_matrix(float *out, float fov, float aspect, float near, float far) {
	float f = 1.0f / tanf((fov/180.0f*PI) / 2.0f);
	float mat[] = {
		f / aspect, 0, 0, 0,
		0, f, 0, 0,
		0, 0, (far + near) / (near - far), -1,
		0, 0, (2.0f * far * near) / (near - far), 0,
		/*f / aspect, 0, 0, 0,
		0, 0, f, -1,
		0, (far + near) / (near - far), 0, 0,
		0, (2.0f * far * near) / (near - far), 0, 0,*/
	};
	memcpy(out, mat, sizeof(float)*16);
}

mat4 mat4_identity() {
	mat4 result = {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1,
	};
	return result;
}

void mat4_apply_mul(float *out, mat4 mat2) {
	float mat1[16];
	memcpy(mat1, out, sizeof(float)*16);

	for (int i = 0; i < 16; ++i) out[i] = 0;

	for (int row = 0; row < 4; ++row) {
		for (int col = 0; col < 4; ++col) {
			for (int i = 0; i < 4; ++i) {
				out[row*4 + col] += mat1[row*4 + i] * mat2.e[i*4 + col];
			}
		}
	}
}

mat4 mat4_mul(mat4 m1, mat4 m2) {
	mat4 out = {0};

	for (int row = 0; row < 4; ++row) {
		for (int col = 0; col < 4; ++col) {
			for (int i = 0; i < 4; ++i) {
				out.e[row*4 + col] += m1.e[row*4 + i] * m2.e[i*4 + col];
			}
		}
	}
}

mat4 mat4_apply_translation(mat4 *m, float3 pos) {
	mat4 result = {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		pos.x, pos.y, pos.z, 1,
	};
	mat4_apply_mul(m, result);
}

void mat4_apply_rotate_x(mat4 *m, float rads) {
	mat4 result = {
		1, 0,          0,           0,
		0, cosf(rads), -sinf(rads), 0,
		0, sinf(rads), cosf(rads),  0,
		0, 0,          0,           1,
	};
	mat4_apply_mul(m, result);
}
void mat4_apply_rotate_y(mat4 *m, float rads) {
	mat4 result = {
		cosf(rads),  0, sinf(rads), 0,
		0,           1, 0,          0,
		-sinf(rads), 0, cosf(rads), 0,
		0,           0, 0,          1,
	};
	mat4_apply_mul(m, result);
}
void mat4_apply_rotate_z(mat4 *m, float rads) {
	mat4 result = {
		cosf(rads), -sinf(rads), 0, 0,
		sinf(rads), cosf(rads),  0, 0,
		0,          0,           1, 0,
		0,          0,           0, 1,
	};
	mat4_apply_mul(m, result);
}

// todo: this seems terribly inefficient
mat4_apply_euler(mat4 *m, float3 rotation) {
	mat4_apply_rotate_x(m, rotation.x);
	mat4_apply_rotate_y(m, rotation.y);
	mat4_apply_rotate_z(m, rotation.z);
}

mat4 euler_to_mat4(float3 euler) {
	mat4 result = mat4_identity();
	mat4_apply_euler(&result, euler);
	return result;
}

void float4_apply_mat4(float *out, float *mat) {
	float4 ff = *(float4*)out;
	//ZeroStruct(ff);
	memset(out, 0, sizeof(float)*4);
	float *f = &ff;

	/*for (int row = 0; row < 4; ++row)*/ {
		for (int col = 0; col < 4; ++col) {
			for (int i = 0; i < 4; ++i) {
				out[col] += f[i] * /*mat[col*4 + i]*/mat[i*4 + col];
			}
		}
	}
}

void float4_apply_perspective(float *out, float *mat) {
	float4 ff = *(float4*)out;
	memset(out, 0, sizeof(float)*4);
	float *f = &ff;

	for (int col = 0; col < 4; ++col) {
		for (int i = 0; i < 4; ++i) {
			out[col] += f[i] * mat[i*4 + col];
		}
	}

	out[0] /= out[3];
	out[1] /= out[3];
	out[2] /= out[3];
}

quaternion quaternion_identity() {
	quaternion result = {0.0f, 0.0f, 0.0f, 1.0f};
	return result;
}

quaternion quaternion_mul(quaternion q1, quaternion q2) {
	quaternion q;
	q.w = q1.w*q2.w - q1.x*q2.x - q1.y*q2.y - q1.z*q2.z;
	q.x = q1.w*q2.x + q1.x*q2.w + q1.y*q2.z - q1.z*q2.y;
	q.y = q1.w*q2.y - q1.x*q2.z + q1.y*q2.w + q1.z*q2.x;
	q.z = q1.w*q2.z + q1.x*q2.y - q1.y*q2.x + q1.z*q2.w;
	return q;
}

// change shouldn't be a quaternion cause it isn't a real one
// should just use seperate parameters instead
void quaternion_apply(quaternion *q, float3 axis, float angle) {
	quaternion local;
	local.w = cosf(angle/2.0f);
	local.x = axis.x * sinf(angle/2.0f);
	local.y = axis.y * sinf(angle/2.0f);
	local.z = axis.z * sinf(angle/2.0f);

	*q = quaternion_mul(local, *q);
}

mat4 quaternion_to_mat4(quaternion q) {
	float len = sqrtf(q.x*q.x + q.y*q.y + q.z*q.z + q.w*q.w);
	float x = q.x / len;
	float y = q.y / len;
	float z = q.z / len;
	float w = q.w / len;
	//quaternion qn = s
	
	/*mat4 result = {
		w*w + x*x - y*y - z*z, 2*x*y - 2*w*z, 2*x*z + 2*w*y, 0,
		2*x*y + 2*w*z, w*w - x*x + y*y - z*z, 2*y*z + 2*w*x, 0,
		2*x*z - 2*w*y, 2*y*z - 2*w*x, w*w - x*x - y*y + z*z, 0,
		0, 0, 0, 1,
	};*/

	mat4 result = {
		1 - 2*y*y - 2*z*z, 2*x*y - 2*w*z,     2*x*z + 2*w*y,     0,
		2*x*y + 2*w*z,     1 - 2*x*x - 2*z*z, 2*y*z + 2*w*x,     0,
		2*x*z - 2*w*y,     2*y*z - 2*w*x,     1 - 2*x*x - 2*y*y, 0,
		0, 0, 0, 1,
	};

	/*mat4 result = {
		1 - 2*(y*y+z*z), 2*x*y + 2*w*z,     2*x*z - 2*w*y,     0,
		2*x*y - 2*w*z,     2*(x*x+z*z) - 1, 2*y*z + 2*w*x,     0,
		2*x*z + 2*w*y,     2*y*z - 2*w*x,     2*(x*x+y*y) - 1, 0,
		0, 0, 0, 1,
	};*/

	/*float XX = x * x;
	float YY = y * y;
	float ZZ = z * z;
	float XY = x * y;
	float XZ = x * z;
	float YZ = y * z;
	float WX = w * x;
	float WY = w * y;
	float WZ = w * z;

	mat4 result = {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1,
	};
	result.e[0] = 1.0f - 2.0f * (YY + ZZ);
	result.e[1] = 2.0f * (XY + WZ);
	result.e[2] = 2.0f * (XZ - WY);

	result.e[4+0] = 2.0f * (XY - WZ);
	result.e[4+1] = 1.0f - 2.0f * (XX + ZZ);
	result.e[4+2] = 2.0f * (YZ + WX);

	result.e[8+0] = 2.0f * (XZ + WY);
	result.e[8+1] = 2.0f * (YZ - WX);
	result.e[8+2] = 1.0f - 2.0f * (XX + YY);*/

	return result;
}

// #define lo(l0x, l0y, l1x, l1y, px, py) ((l1x-l0x)*(py-l0y) - (l1y-l0y)*(px-l0x))
float lo(float l0x, float l0y, float l1x, float l1y, float px, float py) {
	return (l1x-l0x)*(py-l0y) - (l1y-l0y)*(px-l0x);
}

int main() {
	//printf("%i, %s\n", __argc, __argv);

	Rain rain = {0};
	rain.window_width = 1920;
	rain.window_height = 1080;
	rain.window_title = "Ship designer";
	rain_init(&rain);

	load_opengl_extensions();

	GLuint simple_shader = shader_from_string(
		"#version 330\n"
		"uniform mat4 projection_matrix;\n"
		"uniform mat4 camera_rotation;\n"
		"in vec3 in_vertex;\n"
		"in vec4 in_color;\n"
		"out vec4 ex_color;\n"
		"void main() {\n"
		"	gl_Position = projection_matrix * (camera_rotation * vec4(in_vertex, 1.0f));\n"
		"	ex_color = in_color;\n"
		"}\n",

		"#version 330\n"
		"in vec4 ex_color;\n"
		"out vec4 out_color;\n"
		"void main() {\n"
		"	out_color = ex_color;\n"
		"}\n",

		NULL
	);

	GLuint geo_shader = shader_from_string(
		"#version 330\n"
		"uniform mat4 projection_matrix;\n"
		"uniform mat4 camera_rotation;\n"
		"in vec3 in_vertex;\n"
		"in vec4 in_color;\n"
		"out vec4 ex_color;\n"
		"void main() {\n"
		"	gl_Position = projection_matrix * (camera_rotation * vec4(in_vertex, 1.0f));\n"
		"	ex_color = in_color;\n"
		"}\n",

		"#version 330\n"
		"uniform vec4 color;\n"
		"in vec4 ex_color;\n"
		"in vec3 ex_normal;\n"
		"out vec4 out_color;\n"
		"void main() {\n"
		"	out_color = vec4(ex_normal, 1.0f);\n"
		"}\n",

		"#version 330\n"
		"layout(triangles) in;\n"
		"layout(triangle_strip, max_vertices = 3) out;\n"
		"out vec3 ex_normal;\n"
		"void main() {\n"
		"	ex_normal = vec3(1.0f, 1.0f, 0.0f);\n"
		"	gl_Position = gl_in[0].gl_Position;\n"
		"	EmitVertex();\n"
		"	ex_normal = vec3(0.0f, 1.0f, 0.0f);\n"
		"	gl_Position = gl_in[1].gl_Position;\n"
		"	EmitVertex();\n"
		"	ex_normal = vec3(0.0f, 1.0f, 1.0f);\n"
		"	gl_Position = gl_in[2].gl_Position;\n"
		"	EmitVertex();\n"
		"	EndPrimitive();\n"
		"}\n"
	);

	GLuint screen_shader = shader_from_string(
		"#version 330\n"
		"in vec3 in_vertex;\n"
		"in vec4 in_color;\n"
		"out vec4 ex_color;\n"
		"void main() {\n"
		"	gl_Position = vec4(in_vertex, 1.0f);\n"
		"	ex_color = in_color;\n"
		"}\n",

		"#version 330\n"
		"in vec4 ex_color;\n"
		"out vec4 out_color;\n"
		"void main() {\n"
		"	out_color = ex_color;\n"
		"}\n",

		NULL
	);

	float proj_mat[16];
	make_perspective_matrix(proj_mat, 70, (float)rain.window_width/(float)rain.window_height, 0.1f, 100.0f);

	float4 f = {-5.0f, 5.0f, -10.0f, 1.0f};
	//debug_print("f %f, %f, %f, %f \n", f.x, f.y, f.z, f.w);
	float4_apply_mat4(&f, proj_mat);
	/*debug_print("mr0 %f, %f, %f, %f \n", proj_mat[0], proj_mat[1], proj_mat[2], proj_mat[3]);
	debug_print("mr1 %f, %f, %f, %f \n", proj_mat[4], proj_mat[5], proj_mat[6], proj_mat[7]);
	debug_print("mr2 %f, %f, %f, %f \n", proj_mat[8], proj_mat[9], proj_mat[10], proj_mat[11]);
	debug_print("mr3 %f, %f, %f, %f \n", proj_mat[12], proj_mat[13], proj_mat[14], proj_mat[15]);*/

	/*debug_print("f %f, %f, %f, %f \n", f.x, f.y, f.z, f.w);
	f.x /= f.w;
	f.y /= f.w;
	f.z /= f.w;
	debug_print("f %f, %f, %f, %f \n", f.x, f.y, f.z, f.w);*/

	/*hmm_mat4 m;
	memcpy(&m, proj_mat, sizeof(float)*16);
	hmm_vec4 v = {-5.0f, 5.0f, -10.0f, 1.0f};
	hmm_vec4 v2 = HMM_MultiplyMat4ByVec4(m, v);
	debug_print("f %f, %f, %f, %f \n", v2.X, v2.Y, v2.Z, v2.W);*/

	float3 geo_vertices[3*100];
	int geo_indices[3*100];
	int geo_vertex_count = 0;
	int geo_index_count = 0;

	float3 tri_add_buffer[3];
	int tri_add_count = 0;

#define add_geo_triangle(p0, p1, p2)\
	geo_vertices[geo_vertex_count] = p0;\
	geo_indices[geo_index_count++] = geo_vertex_count++;\
	geo_vertices[geo_vertex_count] = p1;\
	geo_indices[geo_index_count++] = geo_vertex_count++;\
	geo_vertices[geo_vertex_count] = p2;\
	geo_indices[geo_index_count++] = geo_vertex_count++;\

	typedef struct {
		int2 vertices[3];
	} Shape;
	Shape shapes[100];
	int shape_count = 0;

#define add_shape(v0, v1, v2)\
	if (shape_count < array_size(shapes)) {\
		shapes[shape_count].vertices[0] = v0; \
		shapes[shape_count].vertices[1] = v1; \
		shapes[shape_count].vertices[2] = v2; \
		int i = v0.y*(GRID_WIDTH+1) + v0.x; \
		grid[i].shapes[grid[i].shape_count++] = shape_count; \
		i = v1.y*(GRID_WIDTH+1) + v1.x; \
		grid[i].shapes[grid[i].shape_count++] = shape_count; \
		i = v2.y*(GRID_WIDTH+1) + v2.x; \
		grid[i].shapes[grid[i].shape_count++] = shape_count; \
		++shape_count; \
	} else {\
		debug_print("Ran out of shape space!\n");\
	}\

#define GRID_WIDTH 10
#define GRID_HEIGHT 10
#define GRID_CELL_SHAPE_MAX 8
	typedef struct {
		int shapes[GRID_CELL_SHAPE_MAX];
		int shape_count;
	} GridSquare;
	GridSquare grid[(GRID_WIDTH+1) * (GRID_HEIGHT+1)] = {0};

	quaternion camera_rotation = quaternion_identity();
	float3 camera_euler_rotation = {-0.5f};
	mat4 camera_matrix;

	int2 drag_start = {0};

	while (!rain.quit) {
		rain_update(&rain);

		float2 mouse = {(float)rain.mouse.position.x / (rain.window_width) * 2.0f - 1.0f,
						-((float)rain.mouse.position.y / (rain.window_height) * 2.0f - 1.0f)};
		//debug_print("mouse %f, %f\n", mouse.x, mouse.y);

		/*if (rain.mouse.position_delta.x || rain.mouse.position_delta.y || rain.mouse.wheel_delta) {
			debug_print("mouse  x%i y%i wheel%i\n", rain.mouse.position_delta.x, rain.mouse.position_delta.y, rain.mouse.wheel_delta);
		}*/

		if (rain.mouse.middle.down) {
			if (rain.mouse.position_delta.x) {
				//quaternion_apply(&camera_rotation, make_float3(0.0f, 1.0f, 0.0f), 0.001f * rain.mouse.position_delta.x);
				camera_euler_rotation.y -= rain.mouse.position_delta.x * 0.002f;
			}
			if (rain.mouse.position_delta.y) {
				//quaternion_apply(&camera_rotation, make_float3(-1.0f, 0.0f, 0.0f), 0.001f * rain.mouse.position_delta.y);
				camera_euler_rotation.x -= rain.mouse.position_delta.y * 0.002f;
			}
		}

		camera_matrix = mat4_identity();
		mat4_apply_rotate_y(&camera_matrix, camera_euler_rotation.y);
		mat4_apply_rotate_x(&camera_matrix, camera_euler_rotation.x);
		mat4_apply_translation(&camera_matrix, make_float3(0, 0, -10.0f));

		glViewport(0, 0, rain.window_width, rain.window_height);
		//glClearColor(0.0f, 0.2f, 0.5f, 0.0f);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_ALPHA_TEST);
		glBlendFunc(GL_ONE_MINUS_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glMatrixMode(GL_PROJECTION);
		//glLoadMatrixf(proj_mat);
		glMatrixMode(GL_MODELVIEW);


#if 1
		float4 closest_point;
		float4 closest_p;
		int2 closest_grid;
		for (int y = 0; y < GRID_HEIGHT+1/*array_size(grid)*/; ++y) {
			for (int x = 0; x <GRID_WIDTH+1; ++x) {
				float4 pos = {(-GRID_WIDTH/2) + x, 0, (-GRID_HEIGHT/2) + y, 1.0f};
				float4 p = pos;
				float4_apply_mat4(&pos, &camera_matrix);
				float4_apply_perspective(&pos, proj_mat);
				if (y==0&&x==0) {
					closest_point = pos;
					closest_p = p;
				} else {
					float d0 = float2_dist(pos.xy, mouse);
					float d1 = float2_dist(closest_point.xy, mouse);
					if (d0 < d1) {
						closest_point = pos;
						closest_p = p;
						closest_grid.x = x;
						closest_grid.y = y;
					}
				}
			}
		}

		if (rain.mouse.left.pressed) {
			drag_start = closest_grid;
		}

		if (rain.mouse.left.released && (closest_grid.x!=drag_start.x || closest_grid.y!=drag_start.y)) {
			debug_print("drag was done! {%i %i} to {%i %i}\n", drag_start.x, drag_start.y, closest_grid.x, closest_grid.y);

			int i = drag_start.y*(GRID_WIDTH+1) + drag_start.x;
			for (int j = 0; j < grid[i].shape_count; ++j) {
				//shapes[grid[i]->shapes[j]]
				for (int k = 0; k < 3; ++k) {
					if (shapes[grid[i].shapes[j]].vertices[k].x == drag_start.x &&
						shapes[grid[i].shapes[j]].vertices[k].y == drag_start.y) {
						if (grid[closest_grid.y*(GRID_WIDTH+1) + closest_grid.x].shape_count < GRID_CELL_SHAPE_MAX) {
							shapes[grid[i].shapes[j]].vertices[k].x = closest_grid.x;
							shapes[grid[i].shapes[j]].vertices[k].y = closest_grid.y;
							grid[closest_grid.y*(GRID_WIDTH+1) + closest_grid.x].shapes[grid[closest_grid.y*(GRID_WIDTH+1) + closest_grid.x].shape_count++] = grid[i].shapes[j];
							grid[i].shapes[j] = grid[i].shapes[grid[i].shape_count-1];
							--grid[i].shape_count;
							--j; // redo this shape index
							break;
						} else {
							debug_print("max shapes reached for grid cell %i %i\n", closest_grid.x, closest_grid.y);
						}
					}
				}
			}
		}

		// todo: this would be simpler to loop though shapes and compare vertices since they are integers
		// check for triangle vertices in the same cell and remove
#if 0
		for (int i = 0; i < array_size(grid); ++i) {
			//int shape = grid[i].shapes[0];
			bool shape_indices[array_size(shapes)] = {0}; // hash table, kinda
			for (int j = 0; j < grid[i].shape_count; ++j) {
				int shape_index = grid[i].shapes[j];
				if (shape_indices[shape_index]) {
					debug_print("removing shape...\n");
					for (int k = 0; k < 3; ++k) {
						int2 v = shapes[shape_index].vertices[k];
						int gi = v.y*(GRID_WIDTH+1) + v.x;
						for (int l = 0; l < grid[gi].shape_count; ++l) {
							if (grid[gi].shapes[l] == shape_index) {
								if (grid[gi].shape_count > 1) {
									grid[gi].shapes[l] = grid[gi].shapes[grid[gi].shape_count-1];
									--grid[gi].shape_count;
								} else {
									--grid[gi].shape_count;
								}
								break;
							}
						}
						// grid[v.y*(GRID_WIDTH+1) + v.x]
					}
					if (shape_count > 1) {
						shapes[shape_index] = shapes[shape_count -1];
						--shape_count;
					} else {
						--shape_count;
					}
					shape_indices[shape_index] = false;
				} else {
					shape_indices[shape_index] = true;
				}
			}
		}
#endif
		for (int i = 0; i < shape_count; ++i) {
			for (int j = 0; j < 3; ++j) {
				int2 v = shapes[i].vertices[j];
				int gi = v.y*(GRID_WIDTH+1) + v.x;
				int found = 0;
				for (int k = 0; k < grid[gi].shape_count; ++k) {
					if (grid[gi].shapes[k]==i) ++found;
				}
				if (found > 1) {
					for (int jj = 0; jj < 3; ++jj) {
						int2 v = shapes[i].vertices[jj];
						int gi = v.y*(GRID_WIDTH+1) + v.x;
						for (int k = 0; k < grid[gi].shape_count; ++k) {
							if (grid[gi].shapes[k]==i) {
								grid[gi].shapes[k] = grid[gi].shapes[grid[gi].shape_count-1];
								--grid[gi].shape_count;
							}
						}
					}
					/*shapes[i] = shapes[shape_count-1];
					--shape_count;*/
					shapes[i].vertices[0].x = 0;
					shapes[i].vertices[0].y = 0;
					shapes[i].vertices[1].x = 0;
					shapes[i].vertices[1].y = 0;
					shapes[i].vertices[2].x = 0;
					shapes[i].vertices[2].y = 0;
					break;
				}
			}
		}

		// check for errors
		for (int i = 0; i < shape_count; ++i) {
			for (int j = 0; j < 3; ++j) {
				int2 v = shapes[i].vertices[j];
				bool found = false;
				for (int k = 0; k < grid[v.y*(GRID_WIDTH+1) + v.x].shape_count; ++k) {
					if (grid[v.y*(GRID_WIDTH+1) + v.x].shapes[k] == i) {
						found = true;
						break;
					}
				}
				if (!found) debug_print("error, shape not found in grid\n");
			}
		}

		/*if (rain.mouse.right.released) {
			tri_add_buffer[tri_add_count++] = closest_p.xyz;
			if (tri_add_count == 3) {
				geo_vertices[geo_vertex_count] = tri_add_buffer[0];
				geo_indices[geo_index_count++] = geo_vertex_count++;
				geo_vertices[geo_vertex_count] = tri_add_buffer[1];
				geo_indices[geo_index_count++] = geo_vertex_count++;
				geo_vertices[geo_vertex_count] = tri_add_buffer[2];
				geo_indices[geo_index_count++] = geo_vertex_count++;
				tri_add_count = 0;
			}
		}*/

		use_shader(0);
		glPointSize(4);
		glBegin(GL_POINTS);
		glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
		glVertex3f(closest_point.x, closest_point.y, closest_point.z);
		glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
		glVertex3f(-5.0f + drag_start.x, 0, -5.0f + drag_start.y);
		//debug_print("%i %i\n", drag_start.x, drag_start.y);
		glEnd();

		use_shader(simple_shader);
		/*glBegin(GL_POINTS);
		glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
		glVertex3f(-5.0f + drag_start.x, 0, -5.0f + drag_start.y);
		glEnd();*/
		float3 pos = {-5.0f + drag_start.x, 0, -5.0f + drag_start.y};
		float4 color = {0.0f, 0.0f, 1.0f, 1.0f};
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, &pos);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, &color);
		glDrawArrays(GL_POINTS, 0, 1);

		/*for (int i = 0; i < tri_add_count; ++i) {
			glColor4f(0.0f, 1.0f, 1.0f, 1.0f);
			glBegin(GL_POINTS);
			float4 pos = make_float4_from_float3(tri_add_buffer[i], 1.0f);
			float4_apply_mat4(&pos, &camera_matrix);
			float4_apply_perspective(&pos, proj_mat);
			glVertex3f(pos.x, pos.y, pos.z);
			glEnd();
		}*/
#endif

		for (int y = 0; y < GRID_HEIGHT; ++y) {
			for (int x = 0; x < GRID_WIDTH; ++x) {
				float4 p = {(-GRID_WIDTH/2) + x,		0, (-GRID_HEIGHT/2) + y, 1.0f};
				for (int j = 0; j < 4; ++j) {
					float4 pos =	{(-GRID_WIDTH/2) + x + (j%2)*0.5f,		  0, (-GRID_HEIGHT/2) + y + (j/2)*0.5f, 1.0f};
					float4 posp00 = {(-GRID_WIDTH/2) + x + (j%2)*0.5f,		  0, (-GRID_HEIGHT/2) + y + (j/2)*0.5f, 1.0f};
					float4 posp10 = {(-GRID_WIDTH/2) + x + (j%2)*0.5f + 0.5f, 0, (-GRID_HEIGHT/2) + y + (j/2)*0.5f, 1.0f};
					float4 posp11 = {(-GRID_WIDTH/2) + x + (j%2)*0.5f + 0.5f, 0, (-GRID_HEIGHT/2) + y + (j/2)*0.5f + 0.5f, 1.0f};
					float4 posp01 = {(-GRID_WIDTH/2) + x + (j%2)*0.5f,		  0, (-GRID_HEIGHT/2) + y + (j/2)*0.5f + 0.5f, 1.0f};
					/*mat4 cam = quaternion_to_mat4(camera_rotation);*/

					float4_apply_mat4(&posp00, &camera_matrix);
					float4_apply_mat4(&posp10, &camera_matrix);
					float4_apply_mat4(&posp11, &camera_matrix);
					float4_apply_mat4(&posp01, &camera_matrix);
					float4_apply_perspective(&posp00, proj_mat);
					float4_apply_perspective(&posp10, proj_mat);
					float4_apply_perspective(&posp11, proj_mat);
					float4_apply_perspective(&posp01, proj_mat);

					float lo0 = lo(posp00.x, posp00.y, posp10.x, posp10.y, mouse.x, mouse.y);
					float lo1 = lo(posp10.x, posp10.y, posp11.x, posp11.y, mouse.x, mouse.y);
					float lo2 = lo(posp11.x, posp11.y, posp00.x, posp00.y, mouse.x, mouse.y);

					float lo3 = lo(posp00.x, posp00.y, posp11.x, posp11.y, mouse.x, mouse.y);
					float lo4 = lo(posp11.x, posp11.y, posp01.x, posp01.y, mouse.x, mouse.y);
					float lo5 = lo(posp01.x, posp01.y, posp00.x, posp00.y, mouse.x, mouse.y);

					if (lo0 < 0 && lo1 < 0 && lo4 < 0 && lo5 < 0) {
						glColor4f(1.0f, 0.0f, 1.0f, 0.5f);
						glBegin(GL_TRIANGLES);
						/*glVertex3f(pos.x, pos.y, pos.z);
						glVertex3f(pos.x+0.5f, pos.y, pos.z);
						glVertex3f(pos.x+0.5f, pos.y, pos.z+0.5f);
						glVertex3f(pos.x, pos.y, pos.z+0.5f);*/
						int2 i0;
						int2 i1;
						int2 i2;
						switch (j) {
						case 0:
							/*p0 = make_float3(p.x, p.y, p.z);
							p1 = make_float3(p.x+1, p.y, p.z);
							p2 = make_float3(p.x, p.y, p.z+1);*/
							/*glVertex3f(p.x, p.y, p.z);
							glVertex3f(p.x+1, p.y, p.z);
							glVertex3f(p.x, p.y, p.z+1);*/
							i0 = make_int2(x, y);
							i1 = make_int2(x+1, y);
							i2 = make_int2(x, y+1);
							break;
						case 1:
							/*p0 = make_float3(p.x, p.y, p.z);
							p1 = make_float3(p.x+1, p.y, p.z);
							p2 = make_float3(p.x+1, p.y, p.z+1);*/
							/*glVertex3f(p.x, p.y, p.z);
							glVertex3f(p.x+1, p.y, p.z);
							glVertex3f(p.x+1, p.y, p.z+1);*/
							i0 = make_int2(x, y);
							i1 = make_int2(x+1, y);
							i2 = make_int2(x+1, y+1);
							break;
						case 2:
							/*p0 = make_float3(p.x, p.y, p.z);
							p1 = make_float3(p.x+1, p.y, p.z+1);
							p2 = make_float3(p.x, p.y, p.z+1);*/
							/*glVertex3f(p.x, p.y, p.z);
							glVertex3f(p.x+1, p.y, p.z+1);
							glVertex3f(p.x, p.y, p.z+1);*/
							i0 = make_int2(x, y);
							i1 = make_int2(x+1, y+1);
							i2 = make_int2(x, y+1);
							break;
						case 3:
							/*p0 = make_float3(p.x+1, p.y, p.z);
							p1 = make_float3(p.x+1, p.y, p.z+1);
							p2 = make_float3(p.x, p.y, p.z+1);*/
							/*glVertex3f(p.x+1, p.y, p.z);
							glVertex3f(p.x+1, p.y, p.z+1);
							glVertex3f(p.x, p.y, p.z+1);*/
							i0 = make_int2(x+1, y);
							i1 = make_int2(x+1, y+1);
							i2 = make_int2(x, y+1);
							break;
						}

						if (rain.mouse.right.down) {
							glVertex3f(-5.0f + i0.x, 0, -5.0f + i0.y);
							glVertex3f(-5.0f + i1.x, 0, -5.0f + i1.y);
							glVertex3f(-5.0f + i2.x, 0, -5.0f + i2.y);
						}
						glEnd();

						if (rain.mouse.right.released) {
							/*int2 i0 = {x, y};
							int2 i1 = {x+1, y};
							int2 i2 = {x+1, y+1};*/
							//add_geo_triangle(p0, p1, p2);
							add_shape(i0, i1, i2);
						}
					}
				}
			}

#if 0
			use_shader(screen_shader);
			float3 v[] = {posp00.x, posp00.y, 0.0f, // todo: why is z wrong?
						  posp10.x, posp10.y, 0.0f,
						  posp11.x, posp11.y, 0.0f,};
			float4 c[] = {0.0f, 1.0f, 0.0f, 1.0f,
						  0.0f, 1.0f, 0.0f, 1.0f,
						  0.0f, 1.0f, 0.0f, 1.0f,};
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, &v);
			glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, &c);
			glPointSize(4.0f);
			glDrawArrays(GL_TRIANGLES, 0, 3);
#endif

			use_shader(simple_shader);
#if 0
			if (lo0 < 0 && lo1 < 0 && lo4 < 0 && lo5 < 0) {
				glColor4f(1.0f, 0.0f, 1.0f, 0.5f);
				glBegin(GL_QUADS);
				glVertex3f(pos.x, pos.y, pos.z);
				glVertex3f(pos.x+1, pos.y, pos.z);
				glVertex3f(pos.x+1, pos.y, pos.z+1);
				glVertex3f(pos.x, pos.y, pos.z+1);
				glEnd();

				//if (float2_dist(posp00.xy, mouse) < )
				/*float4 p = posp00;
				float d = float2_dist(posp00.xy, mouse);
				float dd;
				if ((dd = float2_dist(posp10.xy, mouse)) < d) {
					p = posp10;
					d = dd;
				}
				if ((dd = float2_dist(posp11.xy, mouse)) < d) {
					p = posp11;
					d = dd;
				}
				if ((dd = float2_dist(posp01.xy, mouse)) < d) {
					p = posp01;
					d = dd;
				}*/
			}
#endif
#if 0
			if (lo0 < 0 && lo1 < 0 && lo2 < 0) {
				glColor4f(1.0f, 0.0f, 1.0f, 0.5f);
				glBegin(GL_TRIANGLES);
				glVertex3f(pos.x, pos.y, pos.z);
				glVertex3f(pos.x+1, pos.y, pos.z);
				glVertex3f(pos.x+1, pos.y, pos.z+1);
				glEnd();

				if (rain.mouse.left.released) {
					float3 p0 = pos.xyz;
					float3 p1 = {pos.x+1, pos.y, pos.z};
					float3 p2 = {pos.x+1, pos.y, pos.z+1};
					add_geo_triangle(p0, p1, p2);
				}
			}
			if (lo3 < 0 && lo4 < 0 && lo5 < 0) {
				glColor4f(1.0f, 0.0f, 1.0f, 0.5f);
				glBegin(GL_TRIANGLES);
				glVertex3f(pos.x, pos.y, pos.z);
				glVertex3f(pos.x+1, pos.y, pos.z+1);
				glVertex3f(pos.x, pos.y, pos.z+1);
				glEnd();

				if (rain.mouse.left.released) {
					float3 p0 = pos.xyz;
					float3 p1 = {pos.x+1, pos.y, pos.z+1};
					float3 p2 = {pos.x, pos.y, pos.z+1};
					add_geo_triangle(p0, p1, p2);
				}
			}
#endif
		}

		use_shader(simple_shader);
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		glBegin(GL_LINES);
		for (int y = 0; y < GRID_HEIGHT+1; ++y) {
			glVertex3f(-GRID_WIDTH/2, -0.1f, -GRID_HEIGHT/2 + y);
			glVertex3f(GRID_WIDTH/2, -0.1f, -GRID_HEIGHT/2 + y);
		}
		for (int x = 0; x < GRID_WIDTH+1; ++x) {
			glVertex3f(-GRID_WIDTH/2 + x, -0.1f, -GRID_HEIGHT/2);
			glVertex3f(-GRID_WIDTH/2 + x, -0.1f, GRID_HEIGHT/2);
		}
		glEnd();

#if 0
		glColor4f(0.5f, 0.0f, 0.5f, 1.0f);
		glBegin(GL_QUADS);
		glVertex3f(-0.5f, 0.5f, -6.0f);
		glVertex3f(0.5f, 0.5f, -6.0f);
		glVertex3f(0.5f, -0.5f, -6.0f);
		glVertex3f(-0.5f, -0.5f, -6.0f);
		glEnd();
#else
		typedef struct {
			struct {
				float x;
				float y;
				float z;
			} pos;
			struct {
				float r;
				float g;
				float b;
				float a;
			} color;
		} Vertex;
		Vertex vertices[] = {
			{{-0.5f, 0.5f, -9.0f},{0.0f, 1.0f, 1.0f, 1.0f}},
			{{0.5f, 0.5f, -9.0f},{0.0f, 1.0f, 1.0f, 1.0f}},
			{{0.5f, -0.5f, -9.0f},{0.0f, 1.0f, 1.0f, 1.0f}},
			{{-0.5f, -0.5f, -9.0f}, {0.0f, 1.0f, 1.0f, 1.0f}},
		};
		int indices[] = {
			0, 1, 2,
			0, 2, 3,
		};

		/*float4 a = {-0.5f, 0.5f, -9.0f, 1.0f};
		float4_apply_perspective(&a, proj_mat);
		int x = 0;*/

		use_shader(simple_shader);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), vertices);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), &vertices[0].color);
		/*mat4 camera_rotation_matrix = quaternion_to_mat4(camera_rotation);*/
		
		int projection_uniform = glGetUniformLocation(simple_shader, "projection_matrix");
		int camera_uniform = glGetUniformLocation(simple_shader, "camera_rotation");
		glUniformMatrix4fv(projection_uniform, 1, GL_FALSE, proj_mat);
		glUniformMatrix4fv(camera_uniform, 1, GL_FALSE, &camera_matrix);
		// glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, indices);


		use_shader(geo_shader);
		projection_uniform = glGetUniformLocation(geo_shader, "projection_matrix");
		camera_uniform = glGetUniformLocation(geo_shader, "camera_rotation");
		glUniformMatrix4fv(projection_uniform, 1, GL_FALSE, proj_mat);
		glUniformMatrix4fv(camera_uniform, 1, GL_FALSE, &camera_matrix);

		{
			geo_vertex_count = 0;
			geo_index_count = 0;
			for (int i = 0; i < shape_count; ++i) {
				float3 p0 = {
					-5.0f + shapes[i].vertices[0].x,
					0.0f,
					-5.0f + shapes[i].vertices[0].y
				};
				float3 p1 = {
					-5.0f + shapes[i].vertices[1].x,
					0.0f,
					-5.0f + shapes[i].vertices[1].y
				};
				float3 p2 = {
					-5.0f + shapes[i].vertices[2].x,
					0.0f,
					-5.0f + shapes[i].vertices[2].y
				};
				add_geo_triangle(p0, p1, p2);
			}

			float3 lines[array_size(geo_vertices)*2];
			int line_count = 0;
			for (int i = 0; i < geo_vertex_count; i += 3) {
				lines[line_count] = geo_vertices[i];
				lines[line_count++].y = 0.01f;
				lines[line_count] = geo_vertices[i+1];
				lines[line_count++].y = 0.01f;
				lines[line_count] = geo_vertices[i+1];
				lines[line_count++].y = 0.01f;
				lines[line_count] = geo_vertices[i+2];
				lines[line_count++].y = 0.01f;
				lines[line_count] = geo_vertices[i+2];
				lines[line_count++].y = 0.01f;
				lines[line_count] = geo_vertices[i];
				lines[line_count++].y = 0.01f;
			}

			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float3), geo_vertices);
			glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), &vertices[0].color);
			glUniform4f(glGetUniformLocation(geo_shader, "color"), 0.0f, 0.0f, 1.0f, 1.0f);
			glDrawElements(GL_TRIANGLES, (geo_index_count/3)*3, GL_UNSIGNED_INT, geo_indices);
			
			//glDrawElements(GL_LINE, (geo_index_count/3)*3, GL_UNSIGNED_INT, geo_indices);
			glUniform4f(glGetUniformLocation(geo_shader, "color"), 0.0f, 1.0f, 0.0f, 1.0f);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, lines);
			glDrawArrays(GL_LINES, 0, line_count);
			//debug_print("geo_index_count %i\n", (geo_index_count/3)*3);
		}
#endif
	}
}