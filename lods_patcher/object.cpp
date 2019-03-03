#include "object.h"

// локаль
Locale::Locale(const int& l)
{
	cp = GetConsoleCP();
	output_cp = GetConsoleOutputCP();
	SetConsoleCP(l);
	SetConsoleOutputCP(l);
}

Locale::~Locale()
{
	SetConsoleCP(cp);
	SetConsoleOutputCP(output_cp);
}

// вершина
vertex vertex::operator+(const vertex & v)
{
	vertex vert(*this);
	vert.x += v.x;
	vert.y += v.y;
	vert.z += v.z;
	return vert;
}

bool vertex::operator==(const vertex & v)
{
	return this->x == v.x && this->y == v.y && this->z == v.z;
}

bool vertex::operator!=(const vertex & v)
{
	return !(*this == v);
}

bool vertex::equals_epsilon(const vertex & v)
{
	float epsilon(0.01f); // 0.01f
	return this->x - v.x < epsilon && this->y - v.y < epsilon && this->z - v.z < epsilon;
}

bool vertex::equals_epsilon(const vertex & v) const
{
	float epsilon(0.01f); // 0.01f
	return this->x - v.x < epsilon && this->y - v.y < epsilon && this->z - v.z < epsilon;
}

bool vertex::operator<(const vertex & v) const
{
	if (this->x == v.x) // abs(this->x - v.x) < 0.01f
	{
		if (this->y == v.y) // abs(this->y - v.y) < 0.01f
		{
			if (this->z >= v.z)
				return false;
			else
				return true;
		}
		else if (this->y > v.y)
			return false;
		else
			return true;
	}
	else if (this->x > v.x)
		return false;
	else
		return true;	
}

std::ostream & operator<<(std::ostream & ostream, const vertex & v)
{
	return ostream << "Vertex: [" << v.x << ";" << v.y << ";" << v.z << "]" << std::endl;
}

// данные о материале
// NULL

// данные о создании
// NULL

// данные о геометрии
// NULL

// объект
// NULL

// финальный объект, который будет сравниваться
final_object::final_object()
{
	textures_stop[0] = "trees\\trees_fuflo";
	textures_stop[1] = "trees\\trees_fake";
	textures_stop[2] = "trees\\tree_fuflo";
	textures_stop[3] = "trees\\trees_sosna_sux"; // по - идее эта текстура мешает
	textures_stop[4] = "prop\\prop_fake_kollision";

	// TODO, остальные текстуры, которые не надо обрабатывать
}

