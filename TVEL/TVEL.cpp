#include <iostream>
#include <vector>
#include <utility>
#include <algorithm>
#include <cmath>
#include "gnuplot-iostream.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include "Temp_calc_in_fuel_el.h"
#include <Windows.h>
#include <winuser.h>
#include "Exceptions.h"

using json = nlohmann::json;

/*
Допущения:
1. Длина много больше диаметра. Теплопередачей по высоте пренебрегаем. 
2. Топливная таблетка однородна
3. Коэф. теплопроводности постоянны
*/

int main(int argc, char* argv[])
{
	try {
		setlocale(LC_ALL, "Rus");
		TVEL t;
		// Расчитывет значение температуры от радиуса и отправляет данные в v_of_temp_K
		t.temperature_calculation();
		// Сортировка неупорядоченных значений вектора v_of_temp_K
		t.sort();
		// Построение графика
		Gnuplot gp;
		gp << "set ylabel 'Температура, ^ K' font 'Times - Roman, 16'\n ";
		gp << "set xlabel 'Радиус, ^ м' font 'Times - Roman, 16'\n ";
		gp << "set xrange [0:0.007]\nset yrange [0:2500]\n";
		gp << "set terminal pngcairo\n";
		gp << "set output 'Температурное распределение по радиусу ТВЭЛа.png'\n";
		gp << "plot '-' lt 11 lw 2 with linespoints title "
			"'Temperature'\n";
		gp.send1d(t.get_v());

		return 0;
	}
	catch (const Exceptions& e) {
		SetConsoleCP(1251);
		std::string str = e.what();
		MessageBoxA(
			NULL,
			(LPCSTR)str.c_str(),
			NULL,
			MB_ICONERROR | MB_OK
		);
	}
	catch (...) {
		std::cout << "Неизвестная ошибка.";
	}
}
