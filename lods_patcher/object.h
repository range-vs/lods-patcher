#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <iterator>
#include <algorithm>
#include <windows.h>
#include <iomanip>
#include <vector>
#include <time.h>
#include <numeric>
#include <list>
#include <filesystem>
#include <map>
#include <regex>
#include <set>
#include <array>
#include <sstream>

#include "function.h"

namespace fs = std::experimental::filesystem;

class Locale // ������
{
	UINT cp;
	UINT output_cp;
public:
	Locale(const int& l);
	~Locale();
};

struct vertex // �������
{
	float x = 0;
	float y = 0;
	float z = 0;

	vertex() {}
	vertex(float a, float b, float c) :x(a), y(b), z(c) {}

	vertex operator+(const vertex& v);
	bool operator==(const vertex& v);
	bool operator!=(const vertex& v);
	bool operator<(const vertex& v)const;
	bool equals_epsilon(const vertex & v);
	bool equals_epsilon(const vertex & v)const;
	friend std::ostream& operator<<(std::ostream& ostream, const vertex& v); // �����
};

struct material // ������ � ���������
{
	std::string name; // ��� ���������
	std::string shader; // ������ ��� ������
	std::string compile; // ������ ��� �����������
	std::string gamemtl; // ��� ����
	std::string texture; // ���� � ��������
	std::string unknown; // ����������� � ����� ���������� ����������(0x1008)
	uint double_sides; // ����������� ��������(0x10 - ��, 0x00 - ���)
	char unknown_data[8]; // ������ �����������, ����������� ������
};

struct author // ������ � ��������
{
	std::string creator; // ���������
	tm date_creator; // ���� ��������
	std::string modifycator; // ��������� ��������������
	tm date_modify; // ���� ��������� �����������
};

struct minobject // ������ � ���������
{
	char unknown1[2]; // ����������� ������
	std::string name; // ��������
	vertex bbox_min; // ������� ��������������� ������
	vertex bbox_max; // �������� ��������������� ������
	char unknown2; // ����������� ������
	char unknown3[8]; // ����������� ������
	std::vector<vertex> vert; // ���� ������
	std::vector<std::pair<std::string, std::vector<int>>> materials; // ���������(���� - ��� ���������, �������� - ������ �������� ������
	std::vector<std::array<int, 3>> faces; // ������ ��������, < ������� ������, ������������ ���� ������� >
	// �� ��� �����, ��������� ������� �� �������������
};

struct file_object // ������
{
	char version[2]; // ������������ � hex �������, ���������������� ������(???)
	std::string user_data; // ���������������� ����������
	std::string path_to_lod; // ���� �� ����
	uint object_type; // ��� �������
	std::vector<minobject> objs; // ������ � ��������� �����������
	std::vector<material> materials; // ������ ����������
	author auth; // �����
};

struct final_object // ��������� ������, ������� ����� ������������
{
private:
	std::array<std::string, 5> textures_stop;
public:
	std::set<vertex> vertexs; // ������ ������
	vertex v; // �������, ������� ������ ���������� ���� ������������� ������
	std::set<std::string> textures; // ��� �������� �������(����: ��������, ��������)

	final_object();
	bool init(const char* name_file, bool b = false);
	bool init(const std::string& name_file, bool b = false) { return this->init(name_file.c_str(), b); }
	void calculate_general_vertex(); // ����������� ����� �������
	bool operator==(const final_object& fo); // ����� ���������
	void add_texture(const std::string& texture); // ��������� �������� � �������

};

struct section // ������ � ����� scene_objects.part
{
	std::string section_number;
	std::string clsid;
	std::string co_flags;
	std::string flags;
	std::string name;
	std::string position;
	std::string reference_name;
	std::string rotation;
	std::string scale;
	std::string version;
};

class scene_object_out // �������/�������� ������
{
private:
	std::string name_file; // ��� �����
	std::list<section> sect; // ������ ������
	std::vector<std::string> temp_data; // ��������� ������
public:
	scene_object_out(const std::string& n) : name_file(n) {}
	bool decompile(); // ������ ����
	std::list<section>& get_sections() { return this->sect; } // ������ ��� ������

	bool write_new_file(const std::string& out_path); // ������������� ����� ����, TODO, ���������� �����
};

class edit_objects // ����� �����, ������� ����� �������� ��������
{
	std::string location; // ������� �������(arg cmd)
	scene_object_out scene_object; // ���� � ��������
	std::string path_sdk; // ���� �� ���
	std::string path_out; // �������� ����, ���� ���������� �������, ��� ������
	std::map<std::string, final_object> lods_in; // ������� ����, ������� ����� ����������
	std::map<std::string, final_object> lods; // ����, �� ������� ����� ������������ ������
	bool debug; // ����� �������(����������� ��������� ����� � �����)

public:
	edit_objects(const std::string& n):scene_object(n), debug(false){}
	bool init(char** argv, int argc); // �������������� ������
	bool load_data(); // ��������� ��� ������� � ������
	void copy_files_debug(); // �������� ��������� ���� � ���� �����
	void replace_references_name(); // ������ ������ �� ���� � �������� �����
	bool write_new_file(const std::string& out_path) { return scene_object.write_new_file(out_path); } // ���������� ����� ���� � ��������
};