bool final_object::init(const char * name_file, bool b)
{
	std::ifstream file(name_file, std::ios_base::binary);
	if (!file)
		return false;

	// переменная - файл object
	file_object fObject;

	// размер файла
	file.seekg(0, std::ios_base::end);
	int size_file = file.tellg();
	file.seekg(0);

	if (size_file < 12)
	{
		std::cout << "Файл слишком маленький." << std::endl;
		file.close();
		return false;
	}

	// читаем блок файла
	// эти данные хранить НЕ нужно, они ОБЩИЕ

	uint block = 0; // id блока
	uint _size = 0; // размер блока
	time_t date = 0; // дата в числовом виде
	tm tml; // форматированная дата
	// tmp
	uint buf(0);
	unsigned short c(0);
	uint tmp(0);

	if (!read_block_and_size(file, block, _size))
		return false;

	if (block != 0x7777)
	{
		std::cout << "Ошибка: Файл не соответсвует нужной структуре." << std::endl;
		file.close();
	}

	// начинаем читать блоки пока не закончится буфер
	while (true)
	{
		// попытка чтения файла, если не удачно - выходим из цикла
		char _tmp(' ');
		file.read((char*)&_tmp, 1);
		if (!file)
			break;
		else
			file.seekg(-1, std::ios::cur);

		// сначала читаем блок
		if (!read_block_and_size(file, block, _size))
			return false;

		switch (block) // обработчик всех блоков файла
		{
		case 0x0900: // версия (но чего???)
		{
			std::cout << "Version(???) block: " << "0x" << std::setw(4) << std::setfill('0') << std::hex << block << "\n";

			file.read(fObject.version, sizeof(fObject.version));
			if (!file)
				return false;
			std::copy_if(fObject.version, fObject.version + 2, std::ostream_iterator<int>(std::cout, " "), [](const int& elem) -> std::ostream& {
				return std::cout << std::hex << "0x" << std::setw(2) << std::setfill('0');
			});
			std::cout << std::endl << std::endl;
			break;
		}

		case 0x0912: // user data
		{
			std::cout << "User data block: " << "0x" << std::setw(4) << std::setfill('0') << std::hex << block << "\n";

			fObject.user_data = read_binary_text(file);
			std::cout << fObject.user_data << std::endl << std::endl;
			break;
		}

		case 0x0925: // lod или нет
		{
			std::cout << "Lod block: " << "0x" << std::setw(4) << std::setfill('0') << std::hex << block << "\n";

			fObject.path_to_lod = read_binary_text(file);
			std::cout << fObject.path_to_lod << std::endl << std::endl;
			break;
		}

		case 0x0903: // тип модели(лод, статик, хом и т.д.)
		{
			// static - 0x00
			// dynamic - 0x01
			// hom - 0x08
			// multiple usage(lod) - 0x14
			// progressive dynamic(???) - 0x03
			// sound occluder - 0x20
			std::cout << "Object type block: " << "0x" << std::setw(4) << std::setfill('0') << std::hex << block << "\n";

			file.read((char*)&fObject.object_type, sizeof(fObject.object_type));
			if (!file)
				return false;
			std::cout << std::hex << "0x" << std::setw(2) << std::setfill('0') << fObject.object_type << std::endl << std::endl;
			break;
		}

		case 0x0910: // данные о геометрии. Один большой блок
		{
			std::cout << "Geometry block: " << "0x" << std::setw(4) << std::setfill('0') << std::hex << block << "\n\n";
			while (true)
			{
				// пропускаем размер блока + данные этого блока
				if (!read_block_and_size(file, block, _size))
					return false;

				// сначала читаем блок //
				if (!read_block_and_size(file, block, _size))
					return false;
				if (block != 0x1000) // блоки геометрии закончились
				{
					file.seekg(-16, std::ios::cur); // сдвигаем позицию чтения назад на 16 байт
					break; // выходим
				}

				std::cout << "Object type block №" << fObject.objs.size() + 1 << "\n";
				minobject obj;

				if (block == 0x1000) // неизвестные данные
				{
					file.read(obj.unknown1, sizeof(obj.unknown1));
					if (!file)
						return false;
					std::cout << "Unknown data 1: ";
					std::copy_if(obj.unknown1, obj.unknown1 + 2, std::ostream_iterator<int>(std::cout, " "), [](const int& elem) -> std::ostream& {
						return std::cout << std::hex << "0x" << std::setw(2) << std::setfill('0');
					});
					std::cout << std::endl;
				}
				else
					return false;

				// сначала читаем блок // 
				if (!read_block_and_size(file, block, _size))
					return false;
				if (block == 0x1001)// имя подобъекта
				{
					obj.name = read_binary_text(file); std::cout << "Name object: " << obj.name << std::endl;
				}

				// сначала читаем блок // 
				if (!read_block_and_size(file, block, _size))
					return false;
				if (block == 0x1004)// центр модели
				{
					vertex* arr[] = { &obj.bbox_min, &obj.bbox_max };
					for (int i(0); i < 2; i++)
					{
						file.read((char*)&arr[i]->x, sizeof(float));
						if (!file)
							return false;
						file.read((char*)&arr[i]->y, sizeof(float));
						if (!file)
							return false;
						file.read((char*)&arr[i]->z, sizeof(float));
						if (!file)
							return false;
					}
					std::cout << "BBox min: [" << obj.bbox_min.x << ";" << obj.bbox_min.y << ";" << obj.bbox_min.z << "]" << std::endl;
					std::cout << "BBox max: [" << obj.bbox_max.x << ";" << obj.bbox_max.y << ";" << obj.bbox_max.z << "]" << std::endl;
				}
				else
					return false;

				// сначала читаем блок // 
				if (!read_block_and_size(file, block, _size))
					return false;
				if (block == 0x1002)//неизвестные данные
				{
					file.read((char*)&obj.unknown2, sizeof(obj.unknown2)); // нерасшифровываемое значение
					if (!file)
						return false;
					std::cout << "Unknown data 2: " << std::hex << "0x" << std::setw(2) << std::setfill('0') << obj.unknown2 << std::endl;
				}
				else
					return false;

				// сначала читаем блок //  
				if (!read_block_and_size(file, block, _size))
					return false;
				if (block == 0x1010)//неизвестные данные
				{
					file.read(obj.unknown3, sizeof(obj.unknown3));
					if (!file)
						return false;
					std::cout << "Unknown data 3: ";
					std::copy_if(obj.unknown3, obj.unknown3 + 8, std::ostream_iterator<int>(std::cout, " "), [](const int& elem) -> std::ostream& {
						return std::cout << std::hex << "0x" << std::setw(2) << std::setfill('0');
					});
					std::cout << std::endl;
				}
				else
					return false;

				// сначала читаем блок // 
				if (!read_block_and_size(file, block, _size))
					return false;
				if (block == 0x1005)// массив вершин
				{
					int count(0);
					file.read((char*)&count, sizeof(count));
					for (int i(0); i < count; i++)
					{
						vertex _v;
						file.read((char*)&_v.x, sizeof(float));
						if (!file)
							return false;
						file.read((char*)&_v.y, sizeof(float));
						if (!file)
							return false;
						file.read((char*)&_v.z, sizeof(float));
						if (!file)
							return false;
						obj.vert.push_back(_v);
						std::cout << "№" << std::to_string(i) << ":" << _v;
					}
				}
				else
					return false;

				// сначала читаем блок // 
				if (!read_block_and_size(file, block, _size))
					return false;
				if (block == 0x1006)// грани(faces)
				{
					file.read((char*)&tmp, sizeof(uint)); 
					if (!file)
						return false;
					for (int i(0); i < tmp; i++)
					{
						obj.faces.push_back({}); // добавляем пустой полигон
						for (int ind(0), j(0); ind < 3; ind++)
						{
							file.read((char*)&buf, sizeof(uint));
							if (!file)
								return false;
							obj.faces.back()[j++] = buf;
							file.read((char*)&buf, sizeof(uint)); // ненужный индекс текстурной координаты
							if (!file)
								return false;
						}
					}
				}
				else
					return false;

				// сначала читаем блок // 
				if (!read_block_and_size(file, block, _size))
					return false;
				if (block == 0x1013)// пропускаем группы сглаживания
				{
					for (int i(0); i < tmp; i++)
					{
						file.read((char*)&buf, sizeof(uint));
						if (!file)
							return false;
					}
				}
				else
					return false;

				// сначала читаем блок // 
				if (!read_block_and_size(file, block, _size))
					return false;
				if (block == 0x1008)// попускаем словарь текстурных координат
				{
					file.read((char*)&tmp, sizeof(uint)); 
					if (!file)
						return false;
					int size(tmp);
					for (int i(0); i < size; i++)
					{
						char* temp;
						file.read((char*)&temp, 1);
						if (!file)
							return false;
						file.read((char*)&tmp, sizeof(uint));
						if (!file)
							return false;
						file.read((char*)&tmp, sizeof(uint));
						if (!file)
							return false;
					}
				}
				else
					return false;

				// сначала читаем блок // 
				if (!read_block_and_size(file, block, _size))
					return false;
				if (block == 0x1009)// материалы подобъекта
				{
					file.read((char*)&c, sizeof(c));
					if (!file)
						return false;
					for (int i(0); i < c; i++)
					{
						obj.materials.push_back({ read_binary_text(file), {} });
						std::cout << "Material №" << std::to_string(i + 1) << ": " << obj.materials.back().first << std::endl;
						file.read((char*)&tmp, sizeof(uint));
						if (!file)
							return false;
						std::vector<int> indexs;
						for (int j(0); j < tmp; j++)
						{
							file.read((char*)&buf, sizeof(uint));
							indexs.push_back(buf);
							if (!file)
								return false;
						}
						obj.materials.back().second = indexs;
					}
				}
				else
					return false;

				// сначала читаем блок // 
				if (!read_block_and_size(file, block, _size))
					return false;
				if (block == 0x1012)// пропускаем блок текстурные координаты
				{
					file.read((char*)&tmp, sizeof(uint));
					if (!file)
						return false;
					for (int i(0); i < tmp; i++)
					{
						std::string temp = read_binary_text(file);
						file.read((char*)&c, sizeof(c));
						if (!file)
							return false;
						char num_table(' ');
						file.read((char*)&num_table, 1);
						if (!file)
							return false;
						file.read((char*)&buf, sizeof(uint));
						if (!file)
							return false;
						for (int j(0); j < buf * 2; j++)
						{
							float tmp_1(0.f);
							file.read((char*)&tmp_1, sizeof(float));
							if (!file)
								return false;
						}
						uint _temp(0);
						for (int j(0); j < buf; j++)
						{
							file.read((char*)&_temp, sizeof(_temp));
							if (!file)
								return false;
						}
						if (i > 0)
						{
							for (int j(0); j < buf; j++)
							{
								file.read((char*)&_temp, sizeof(_temp));
								if (!file)
									return false;
							}
						}
					}
				}
				else
					return false;

				fObject.objs.push_back(obj);
				std::cout << std::endl;
			}

			std::cout << std::endl;
			break;
		}

		case 0x0907: // материалы объекта
		{
			std::cout << "Material block(-s): " << "0x" << std::setw(4) << std::setfill('0') << std::hex << block << "\n";

			uint count(0);
			file.read((char*)&count, sizeof(uint));
			if (!file)
				return false;
			for (int i(0); i < count; i++)
			{
				material m;
				m.name = read_binary_text(file); std::cout << "Name material: " << m.name << std::endl;
				m.shader = read_binary_text(file); std::cout << "Name shader: " << m.shader << std::endl;
				m.compile = read_binary_text(file); std::cout << "Name compile: " << m.compile << std::endl;
				m.gamemtl = read_binary_text(file); std::cout << "Name game material: " << m.gamemtl << std::endl;
				m.texture = read_binary_text(file); std::cout << "Name texture: " << m.texture << std::endl;
				std::transform(m.texture.begin(), m.texture.end(), m.texture.begin(), tolower); // опускаем всю текстуру в нижний регистр
				m.unknown = read_binary_text(file); std::cout << "Unknown(???): " << m.unknown << std::endl;
				file.read((char*)&m.double_sides, sizeof(m.double_sides));
				if (!file)
					return false;
				std::cout << std::hex << "Double sides: 0x" << std::setw(2) << std::setfill('0') << m.double_sides << std::endl;
				std::cout << "Unknown data(???): ";
				file.read(m.unknown_data, sizeof(m.unknown_data));
				if (!file)
					return false;
				std::copy_if(m.unknown_data, m.unknown_data + 8, std::ostream_iterator<int>(std::cout, " "), [](const int& elem) -> std::ostream& {
					return std::cout << std::hex << "0x" << std::setw(2) << std::setfill('0');
				});
				std::cout << std::endl << std::endl;
				fObject.materials.push_back(m);
			}
			std::cout << "Material block(-s) end" << std::endl << std::endl;
			break;
		}

		case 0x0922: // блок авторы
		{
			std::cout << "Author block: " << "0x" << std::setw(4) << std::setfill('0') << std::hex << block << "\n";

			fObject.auth.creator = read_binary_text(file); std::cout << "Author: " << fObject.auth.creator << std::endl;

			std::cout << "Date create: ";
			file.read(reinterpret_cast<char*>(&date), 4);
			if (!file)
				return false;
			gmtime_s(&tml, &date);
			std::cout << std::dec << std::setw(2) << std::setfill('0') << tml.tm_mday << ":" << std::setw(2) << std::setfill('0') << 1 + tml.tm_mon << ":" << 1900 + tml.tm_year <<
				" " << std::setw(2) << std::setfill('0') << tml.tm_hour << ":" << std::setw(2) << std::setfill('0') << tml.tm_min << ":" <<
				std::setw(2) << std::setfill('0') << tml.tm_sec << std::endl;
			fObject.auth.date_creator = tml;

			fObject.auth.modifycator = read_binary_text(file); std::cout << "Modifycator: " << fObject.auth.modifycator << std::endl;

			std::cout << "Date modify: ";
			file.read(reinterpret_cast<char*>(&date), 4);
			if (!file)
				return false;
			gmtime_s(&tml, &date);
			std::cout << std::dec << std::setw(2) << std::setfill('0') << tml.tm_mday << ":" << std::setw(2) << std::setfill('0') << 1 + tml.tm_mon << ":" << 1900 + tml.tm_year <<
				" " << std::setw(2) << std::setfill('0') << tml.tm_hour << ":" << std::setw(2) << std::setfill('0') << tml.tm_min << ":" <<
				std::setw(2) << std::setfill('0') << tml.tm_sec << std::endl;
			fObject.auth.date_modify = tml;
			std::cout << std::endl << std::endl;
			break;
		}

		default: // неизвестные данные
			break;
		}
	}

	// складываем нужные вершины(без стоп - текстур(материалов))
	for (auto& mobj : fObject.objs) // перебираем все подобъекты
	{
		for(auto& mat: mobj.materials) // перебираем все материалы подобъекта
		{
			auto iter = std::find_if(fObject.materials.begin(), fObject.materials.end(), [&mat](const material& m) {
				return mat.first == m.name;
			}); // получаем текстуру по материалу
			if (iter->compile.find("flora_collision") != -1) // если настройка для компилятора == стоп - настройке
				continue; // пропускаем эту текстуру
			this->add_texture(iter->texture); // попытка добавить текстуру в массив всех текстур объекта
			auto iter1 = std::find_if(this->textures.begin(), this->textures.end(), [&iter](const std::string& t) {
				return iter->texture == t;
			}); // узнаем, есть ли текстура в общем массиве текстур объекта
			if (iter1 != this->textures.end()) // текстура есть
			{
				// добавляем в массив вершин вершины, соответствующие ИНДЕКСАМ материала
				auto * v = &this->vertexs;
				std::for_each(mat.second.begin(), mat.second.end(), [&mobj, v](const int& i) {
					auto ind = mobj.faces[i]; // получили три индекса
					for (int i(0); i < 3; i++)
						v->insert(mobj.vert[ind[i]]); // пытаемся добавить вершину
				});
			}
		}
	}

	// end instruction
	file.close();

	// рассчитываем общую вершину
	this->calculate_general_vertex();
	return true;
}

