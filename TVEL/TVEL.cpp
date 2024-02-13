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
1. Длина много больше диаметра. Теплопередачей по высоте пренебрегаем.;
2. Топливная таблетка однородна;
3. Коэф. теплопроводности постоянны;
4. Отношение значений чисел Прандтля пристеночной области к области, находящейся за пограничными слоями, примерно равны 1.
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
		gp << "set xlabel 'Радиус, ^ мм' font 'Times - Roman, 16'\n ";
		gp << "set xrange [0:7]\nset yrange [0:1800]\n";
		gp << "set terminal pngcairo\n";
		gp << "set grid\n";
		gp << "set output 'Температурное распределение по радиусу ТВЭЛа.png'\n";
		gp << "plot '-' lt 11 lw 2 with linespoints title "
			"'Temperature calculation in program', 'Validation_data.txt' with points pt 7 lt 1\n";
		gp.send1d(t.get_v());


		return 0;
	}  //gnuplot> plot "stats.txt" u 1:2 title "Heart disease" w l, "" u 1:3 title "Cancer" w l, "" 
	//u 1:4 title "Cerebro-vascular diseases" w l, "" u 1:5 title "Lower respiratory diseases" w l, "" u 1:6 title 
		//"Diabetes mellitus" w l, "" u 1:7 title "Influenza and pneumonia" w l, "" u 1:8 title "Chronic liver disease" w l, "" 
		//u 1:9 title "Accidents" w l, "" u 1:10 title "Suicide" w l, "" u 1:11 title "Homicide" w l
	// Выброс окна об ошибке
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
