#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

#include <random>
#include <ctime>
#include <string>
#include <iostream>
#include <filesystem>


#pragma warning(disable : 4996)


//---------------------------------------------
/*
ENG:
Hi there! My name is Grub_666, I am the author of this program.
INFO: the work of this script depends of choosen font (Default: arial) and font size (Default: 100)
if you are about to change something of this, you should correct MAX_FONT_HEIGHT.
Define the highest char anf find out his height (area.height)

Actual bugs:
1.Sometimes whitespases appears without a reason in the result render
2.Russian chars can`t be showed. *I don`t know why*


RU:
Всем привет, меня зовут Grub_666, Я - автор этой программы.
ИНФО: Работа программы зависит от выбранного шрифта (Умолч: arial) и размера шрифта (Умолч: 100)
если вы собираетесь изменить что-то из этого, вам следует скорректировать MAX_FONT_HEIGHT
Определите самый высокий символ и выясните его высоту (area.height)

Актуальные баги:
1. Иногда между символами появляются пробелы
2. В конечном рендеринге не отображаются русские символы (не понимаю почему, раньше они отображались нормально)
*/
//---------------------------------------------



#define MAX_HEIGHT 18 //Maximum height of land //Максимальная высота рельефа
#define MAX_FONT_HEIGHT 94 //Supposed maximum height of font //Макисмальная высота шрифта
#define SPACE_BETWEEN_CHARS 12 //Расстояние между символами
#define SPACE_ON_BORDERS SPACE_BETWEEN_CHARS/4 //The indent of borders //Отступ от краёв

#define WAY_TO_FONT "c:/windows/fonts/arial.ttf" //Путь до шрифта
#define DEFAULT_SAVE_FOLDER "Results/"

const int WIDTH = 500; //OGL SETTINGS
const int HEIGHT = 350;

sf::RenderWindow window;

int getPixelLongOfString(std::string str) //Returns the long of string in pixels (with empty space)//Возвращает длинну строки в пикселях (с учётом пустого пространства)
{
	int result = 0;
	constexpr size_t font_size = 100;
	sf::Font fnt;
	fnt.loadFromFile(WAY_TO_FONT);

	for (int i = 0; i < str.length(); i++)
	{
		const auto area = fnt.getGlyph((wchar_t)str[i], font_size, false).textureRect;
		result += area.width;
	}

	result += SPACE_BETWEEN_CHARS * (str.length() - 1) + 2*SPACE_ON_BORDERS;
	return result;
}

//The following functions returns height for point in array
float getMaxHeightValue()
{
	return MAX_HEIGHT + sin(rand() % 10) * 1.4;
}

float getMiddleHeightValue()
{
	return MAX_HEIGHT / 3 + sin(rand() % 10) * 1.4;
}

float getZeroHeightValue()
{
	return abs(sin(rand() % 10)) * 2.1;
}