void final_object::calculate_general_vertex()
{
	v = std::accumulate(vertexs.begin(), vertexs.end(), vertex(0.f, 0.f, 0.f), [](vertex vert, vertex left) {
		return vert + left;
	});
}

bool final_object::operator==(const final_object & fo)
{
	bool result(true);

	// сравниваем
	bool a(false); // сначала вершины
	bool b(false); // количество текстур
	bool c(false); // затем контрольную вершину
	bool d(false); // и последний этап - cравниваем все текстуры


	if (this->vertexs.size() != fo.vertexs.size()) // если вершины не равны
	{
		// сравниваем их по значению
		auto *first = this->vertexs.size() < fo.vertexs.size() ? &this->vertexs : &fo.vertexs;
		auto *second = this->vertexs.size() > fo.vertexs.size() ? &this->vertexs : &fo.vertexs;
		int count_vertexs(0);
		int max_count_vertexs(first->size());
		for (auto iter = first->begin(); iter != first->end(); iter++) 
		{
			if (second->find(*iter) == second->end())
				count_vertexs++;
		}
		a = count_vertexs != max_count_vertexs ? false : true;
		result &= a;
	}
	else
	{
		//иначе сравниваем сумму вершин
		a = true;
		result &= a;
		c = this->v.equals_epsilon(fo.v);	// затем контрольную вершину
		result &= c;
	}

	b = this->textures.size() == fo.textures.size();	// количество текстур
	result &= b;
	d = this->textures == fo.textures;	// и последний этап - cравниваем все текстуры
	result &= d;

	return result;
}

