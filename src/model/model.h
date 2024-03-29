#pragma once

#include "../glut.h"
#include "../bmp.h"

#include <cassert>
#include <vector>
#include <iostream>
using namespace std;

namespace Sokoban
{
	class Object;
	class Map;
	extern Map map;
	extern vector<float> eye;
	extern vector<float> direction;
	extern vector<float> up;
	extern vector<int> window_size;
	extern int windowHandle;
	extern float zoomAngle;
	extern float eye_h;

	// textures
	extern const int texture_count;
	extern vector<GLuint> textures;
	extern vector<bmpImage*> texture_images;

	// display list
	extern const int display_count;
	extern vector<GLuint> display_list;

	// view
	extern bool light_mode;
	extern int sleep_cnt;

	// init each
	void init_map();
	void init_texture();
	void init_display_list();

	// init all paras
	void init_paras();
	void init_ctrl_paras();
	void init_model_paras();
	void init_view_paras();

	// screen shot
	void screen_shot();
	
	// object enums
	enum ObjectID {
		EMPTY_ID, CUBE_ID, WALL_ID,
		DST_ID, COMPLETE_ID
	};
	enum TextureID {
		BOX, Floor, Wall,
		DST, Complete, Sky, Border
	};
	enum DisplayID {
		BOX_Disp, Floor_Disp, Wall_Disp,
		DST_Disp, Complete_Disp
	};

	class Object
	{
	private:
		ObjectID id;
	public:
		Object(ObjectID id): id(id) {}
		ObjectID getID() const { return id; }

		// draw function for actual paint
		virtual void draw() = 0;

		virtual bool is_movable() = 0;
		virtual bool is_moving() = 0;
		virtual bool can_enter() = 0;
		// move
		virtual void move_to(int x, int y, int z = 0) = 0;
		virtual void move_to(vector<int> end) = 0;
		virtual const vector<int>& get_position() const = 0;
		virtual void set_position(const vector<int>& p) = 0;
	};

	class EmptyCube : public Object
	{
	private:
		vector<int> position;

	public:
		EmptyCube(const vector<int>& p);
		virtual ~EmptyCube() = default;

		// getters
		virtual bool is_movable() { return false; }
		virtual bool is_moving() { return false; }
		virtual const vector<int>& get_position() const { return position; }
		virtual bool can_enter() { return true; }
		virtual void set_position(const vector<int>& p) { position = p; }
		// move
		virtual void move_to(int x, int y, int z = 0) {}
		virtual void move_to(vector<int> end) {}
		// draw
		static void register_disp();
		virtual void draw();
	};

	class SolidCube : public Object
	{
	protected:
		vector<int> position;
		vector<float> move;
		bool moving;

		void move_once();

	public:
		SolidCube(const vector<int>& p, ObjectID id = CUBE_ID);
		virtual ~SolidCube() = default;

		// getters
		virtual bool is_movable() { return true; }
		virtual bool is_moving() { return moving; }
		virtual const vector<int>& get_position() const { return position; }
		virtual bool can_enter() { return false; }
		virtual void set_position(const vector<int>& p) { position = p; }
		// move
		virtual void move_to(int x, int y, int z = 0);
		virtual void move_to(vector<int> end);
		// draw
		virtual void draw_directly();
		virtual void draw();
	};

	class WallCube : public Object
	{
	private:
		vector<int> position;

	public:
		WallCube(const vector<int>& p);
		virtual ~WallCube() = default;

		// getters
		virtual bool is_movable() { return false; }
		virtual bool is_moving() { return false; }
		virtual const vector<int>& get_position() const { return position; }
		virtual bool can_enter() { return false; }
		virtual void set_position(const vector<int>& p) { position = p; }
		// move
		virtual void move_to(int x, int y, int z = 0) {}
		virtual void move_to(vector<int> end) {}
		// draw
		static void register_disp();
		virtual void draw();
	};

