#pragma once
#include <iostream>
#include <vector>
#include <utility>
#include <algorithm>
#include <cmath>
#include "gnuplot-iostream.h"
#include <nlohmann/json.hpp>
#include <fstream>

using json = nlohmann::json;

class TVEL {
	float fuel_el_step; // Шаг размешения ТВЭЛов
	const float pi = 3.14159;
	float delta_shell; //Толщина оболочки ТВЭЛа [м]
	float delta_He; //Толщина газового зазора [м]
	float outer_radius_fuel_tablet; //Наружний радиус топливной таблетки [м]
	float inner_radius_fuel_tablet; //Внутренний радиус топливной таблетки [м]
	float coolant_temp_in_K; //Температура теплоносителя на входе [K]
	float coolant_temp_out_K; //Температура теплоносителя на выходе [K]
	float q_v; //Обьемное энерговыделение топлива [Вт/м3]
	float r_step; //Шаг построения графика по радиусу [м]
	float Cp; // Теплоемкость теплоносителя 
	float coolant_lambda; // [Вт/м*К]
	float fuel_lambda;// [Вт/м*К]
	float He_lambda; // [Вт/м*К]
	float shell_lambda;// [Вт/м*К]
	float K_r_max; // []
	float K_z;//[]
	long double l; //[м]
	size_t n_assembly; //Число ТВС
	size_t n_fuel_el; //Число ТВЭЛ
	std::vector <std::pair <float, float>> v_of_temp_K_r; //Вектор расперделения температур по радиусу в Кельвинах
	float rho_coolant_avg;//[кг/м^3]
	float mu_coolant_avg;//Динамическая вязкость [Па*с]
	long double N_t;// 3200 / 163 / 312 * 1000000; //[Вт] на 1 ТВЭЛ (62923)
	long double Q_1; // 88000 / 163 / 312 / 3600; // Расход через сечение 1 ТВЭЛ (0.00048066) [м/с]

public:
	TVEL() {

		std::ifstream file;
		file.open(R"(consts.json)", std::ios::in);
		json Doc{ json::parse(file) };
		for (auto& [key, value] : Doc.items()) {
			if (key == "fuel_el_step") fuel_el_step = value;
			else if (key == "delta_shell") delta_shell = value;
			else if (key == "delta_He") delta_He = value;
			else if (key == "outer_radius_fuel_tablet") outer_radius_fuel_tablet = value;
			else if (key == "inner_radius_fuel_tablet") inner_radius_fuel_tablet = value;
			else if (key == "coolant_temp_in_K") coolant_temp_in_K = value;
			else if (key == "coolant_temp_out_K") coolant_temp_out_K = value;
			else if (key == "q_v") q_v = value;
			else if (key == "r_step") r_step = value;
			else if (key == "Cp") Cp = value;
			else if (key == "coolant_lambda") coolant_lambda = value;
			else if (key == "fuel_lambda") fuel_lambda = value;
			else if (key == "He_lambda") He_lambda = value;
			else if (key == "shell_lambda") shell_lambda = value;
			else if (key == "K_r_max") K_r_max = value;
			else if (key == "K_z") K_z = value;
			else if (key == "l") l = value;
			else if (key == "n_assembly") n_assembly = value;
			else if (key == "n_fuel_el") n_fuel_el = value;
			else if (key == "rho_coolant_avg") rho_coolant_avg = value;
			else if (key == "mu_coolant_avg") mu_coolant_avg = value;
			else if (key == "N_t") N_t = value;
			else if (key == "Q_1") Q_1 = value;

		}
	}
	void calculation();
	std::vector <std::pair <float, float>> get_v();
	void sort();

	TVEL(const TVEL&) = delete;
	void operator= (const TVEL&) = delete;
};