void final_object::add_texture(const std::string& texture)
{
	int pos(0);
	for (auto& t : textures_stop)
		if ((pos = texture.find(t)) != -1)
		{
			if(pos + texture.length() == t.length()) // дополниельная проверка, что это НЕ ЧАСТЬ слова
				return;
		}
	this->textures.insert( texture );
}

// входной/выходной объект
bool scene_object_out::decompile()
{
	std::ifstream file(name_file);
	if (!file)
	{
		std::cout << "Error open file " << name_file << std::endl;
		return false;
	}
	std::string tmp;
	while (true) // пропускаем шапку
	{
		std::getline(file, tmp, '\n');
		if (!file)
		{
			std::cout << "Error read file " << name_file << std::endl;
			return false;
		}
		if (tmp.find("[object_") != -1) // нашли первую секцию - останавливаем проверку объектов
			break;
		temp_data.push_back(tmp);
	}
	while (true) // грузим все секции 
	{
		section sc;
		tmp.resize(tmp.length() - 1);
		tmp.erase(0, 8);
		sc.section_number = tmp;
		std::getline(file, sc.clsid, '\n'); // clsid
		if (!file)
		{
			std::cout << "Error read file " << name_file << std::endl;
			return false;
		}
		std::getline(file, sc.co_flags, '\n'); // co_flags
		if (!file)
		{
			std::cout << "Error read file " << name_file << std::endl;
			return false;
		}
		std::getline(file, sc.flags, '\n'); // flags
		if (!file)
		{
			std::cout << "Error read file " << name_file << std::endl;
			return false;
		}
		std::getline(file, sc.name, '\n'); // name
		if (!file)
		{
			std::cout << "Error read file " << name_file << std::endl;
			return false;
		}
		std::getline(file, sc.position, '\n'); // position
		if (!file)
		{
			std::cout << "Error read file " << name_file << std::endl;
			return false;
		}
		std::getline(file, sc.reference_name, '\n'); // reference_name
		if (!file)
		{
			std::cout << "Error read file " << name_file << std::endl;
			return false;
		}
		std::getline(file, sc.rotation, '\n'); // rotation
		if (!file)
		{
			std::cout << "Error read file " << name_file << std::endl;
			return false;
		}
		std::getline(file, sc.scale, '\n'); // scale
		if (!file)
		{
			std::cout << "Error read file " << name_file << std::endl;
			return false;
		}
		std::getline(file, sc.version, '\n'); // version
		if (!file)
		{
			std::cout << "Error read file " << name_file << std::endl;
			return false;
		}
		sect.push_back(sc); // добавляем секцию
		std::getline(file, tmp, '\n'); // пропускаем пробел

		std::getline(file, tmp, '\n'); // section number
		if (file.eof())
			break;
		if (!file)
		{
			std::cout << "Error read file " << name_file << std::endl;
			return false;
		}
	}
	file.close();
	return true;
}

