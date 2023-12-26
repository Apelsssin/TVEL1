#include <iostream>
#include <vector>
#include <utility>
#include <algorithm>
#include <cmath>
#include "gnuplot-iostream.h"
#include <nlohmann/json.hpp>
#include <fstream>

using json = nlohmann::json;

/*
Допущения:
1. Длина много больше диаметра. Теплопередачей по высоте пренебрегаем. 
2. Топливная таблетка однородна
3. Коэф. теплопроводности постоянны
*/
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
					else if (key == "outer_radius_fuel_tablet") outer_radius_fuel_tablet  = value;
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
	void calculation() { 
		double S = sqrt(3) / 2 * fuel_el_step * fuel_el_step - pi * (outer_radius_fuel_tablet + delta_He + delta_shell) 
			* (outer_radius_fuel_tablet + delta_He + delta_shell); // Площадь теплоносителя 1 ТВЭЛ [м^2]
		double P = 2 * pi * (outer_radius_fuel_tablet + delta_He + delta_shell);// Смачиваемый периметр 1 ТВЭЛ [м]
		double coolant_velocity = Q_1 / S; //Скорость теплоносителя [м/с]
		long double Pe = rho_coolant_avg * Cp * coolant_velocity * l / coolant_lambda; //Число Пекле
		double delta_t = sqrt(1 / Pe) * l; // Толщина теплового пограничного слоя теплоносителя [м]
		float coolant_temp_avg_K = (coolant_temp_in_K + coolant_temp_out_K) / 2; //Средняя температура теплоносителя [К]
		float nu_coolant_avg = mu_coolant_avg / rho_coolant_avg;//Кинематическая вязкость [м^2/c]
		double d_g_assembly = 4 * S / P;//Гидравлический диаметр [м]
		double coolant_alpha = nu_coolant_avg * coolant_lambda / d_g_assembly;//[Вт/м^2*К]
		
		//Вычисляются разности температур. Они зависят только от z.
		float temp_between_max_and_outer_radius_fuel_tablet = q_v * outer_radius_fuel_tablet * outer_radius_fuel_tablet / 4 / fuel_lambda * K_r_max * K_z;
		float temp_between_outer_radius_fuel_tablet_and_inner_shell = delta_He * N_t / He_lambda * n_assembly / ((outer_radius_fuel_tablet) * 2) / pi / n_fuel_el * K_z * K_r_max;
		float temp_between_inner_and_outer_shell = delta_shell * N_t * n_assembly / shell_lambda / ((outer_radius_fuel_tablet + delta_He) * 2) / pi / n_fuel_el * K_z * K_r_max;
		float temp_between_outer_shell_and_coolant = N_t * n_assembly * K_z * K_r_max / coolant_alpha / pi / 2 / (outer_radius_fuel_tablet + delta_He + delta_shell);
		//Вычисляются значения температур
		float temp_outer_shell_K = coolant_temp_avg_K + temp_between_outer_shell_and_coolant; // [К]
		float temp_inner_shell_K = temp_outer_shell_K + temp_between_inner_and_outer_shell; // [К]
		float temp_outer_radius_fuel_tablet_K = temp_inner_shell_K + temp_between_outer_radius_fuel_tablet_and_inner_shell;//  [К]
		float temp_max_K = temp_outer_radius_fuel_tablet_K + temp_between_max_and_outer_radius_fuel_tablet;// [К]

		auto push = [&](float r, float temp) {
			std::pair <float, float> var(r, temp);
			v_of_temp_K_r.push_back(var);
			};

		push(0, temp_max_K);
		push(outer_radius_fuel_tablet, temp_outer_radius_fuel_tablet_K);
		push(outer_radius_fuel_tablet + delta_He, temp_inner_shell_K);
		push(outer_radius_fuel_tablet + delta_He + delta_shell, temp_outer_shell_K);
		push(outer_radius_fuel_tablet + delta_He + delta_shell + delta_t, coolant_temp_avg_K);
		push(fuel_el_step / 2, coolant_temp_avg_K);

		//Температурное распределение оболочки
		float pred = outer_radius_fuel_tablet + delta_He + delta_shell;
		for (float i = outer_radius_fuel_tablet + delta_He + r_step; i < pred; i += r_step) {
			float temp_var = temp_inner_shell_K - (temp_between_inner_and_outer_shell) / log(1 + delta_shell 
				/ (outer_radius_fuel_tablet + delta_He)) * log(i / (outer_radius_fuel_tablet + delta_He));
			push(i, temp_var);
		}
		//Температурное распределение таблетки
		for (float i = 0; i < outer_radius_fuel_tablet; i += r_step) {
			float temp_var = temp_outer_radius_fuel_tablet_K + q_v / 4 / fuel_lambda 
				* (outer_radius_fuel_tablet * outer_radius_fuel_tablet - i * i);
			push(i, temp_var);
		}
	 }

	auto get_v() {
		return v_of_temp_K_r;
	}

	void sort() { //Сортировка по значению радиуса по возрастанию
		std::stable_sort(v_of_temp_K_r.begin(), v_of_temp_K_r.end(),
			[&](std::pair <float, float> pair1, std::pair <float, float> pair2) {
				return (pair1.first < pair2.first);
			});
	}

	TVEL(const TVEL&) = delete;
	void operator= (const TVEL&) = delete;
};

int main(int argc, char* argv[])
{
	setlocale(LC_ALL, "Rus");

	TVEL t;
	t.calculation();
	t.sort();
	
	Gnuplot gp;
	gp << "set xrange [0:0.007]\nset yrange [0:2500]\n";
	gp << "plot '-' lt 11 lw 2 with linespoints title "
		"'Temperature'\n";
	gp.send1d(t.get_v());

	return 0;
}