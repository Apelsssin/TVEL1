#pragma once
#include <iostream>
#include <cmath>
#include <vector>
#include <utility>
#include <algorithm>
#include <cmath>
#include "gnuplot-iostream.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include "Exceptions.h"

using json = nlohmann::json;

class TVEL {
	float fuel_el_step;	// Шаг размешения ТВЭЛов	[м]
	float delta_shell;	// Толщина оболочки ТВЭЛа	[м]
	float delta_He;	// Толщина газового зазора	[м]
	float outer_radius_fuel_tablet;	// Наружний радиус топливной таблетки	[м]
	float inner_radius_fuel_tablet;	// Внутренний радиус топливной таблетки	[м]
	float coolant_temp_in_K;	// Температура теплоносителя на входе	[K]
	float coolant_temp_out_K;	// Температура теплоносителя на выходе	[K]
	float q_v;	// Обьемное энерговыделение топлива	[Вт/м3]
	float r_step = 0.0001;	// Шаг построения графика по радиусу	[м]
	float Cp;	// Удельная теплоемкость теплоносителя	[Дж/кг*К]
	float coolant_lambda;	// Коэф. теплопроводности теплоносителя	[Вт/м*К]
	float fuel_lambda;	// Коэф. теплопроводности топливной таблетки	[Вт/м*К]
	float He_lambda;	// Коэф. теплопроводности газового зазора	[Вт/м*К]
	float shell_lambda;	// Коэф. теплопроводности оболочки ТВЭЛа	[Вт/м*К]
	float K_r_max;	// Макс. коэф. неравномерности по радиусу	[-]
	float K_z;	// Коэф. неравномерности по высоте	[-]
	const size_t n_assembly = 163;	// Число ТВС	[-]
	const size_t n_fuel_el = 312;	// Число ТВЭЛ	[-]
	std::vector <std::pair <float, float>> v_of_temp_K;	// Вектор расперделения температур по радиусу в Кельвинах
	float rho_coolant_avg;	// Средняя плотность теплоносителя	[кг/м^3]
	float mu_coolant_avg;	// Динамическая вязкость теплоносителя	[Па*с]
	float reactor_thermal_power;	// Тепловая мощность реактора	[МВт]
	float N_t;	// Тепловая мощность одного ТВЭЛа	[Вт] 
	float consumption;	// Расход через сечение 1 ТВЭЛ	[кг/с]   
	float enthalpy_in;	// Энтальпия на входе	[Дж/кг]
	float enthalpy_out;	// Энтальпия на выходе	[Дж/кг]
	float height;	// Высота обогреваемой части	[м]

public:
	TVEL() {
		// Выполняется парсинг данных из файла consts.json
		std::ifstream file;
		file.open(R"(consts.json)", std::ios::in);
		if (!file.is_open()) throw Exceptions("Попытка открыть несуществующий файл.");
		json Doc{ json::parse(file) };
		for (auto& [key, value] : Doc.items()) {
			if (value <= 0) throw Exceptions("Значение входных данных < или = 0.");
			if (key == "fuel_el_step") fuel_el_step = value;
			else if (key == "delta_shell") delta_shell = value;
			else if (key == "delta_He") delta_He = value;
			else if (key == "outer_radius_fuel_tablet") outer_radius_fuel_tablet = value;
			else if (key == "inner_radius_fuel_tablet") inner_radius_fuel_tablet = value;
			else if (key == "coolant_temp_in_K") coolant_temp_in_K = value;
			else if (key == "coolant_temp_out_K") coolant_temp_out_K = value;
			else if (key == "r_step") r_step = value;
			else if (key == "Cp") Cp = value;
			else if (key == "coolant_lambda") coolant_lambda = value;
			else if (key == "fuel_lambda") fuel_lambda = value;
			else if (key == "He_lambda") He_lambda = value;
			else if (key == "shell_lambda") shell_lambda = value;
			else if (key == "K_r_max") K_r_max = value;
			else if (key == "K_z") K_z = value;
			else if (key == "rho_coolant_avg") rho_coolant_avg = value;
			else if (key == "mu_coolant_avg") mu_coolant_avg = value;
			else if (key == "reactor_thermal_power") reactor_thermal_power = value;
			else if (key == "enthalpy_in") enthalpy_in = value;
			else if (key == "enthalpy_out") enthalpy_out = value;
			else if (key == "height") height = value;
			else throw Exceptions("Некорректные входные данные.");
		}
		// Выполняется расчет тепловой мощности и расхода для одного ТВЭЛа 
		N_t = reactor_thermal_power * 1000000 / n_assembly / n_fuel_el;
		consumption = N_t / (enthalpy_out - enthalpy_in);
	}
	// Расчитывет значение температуры от радиуса и отправляет данные в v_of_temp_K
	void temperature_calculation();
	// Геттер для v_of_temp_K
	std::vector <std::pair <float, float>> get_v();
	// Сортировка неупорядоченных значений вектора v_of_temp_K
	void sort();
	// Запрет на создание копий обьекта класса
	TVEL(const TVEL&) = delete;
	void operator= (const TVEL&) = delete;
};