bool scene_object_out::write_new_file(const std::string & out_path)
{
	std::ofstream file(out_path);
	file.imbue(std::locale("rus_rus.1251"));
	if (!file)
	{
		std::cout << "Error create file " << out_path << std::endl;
		return false;
	}
	for (auto& data : this->temp_data)
	{
		file << data << std::endl;
		if (!file)
		{
			std::cout << "Error write file " << out_path << std::endl;
			return false;
		}
	}

	int y(0);
	for (auto& sc : this->sect) // записываем все секции 
	{
		file << "[object_" << sc.section_number << "]" << std::endl;
		if (!file)
		{
			std::cout << "Error write file " << out_path << std::endl;
			return false;
		}
		file << sc.clsid << std::endl;
		if (!file)
		{
			std::cout << "Error write file " << out_path << std::endl;
			return false;
		}
		file << sc.co_flags << std::endl;
		if (!file)
		{
			std::cout << "Error write file " << out_path << std::endl;
			return false;
		}
		file << sc.flags << std::endl;
		if (!file)
		{
			std::cout << "Error write file " << out_path << std::endl;
			return false;
		}
		file << sc.name << std::endl;
		if (!file)
		{
			std::cout << "Error write file " << out_path << std::endl;
			return false;
		}
		file << sc.position << std::endl;
		if (!file)
		{
			std::cout << "Error write file " << out_path << std::endl;
			return false;
		}
		file << sc.reference_name << std::endl;
		if (!file)
		{
			std::cout << "Error write file " << out_path << std::endl;
			return false;
		}
		file << sc.rotation << std::endl;
		if (!file)
		{
			std::cout << "Error write file " << out_path << std::endl;
			return false;
		}
		file << sc.scale << std::endl;
		if (!file)
		{
			std::cout << "Error write file " << out_path << std::endl;
			return false;
		}
		file << sc.version << std::endl << std::endl;
		if (!file)
		{
			std::cout << "Error write file " << out_path << std::endl;
			return false;
		}

	}
	file.close();
	return true;
}