	class DstCube : public Object
	{
	private:
		vector<int> position;

	public:
		DstCube(const vector<int>& p);
		virtual ~DstCube() = default;

		// getters
		virtual bool is_movable() { return false; }
		virtual bool is_moving() { return false; }
		virtual const vector<int>& get_position() const { return position; }
		virtual bool can_enter() { return true; }
		virtual void set_position(const vector<int>& p) { position = p; }
		// move
		virtual void move_to(int x, int y, int z = 0) {}
		virtual void move_to(vector<int> end) {}
		// draw
		static void register_disp();
		virtual void draw();
	};

	class CompleteCube : public SolidCube
	{
	public:
		CompleteCube(const vector<int>& p);
		virtual ~CompleteCube() = default;
		// draw
		virtual void draw_directly();
		virtual void draw();
	};

	class Map
	{
	public:
		static constexpr const char* FilePrefix = "res/map";
		static const int total_map_number = 5;
	private:
		Object**** map_data;
		vector<int> size;
		vector<float> o;
		vector<float> x;
		vector<float> y;
		vector<float> z;
		vector<float> x_r;
		vector<float> y_r;
		vector<float> z_r;
		float cube_len;

		// game parameters
		int dstNum;
		int completeNum;
		int map_No;
		int step_number;

	public:
		Map();
		~Map();

		// setters and getters
		const vector<float> real_position(const vector<float>& position) const;
		const vector<float> map_position(const vector<float>& position) const;
		void set_object(Object* object, int x, int y, int z = 0);
		Object* get_object(int x, int y, int z = 0) const;
		float get_cube_len() const { return cube_len; }
		const vector<int>& get_size() const { return size; }

		// load map
		void load(int map_No);
		void reload();

		// draw
		static void register_disp_floor();
		void drawFloor() const;
		void drawBorder() const;
		void draw() const;

		// memory
		void map_data_alloc();
		void map_data_free();

		// game
		void add_complete() { completeNum++; cout << completeNum << endl; }
		void sub_complete() { completeNum--; cout << completeNum << endl; }
		void add_step() { step_number++; }
		bool win() { return completeNum == dstNum; }
		void to_prev_map() { map_No = map_No > 0 ? map_No - 1 : map_No; reload(); }
		void to_next_map() { map_No = map_No < total_map_number-1 ? map_No + 1 : map_No; reload(); }
		int get_step_number() { return step_number; }
		int get_map_number() { return map_No; }
	};

	// vector operation
	template <class T>
	const vector<T> operator+(const vector<T>& v1, const vector<T>& v2)
	{
		assert(v1.size() == v2.size());
		vector<T> ret;
		int size = v1.size();
		for (int i = 0; i < size; i++)
			ret.push_back(v1[i] + v2[i]);
		return ret;
	}

	template <class T>
	const vector<T> operator-(const vector<T>& v1, const vector<T>& v2)
	{
		assert(v1.size() == v2.size());
		vector<T> ret;
		int size = v1.size();
		for (int i = 0; i < size; i++)
			ret.push_back(v1[i] - v2[i]);
		return ret;
	}

	template <class T>
	ostream& operator<<(ostream& out, const vector<T>& v)
	{
		if (v.empty()) {
			out << "[ ]";
			return out;
		}
		out << "[ " << v[0];
		for (int i = 1; i < (int)v.size(); i++) {
			out << ", " << v[i];
		}
		out << " ]";
		return out;
	}

	template <class T>
	const vector<T> operator*(const vector<T> &v, const float f)
	{
		vector<T> ret;
		int size = v.size();
		for (int i = 0; i < size; i++)
			ret.push_back(v[i] * f);
		return ret;
	}

	template <class T>
	const vector<T> operator-(const vector<T>& v)
	{
		vector<T> ret;
		int size = v.size();
		for (int i = 0; i < size; i++)
			ret.push_back(-v[i]);
		return ret;
	}
}