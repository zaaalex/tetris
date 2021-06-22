#define COUNT_SQUARE_IN_FIGURE 4
#define COUNT_FIGURE 7
#include <SFML/Graphics.hpp>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <array>
#include <time.h>
#include <sstream>

using namespace sf;

const int H = 20; // высота игрового поля
const int L = 10; // ширина игрового поля

int field[L][H] = { 0 }; // игровое поле

int figures[COUNT_FIGURE][COUNT_SQUARE_IN_FIGURE] = // Массив фигурок
{
	1,3,5,7, // I
	2,4,5,7, // S
	3,5,4,6, // Z
	3,5,4,7, // T
	2,3,5,7, // L
	3,5,7,6, // J
	2,3,4,5, // O
};

struct Point{
	int x, y;
};

enum color {BLACK, BLUE, CYAN, GREEN, MAGENTA, RED, WRITE};

bool check_out_field(std::array <Point, COUNT_SQUARE_IN_FIGURE> a, int type_check, int change_x=0) {
	bool check = true;

	if (type_check == 1) {
		for (int i = 0; i < COUNT_SQUARE_IN_FIGURE; i++) {
			a[i].x += change_x;
		}
	}
	else if (type_check == 2) {
		Point p = a[1]; // указываем центр вращения
		for (int i = 0; i < COUNT_SQUARE_IN_FIGURE; ++i) {
			int x = a[i].y - p.y, y = a[i].x - p.x; // y - y0 and x - x0
			a[i].x = p.x - x; a[i].y = p.y + y;
		}
	}

	else if (type_check == 3) {
		for (int i = 0; i < COUNT_SQUARE_IN_FIGURE; i++) {
			a[i].y += 1;
		}
	}

	for (auto const& el : a) {
		if (el.x< 0 || el.x>= L || el.y>= H) check=false;
		else if (field[el.x][el.y]) check=false;
	}
	return check;
}

int main(){
	RenderWindow window(VideoMode(270, 370), "Tetris");//создание графического окна

	Font font; font.loadFromFile("font_text.otf"); //шрифт для текста
	Text text_score("", font, 15), text_continue_game("for start new game click the mouse button", font, 10); //текст для вывода
	text_score.setFillColor(Color::Black); text_continue_game.setFillColor(Color::Black); //задание цвета текста
	Texture texture, texture_frame, texture_back; // Создание и загрузка текстуры
	texture.loadFromFile("square.jpg"); texture_frame.loadFromFile("field.jpg"); texture_back.loadFromFile("back.jpg");

	Sprite sprite_texture(texture), sprite_texture_frame(texture_frame), sprite_texture_back(texture_back);// прямоугольник+текстура = спрайт

	std::array <Point, COUNT_SQUARE_IN_FIGURE> a = {0};//массив координат каждой новой фигурки
	int horizontally = 0; bool rotate = false, first_element=true;
	Clock clock; double l_time = 0.0;  
	bool game_over = false; int score = 0;

	// Главный цикл приложения: выполняется, пока открыто окно
	while (window.isOpen()){
		if (!game_over) {
			horizontally = 0; rotate = false; double delay = 0.4; //горизонтальное перемещение-вращение-задержка при падении вниз в секундах
			l_time += clock.getElapsedTime().asSeconds(); clock.restart();

			Event event;
			while (window.pollEvent(event)) {
				if (event.type == Event::Closed) window.close();//пользователь нажал на крестик окна
				if (event.type == Event::KeyPressed) {//нажата клавиша клавиатуры
					if (event.key.code == Keyboard::Left) horizontally = -1;
					else if (event.key.code == Keyboard::Right) horizontally = 1;
					else if (event.key.code == Keyboard::Up) rotate = true;
				}
			}

			if (Keyboard::isKeyPressed(Keyboard::Down)) delay = 0.05;//если нажата кнопка вниз ускоряем падение

			if (check_out_field(a, 1, horizontally)) {
				for (int i = 0; i < COUNT_SQUARE_IN_FIGURE; ++i) {
					a[i].x += horizontally;
				}
			}

			if (rotate) {
				if (check_out_field(a, 2)) {
					Point p = a[1]; // указываем центр вращения
					for (int i = 0; i < COUNT_SQUARE_IN_FIGURE; ++i) {
						int x = a[i].y - p.y; // y - y0
						int y = a[i].x - p.x; // x - x0
						a[i].x = p.x - x; a[i].y = p.y + y;
					}
				}
			}

			int new_score = 0;//игровой счет
			int k = H - 1;//Удаление собранной линии
			for (int i = H - 1; i > 0; --i) {
				int count = 0;
				for (int j = 0; j < L; ++j) {
					if (field[j][i]) count++;
					field[j][k] = field[j][i];
				}
				if (count < L) k--;
				else {
					if (new_score == 0) new_score += 8;//собрана 1 линия
					else new_score += 12;//>1 линии
				}
			}
			score += new_score;

			if (first_element) {//задание параметров при начале игры
				first_element = false;
				srand(time(NULL));
				int type = rand() % COUNT_FIGURE;

				for (int i = 0; i < COUNT_SQUARE_IN_FIGURE; ++i) {
					a[i].x = figures[type][i] % 2;
					a[i].y = figures[type][i] / 2;
				}
			}

			if (l_time > delay) {
				if (check_out_field(a, 3)) {
					for (int i = 0; i < 4; ++i) a[i].y += 1;//опускаем фигурку вниз на клетку
				}
				else {//если фигурка приземлилась, то задаем новую фигурку
					srand(time(NULL));
					int type = rand() % COUNT_FIGURE;
					for (int i = 0; i < COUNT_SQUARE_IN_FIGURE; ++i) field[a[i].x][a[i].y] = 1;
					for (int i = 0; i < COUNT_SQUARE_IN_FIGURE; ++i) {
						a[i].x = figures[type][i] % 2;
						a[i].y = figures[type][i] / 2;
					}
				}
				l_time = 0;
			}

			window.clear(Color::White);
			window.draw(sprite_texture_back);
			window.draw(sprite_texture_frame);
			std::ostringstream player_score;
			player_score << score;
			text_score.setString("Score \n" + player_score.str());
			text_score.setPosition(205, 150);
			window.draw(text_score);

			for (int i = 0; i < L; ++i) {
				for (int j = 0; j < H; ++j) {
					if (field[i][j] != 0) {
						if (j == 1) game_over = true; //если новые фигурки будут касаться края поля
						sprite_texture.setPosition(i * 18, j * 18);
						sprite_texture.move(5, 5); // смещение
						window.draw(sprite_texture);
					}
				}
			}

			for (int i = 0; i < COUNT_SQUARE_IN_FIGURE; ++i) {
				// Устанавливаем позицию каждого квадратика фигуры
				sprite_texture.setPosition(a[i].x * 18, a[i].y * 18);//размер квадратика 18*18 пикселей
				sprite_texture.move(5, 5);
				window.draw(sprite_texture);
			}
		}
		else {
		Event event;
		while (window.pollEvent(event)) {
			if (event.type == Event::Closed) window.close();
			if (event.type == Event::MouseButtonPressed) {
				game_over = false;
				for (int i=0; i<L;++i) for (int j = 0; j < H; ++j) field[i][j] = 0;
			}
		}

		window.clear(Color::White);
		window.draw(sprite_texture_back);

		std::ostringstream player_score;
		player_score << score;
		text_score.setString("Score:" + player_score.str());
		text_score.setPosition(100, 150);
		text_continue_game.setPosition(20, 270);
		window.draw(text_score); window.draw(text_continue_game);
		}

		window.display();
	}

	return 0;
}