// пакет лодов, которые будем пытаться заменять
bool edit_objects::init(char ** argv, int argc)
{
	for (int i(1); i < argc; i++)
	{
		if (!strcmp(argv[i], "-editor") && i + 1 != argc)
		{
			path_sdk = argv[++i];
			if (*(path_sdk.end() - 1) != '\\')
				path_sdk += '\\';
			path_sdk += "rawdata\\objects\\";
		}
		else if (!strcmp(argv[i], "-out") && i + 1 != argc)
		{
			path_out = argv[++i];
			if (*(path_out.end() - 1) != '\\')
				path_out += '\\';
		}
		else if (!strcmp(argv[i], "-l") && i + 1 != argc)
		{
			location = argv[++i];
		}
		else if (!strcmp(argv[i], "-d"))
			debug = true;
		else
		{
			std::cout << "Error parsing args for cmd!" << std::endl;
			return false;
		}
	}
	return true;
}

bool edit_objects::load_data()
{
	if (!scene_object.decompile()) // сначала грузим секции файла
		return 0;
	// теперь грузим все лоды, которые будем менять
	std::regex rgx(R"(([\w\\]+)(_lod)([\d]{1,5}))"); // регулярное выражение - лод 
	std::cmatch result; // результат регулярки
	int i(0);
	for (auto& sc : this->scene_object.get_sections())
	{
		std::string _path(sc.reference_name); _path.erase(0, _path.find("= ") + 2);
		if (std::regex_match(_path.c_str(), result, rgx) && this->lods_in.find(_path) == this->lods_in.end()) // если лод такого с таким именем еще не загружен
		{
			std::cout << "Scene object section №" << std::to_string(i) << std::endl;
			// ключ - путь до лода(без пути до сдк и расширения, из references_name)
			std::string key(_path);
			_path.insert(0, path_sdk); _path += ".object";
			final_object fobject;
			if (!fobject.init(_path))
				return false;
			this->lods_in.insert({ key, fobject }); // добавляем загруженный лод в карту
		}
		i++;
	}

	// теперь грузим все лоды, НА которые будем пытаться заменять
	std::string paths[] = { "trees", "statics\\vehicles" };
	std::map<std::string, final_object>* _lods = &this->lods;
	i = 0;
	for (auto& p : paths) // грузим файлы по очереди из двух каталогов, содержащих лоды
	{
		std::for_each(fs::recursive_directory_iterator(path_sdk + p), fs::recursive_directory_iterator(), [_lods, &i](const fs::path & path) {
			if (path.extension() == ".object") // если встретился объект
			{
				std::cout << "Object lod №" << std::to_string(i++) << std::endl;
				final_object fobject;
				if (fobject.init(path.string())) // если загруженный лод считался корректно
				{
					std::string key = path.string(); key.erase(0, key.find("cts\\") + 4); key.erase(key.find(".object"));
					_lods->insert({ key, fobject }); // добавляем загруженный лод
				}
			}
		});
	}
	return true;
}