void FillArray(float** &arr, std::string str, int &arr_x_size) //Fill array of relief.
{
	//Memory for arr[][] //Выделение памяти для arr[][]
	arr_x_size = getPixelLongOfString(str) / 2;
	arr = new float*[16];
	for (int i = 0; i < 16; i++)
	{
		arr[i] = new float[arr_x_size];
	}

	constexpr size_t font_size = 100;
	float **tmp_arr;

	sf::Font fnt;

	fnt.loadFromFile(WAY_TO_FONT);

	int last_x = 0;

	for (int i = 0; i < str.length(); i++)
	{
		srand(time(nullptr));

		char su = str[i];

		const auto area = fnt.getGlyph((wchar_t)str[i], font_size, false).textureRect;
		sf::Image pixels = fnt.getTexture(font_size).copyToImage();

		int x_size = area.width / 2, y_size = 12;
		int line_distance = MAX_FONT_HEIGHT / y_size;

		//Memory for tmp_arr[][] //Выделение памяти под tmp_arr[][]
		tmp_arr = new float*[y_size];
		for (int j = 0; j < y_size; j++)
		{
			tmp_arr[j] = new float[x_size];
		}

		//Fill tmp_arr[][]//Заполнение tmp_arr[][]
		if (area.height == MAX_FONT_HEIGHT)
		{
			for (int y = 0; y < y_size; y++)
			{
				for (int x = 0; x < x_size; x++)
				{
					if (pixels.getPixel((x + 1) * 2 + area.left, (y + 1) * line_distance + area.top) == sf::Color::White)
						tmp_arr[y][x] = getMaxHeightValue();
					else if (pixels.getPixel((x + 2) * 2 + area.left,  (y + 1) * line_distance + area.top) == sf::Color::White || pixels.getPixel((x) * 2 + area.left, (y + 1) * line_distance + area.top) == sf::Color::White)
						tmp_arr[y][x] = getMiddleHeightValue();
					else
						tmp_arr[y][x] = getZeroHeightValue();
				}

				std::cout << std::endl;
			}
		}

		else
		{
			int differ = MAX_FONT_HEIGHT - area.height;
			differ /= line_distance;

			for (int y = 0; y < differ; y++) //Filling the high part of array //Заполняем верхушку пустотой 
			{
				for (int x = 0; x < x_size; x++)
				{
					tmp_arr[y][x] = getZeroHeightValue();
				}
			}

			for (int y = differ; y < y_size; y++)
			{
				for (int x = 0; x < x_size; x++)
				{
					if (pixels.getPixel((x + 1) * 2 + area.left, (y + 1 - differ) * line_distance + area.top) == sf::Color::White)
						tmp_arr[y][x] = getMaxHeightValue();
					else if (pixels.getPixel((x + 2) * 2 + area.left, (y + 1 - differ) * line_distance + area.top) == sf::Color::White || pixels.getPixel(x * 2 + area.left, (y + 1 - differ) * line_distance + area.top) == sf::Color::White)
						tmp_arr[y][x] = getMiddleHeightValue();
					else
						tmp_arr[y][x] = getZeroHeightValue();
				}
			}
		}

		//Merging tmp_arr[][] and arr[][]//Присоединение tmp_arr[][] к arr[][]
		{
			//Присоединение пустого пространства и полученного символа
			int empty_space = SPACE_BETWEEN_CHARS / 2;
			if (i == 0)
				empty_space = SPACE_ON_BORDERS / 2;

			for (int y = 0; y < 16; y++)
			{
				for (int x = last_x; x < last_x + empty_space + x_size; x++)
				{
					if (y >= 12 || x - last_x < empty_space) //Нижние четыре линии остаются свободными либо же заполнение пустого пространства
					{
						arr[y][x] = getZeroHeightValue();
					}

					else
					{
						arr[y][x] = tmp_arr[y][x - last_x - empty_space];
					}
				}
			}

			last_x += empty_space + x_size; //Обновляем значение крайнего x в arr[][]

			if (i == str.length() - 1) //Добовляем пустоту справа на последней итерации
			{
				for (int y = 0; y < 16; y++)
				{
					for (int x = last_x; x < arr_x_size; x++)
					{
						arr[y][x] = getZeroHeightValue();
					}
				}
			}
		}

		//Clearning the memory of tmp_arr[][] //Очистка памяти tmp_arr[][]
		for (int j = 0; j < y_size; j++)
		{
			delete[] tmp_arr[j];
		}
		delete[] tmp_arr;
	}
}


void FillStringRandomly(std::string &str, int length)
{
	srand(time(nullptr));
	str.clear();

	for (int i = 0; i < length; i++)
	{
		char sym;
		while (true)
		{
			sym = (char)(49 + rand() % (90 - 49 + 1)); //In ASCLI (Only A-Z and 1-9)
			if (sym < 58 || sym > 64)
				break;
		}
		str.push_back(sym);
	}
}

