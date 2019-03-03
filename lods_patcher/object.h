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

class Locale // локаль
{
	UINT cp;
	UINT output_cp;
public:
	Locale(const int& l);
	~Locale();
};

struct vertex // вершина
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
	friend std::ostream& operator<<(std::ostream& ostream, const vertex& v); // вывод
};

struct material // данные о материале
{
	std::string name; // имя материала
	std::string shader; // шейдер для движка
	std::string compile; // шейдер для компилятора
	std::string gamemtl; // для игры
	std::string texture; // путь к текстуре
	std::string unknown; // используетч в блоке текстурыне координаты(0x1008)
	uint double_sides; // двустронний материал(0x10 - да, 0x00 - нет)
	char unknown_data[8]; // всегда повторяется, неизвестные данные
};

struct author // данные о создании
{
	std::string creator; // создатель
	tm date_creator; // дата создания
	std::string modifycator; // последний модифицирующий
	tm date_modify; // дата последней модификации
};

struct minobject // данные о геометрии
{
	char unknown1[2]; // неизвестные данные
	std::string name; // название
	vertex bbox_min; // минимум ограничивающего объема
	vertex bbox_max; // максимум ограничивающего объема
	char unknown2; // неизвестные данные
	char unknown3[8]; // неизвестные данные
	std::vector<vertex> vert; // блок вершин
	std::vector<std::pair<std::string, std::vector<int>>> materials; // материалы(ключ - имя материала, значение - массив индексов граней
	std::vector<std::array<int, 3>> faces; // хранит полигоны, < ИНДЕКСЫ вершин, составляющих один полигон >
	// не все блоки, некоторые опущены за ненадобностью
};

struct file_object // объект
{
	char version[2]; // представлено в hex формате, предположительно версия(???)
	std::string user_data; // пользовательская информация
	std::string path_to_lod; // путь до лода
	uint object_type; // тип объекта
	std::vector<minobject> objs; // данные о геометрии подобъектов
	std::vector<material> materials; // массив материалов
	author auth; // автор
};

struct final_object // финальный объект, который будет сравниваться
{
private:
	std::array<std::string, 5> textures_stop;
public:
	std::set<vertex> vertexs; // массив вершин
	vertex v; // вершина, которая хранит результаты всех суммированных вершин
	std::set<std::string> textures; // все текстуры объекта(пара: материал, текстура)

	final_object();
	bool init(const char* name_file, bool b = false);
	bool init(const std::string& name_file, bool b = false) { return this->init(name_file.c_str(), b); }
	void calculate_general_vertex(); // высчитываем общую вершину
	bool operator==(const final_object& fo); // метод сравнения
	void add_texture(const std::string& texture); // добавляем текстуру к массиву

};

struct section // секция в файле scene_objects.part
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

class scene_object_out // входной/выходной объект
{
private:
	std::string name_file; // имя файла
	std::list<section> sect; // массив секций
	std::vector<std::string> temp_data; // остальные данные
public:
	scene_object_out(const std::string& n) : name_file(n) {}
	bool decompile(); // грузим файл
	std::list<section>& get_sections() { return this->sect; } // геттер для секций

	bool write_new_file(const std::string& out_path); // перезаписывае новый файл, TODO, переделать метод
};

class edit_objects // пакет лодов, которые будем пытаться заменять
{
	std::string location; // префикс локации(arg cmd)
	scene_object_out scene_object; // файл с секциями
	std::string path_sdk; // путь до сдк
	std::string path_out; // выходной путь, куда копировать объекты, для дебага
	std::map<std::string, final_object> lods_in; // входные лоды, которые будут заменяться
	std::map<std::string, final_object> lods; // лоды, НА которые будет производится замена
	bool debug; // режим отладки(копирование заменённых лодов в папку)

public:
	edit_objects(const std::string& n):scene_object(n), debug(false){}
	bool init(char** argv, int argc); // инициализируем объект
	bool load_data(); // загружаем все ресурсы в объект
	void copy_files_debug(); // копируем заменённые лоды в одну папку
	void replace_references_name(); // замена ссылок на лоды в выходном файле
	bool write_new_file(const std::string& out_path) { return scene_object.write_new_file(out_path); } // записываем новый файл с секциями
};