void edit_objects::copy_files_debug()
{
	if (!debug)
		return;

	std::ofstream file("copy_lods_debug.log");
	if (!file)
	{
		std::cout << "Error create log file! Out..." << std::endl;
		return;
	}
	for (auto& p : this->lods_in) // перебираем все исходные файлы
	{
		bool result(false);

		for (auto& np : this->lods) // перебираем все заменяющие лоды
		{
			if (p.second == np.second) // сравниваем объекты
			{
				std::string name_out(p.first); name_out.erase(0, name_out.find_last_of("\\") + 1);
				name_out.insert(0, "debug_info\\edit_lods\\");
				std::error_code err;
				fs::copy(fs::path(path_sdk + np.first + ".object"), fs::path(name_out + ".object"), err);
				std::cout << "lod " << path_sdk << np.first << ".object copy in " << name_out << ".object: ";
				file << "lod " << path_sdk << np.first << ".object copy in " << name_out << ".object: ";
				if (!file)
				{
					std::cout << "Error write log file! Out..." << std::endl;
					return;
				}
				if (!err.value())
				{
					std::cout << "OK" << std::endl;
					file << "OK" << std::endl;
				}
				else
				{
					std::cout << "NO" << std::endl;
					file << "OK" << std::endl;
				}
				if (!file)
				{
					std::cout << "Error write log file! Out..." << std::endl;
					return;
				}
				result = true;
				break;
			}
		}
		if (!result)
		{
			std::cout << "lod " << p.first << " not found suitable copy! Copy file not produced." << std::endl;
			file << "lod " << p.first << " not found suitable copy! Copy file not produced." << std::endl;
			if (!file)
			{
				std::cout << "Error write log file! Out..." << std::endl;
				return;
			}
		}
	}
	file.close();
}