int main()
{
	window.create(sf::VideoMode(500, 350), "Random CAPTCHA generator", sf::Style::Close, sf::ContextSettings(32));

	srand(time(0));

	float **arr = nullptr; 
	std::string str;
	int x_size;

	std::experimental::filesystem::create_directory("Results");

	bool closeWindow = false;

	//OGL settings //Настройки OpenGL
	{
		float scale_factor = 1.0; //1.1
		glScalef(scale_factor, scale_factor, scale_factor);

		//Rotate camera
		int rotate_factor = rand() % 2;
		int rotate_angle = 20 + rand() % 15;

		if (rotate_factor == 1)
			rotate_factor = -1;
		else
			rotate_factor = 1;


		glRotatef(-55.0f, -1.0f, 0.0f, 0.0f);
		//glRotatef(rotate_angle*rotate_factor, 0.0f, 1.0f, 0.0f);

		glMatrixMode(GL_MODELVIEW);
		glEnable(GL_DEPTH);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);
		// Сглаживание точек
		glEnable(GL_POINT_SMOOTH);
		glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
		// Сглаживание линий
		glEnable(GL_LINE_SMOOTH);
		glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
		// Сглаживание полигонов    
		glEnable(GL_POLYGON_SMOOTH);
		glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

		glOrtho(-WIDTH / 2, WIDTH / 2, -HEIGHT / 2, HEIGHT / 2, -200, 200);

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	}
	
	while(true)
	{ 
		window.setVisible(false);
		int ch;
		int count;
		system("cls");
		system("color 0a");
		std::cout << "---------------CAPTCHA GENERATOR ver.1.0---------------\n\n";
		std::cout << "(All results will be saved in Results folder near this program)\n\n\n\n";
		std::cout << "Please, enter the count of the CAPTCHAS: ";
		std::cin >> count;

		ch = 0;

		window.setVisible(true);

		while (!closeWindow && ch < count + 3)
		{
			sf::Event event;

			FillStringRandomly(str, 6);
			FillArray(arr, str, x_size);

			int rotate_factor = rand() % 2;
			int rotate_angle = 20 + rand() % 15;

			if (rotate_factor == 1)
				rotate_factor = -1;
			else
				rotate_factor = 1;

			glRotatef(rotate_angle*rotate_factor, 0.0f, 1.0f, 0.0f);

			while (window.pollEvent(event))
			{
				if (event.type == sf::Event::Closed)
					closeWindow = true;
				else if (event.type == sf::Event::Resized)
					glViewport(0, 0, WIDTH, HEIGHT);
				else if (event.type == sf::Event::KeyPressed)
				{
					if (event.key.code == sf::Keyboard::Escape) closeWindow = true;
				}
			}

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glColor3f(1.0f, 1.0f, 1.0f);
			glBegin(GL_LINES);

			int z_shift = 2 * MAX_FONT_HEIGHT / 3;

			for (int i = 0; i < 16; i++)
			{
				for (int j = 0; j < x_size - 1; j++)
				{
					glVertex3f(-x_size + j * 2, arr[i][j], -(float)i * MAX_FONT_HEIGHT / 12 + z_shift);
					glVertex3f(-x_size + (j + 1) * 2, arr[i][j + 1], -(float)i * MAX_FONT_HEIGHT / 12 + z_shift);
				}
			}

			glEnd();

			sf::Image img = window.capture(); //Save the result
			std::string resul_str = DEFAULT_SAVE_FOLDER + str + ".PNG";
			img.saveToFile(resul_str);

			window.display();

			Sleep(300);

			glRotatef(-rotate_angle * rotate_factor, 0.0f, 1.0f, 0.0f);

			//Clearning the memory of arr[][]
			for (int j = 0; j < 12; j++)
			{
				delete[] arr[j];
			}

			delete[] arr;

			ch++;

			
		}
	}

	return 0;
}
