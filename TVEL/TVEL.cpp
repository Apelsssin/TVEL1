#include <iostream>
#include <vector>
#include <utility>
#include <algorithm>
#include <cmath>
#include "gnuplot-iostream.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include "TVEL1.h"

using json = nlohmann::json;

/*
Допущения:
1. Длина много больше диаметра. Теплопередачей по высоте пренебрегаем. 
2. Топливная таблетка однородна
3. Коэф. теплопроводности постоянны
*/

int main(int argc, char* argv[])
{
	setlocale(LC_ALL, "Rus");

	TVEL t;
	t.temperature_calculation();
	t.sort();
	
	Gnuplot gp;
	gp << "set ylabel 'Температура, ^ K' font 'Times - Roman, 16'\n ";
	gp << "set xlabel 'Радиус, ^ м' font 'Times - Roman, 16'\n ";
	gp << "set xrange [0:0.007]\nset yrange [0:2500]\n";
	gp << "plot '-' lt 11 lw 2 with linespoints title "
		"'Temperature'\n";
	gp.send1d(t.get_v());

	return 0;
}