void edit_objects::replace_references_name()
{
	std::ofstream file("replace_lods.log");
	std::regex rgx(R"(([\w\s=\\]+)(_lod)([\d]{1,5}))"); // регулярное выражение - лод 
	std::cmatch result_reg; // результат регулярки

	if (!file)
	{
		std::cout << "Error create log file! Out..." << std::endl;
		return;
	}
	for (auto& p : this->lods_in) // перебираем все исходные файлы
	{
		int current_name(0);

		for (auto& np : this->lods) // перебираем все заменяющие лоды
		{
			if (p.second == np.second) // сравниваем объекты
			{
				for (auto& s : this->scene_object.get_sections()) // нашли нужный лод, перебираем все секции файла
				{
					// если секция лод и соответствует найденному лоду
					if (std::regex_match(s.reference_name.c_str(), result_reg, rgx) && s.reference_name.find(p.first) != -1) // нашли подходящую ссылку
					{
						s.reference_name.erase(s.reference_name.find("= ") + 2); // меняем ссылку
						s.reference_name += np.first;		
						s.name.erase(s.name.find("= ") + 2); // меняем имя
						std::stringstream format;
						format << np.first << "_" << std::setw(4) << std::setfill('0') << current_name++ << "_" << this->location;
						s.name += format.str();
					}

				}

				// лод был найден и успешно заменен
				std::cout << "lod " << np.first << ".object replacement in " << p.first << ".object: OK" << std::endl;
				file << "lod " << np.first << ".object replacement in " << p.first << ".object: OK" << std::endl;
				if (!file)
				{
					std::cout << "Error write log file! Out..." << std::endl;
					return;
				}
				break;
			}
		}

		// лод не был найден в списке подходящих, говорим об этом
		if (current_name == 0)
		{
			std::cout << "Cannot find suitable lod-object for " << p.first << std::endl;
			file << "Cannot find suitable lod-object for " << p.first << std::endl;
			if (!file)
			{
				std::cout << "Error write log file! Out..." << std::endl;
				return;
			}
		}

	}
	file.close();
}
