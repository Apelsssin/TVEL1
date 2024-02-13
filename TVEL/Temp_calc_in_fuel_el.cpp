#define _USE_MATH_DEFINES
#include "Temp_calc_in_fuel_el.h"
#include <iostream>
#include <vector>
#include <utility>
#include <algorithm>
#include <cmath>
#include "gnuplot-iostream.h"
#include <nlohmann/json.hpp>
#include <fstream>


using json = nlohmann::json;
// Расчитывет значение температуры от радиуса и отправляет данные в v_of_temp_K
void TVEL::temperature_calculation() {
	float area = sqrt(3) / 2 * fuel_el_step * fuel_el_step - M_PI * (outer_radius_fuel_tablet + delta_He + delta_shell)
		* (outer_radius_fuel_tablet + delta_He + delta_shell);	// Площадь теплоносителя 1 ТВЭЛ	[м^2]
	float perimeter = 2 * M_PI * (outer_radius_fuel_tablet + delta_He + delta_shell);	// Смачиваемый периметр 1 ТВЭЛ	[м]
	float coolant_velocity = consumption / area / rho_coolant_avg;	// Скорость теплоносителя	[м/с]
	float d_g_assembly = 4 * area / perimeter;	// Гидравлический диаметр	[м]
	float coolant_temp_avg_K = (coolant_temp_in_K + coolant_temp_out_K) / 2;	// Средняя температура теплоносителя	[К]
	float nu_coolant_avg = mu_coolant_avg / rho_coolant_avg;	// Кинематическая вязкость	[м^2/c]

	long float Pe = rho_coolant_avg * Cp * coolant_velocity * d_g_assembly / coolant_lambda;	// Число Пекле	[-]
	float delta_t = sqrt(1 / Pe) * d_g_assembly;	// Толщина теплового пограничного слоя теплоносителя	[м]
	float Re = coolant_velocity * d_g_assembly / nu_coolant_avg;	// Критерий Рейнольдса	[-]
	float Pr = mu_coolant_avg * Cp / coolant_lambda;	// Критерий Прандтля	[-]
	float Nu;	// Число Нуссельта	[-]
	if (Re < 100000) Nu = 0.69 * pow(Re, 0.5) * pow(Pr, 0.33);
	else Nu = 0.037 * pow(Re, 0.8) * pow(Pr, 0.43);
	float coolant_alpha = Nu * coolant_lambda / d_g_assembly;	// Коэф. теплоотдачи теплоносителя	[Вт/м^2*К]
	q_v = N_t / (M_PI * pow(outer_radius_fuel_tablet + delta_He + delta_shell , 2) * height);

	//Вычисляются разности температур. Они зависят только от z.
	//Температурная разность между максимальной температурой и температурой на внешнем радиусе топливной таблетки	[К]
	float temp_between_max_and_outer_radius_fuel_tablet = q_v * outer_radius_fuel_tablet * outer_radius_fuel_tablet
		/ 4 / fuel_lambda * K_r_max * K_z;
	//Температурная разность между температурой на внешнем радиусе топливной таблетки и температурой на внутренней стороне оболочки ТВЭЛа	[К]
	float temp_between_outer_radius_fuel_tablet_and_inner_shell = delta_He * N_t / He_lambda / ((outer_radius_fuel_tablet) * 2)
		/ M_PI * K_z * K_r_max;
	//Температурная разность между температурой на внутренней стороне и температурой на внешней стороне оболочки ТВЭЛа	[К]
	float temp_between_inner_and_outer_shell = delta_shell * N_t / shell_lambda
		/ ((outer_radius_fuel_tablet + delta_He) * 2) / M_PI  * K_z * K_r_max;
	//Температурная разность между температурой на внешней стороне оболочки ТВЭЛа и температурой теплоносителя	[К]
	float temp_between_outer_shell_and_coolant = N_t  * K_z * K_r_max / coolant_alpha / M_PI
		/ 2 / (outer_radius_fuel_tablet + delta_He + delta_shell);
	//Вычисляются значения температур
	float temp_outer_shell_K = coolant_temp_avg_K + temp_between_outer_shell_and_coolant;	// Температура на внешней стороне оболочки ТВЭЛа	[К]
	float temp_inner_shell_K = temp_outer_shell_K + temp_between_inner_and_outer_shell;	// Температура на внутренней стороне оболочки ТВЭЛа	[К]
	float temp_outer_radius_fuel_tablet_K = temp_inner_shell_K + temp_between_outer_radius_fuel_tablet_and_inner_shell;	// Температура на внешнем радиусе топливной таблетки	[К]
	float temp_max_K = temp_outer_radius_fuel_tablet_K + temp_between_max_and_outer_radius_fuel_tablet;	// Температура на внутреннем радиусе топливной таблетки	[К]

	// Лямбда-функция для добавления в вектор v_of_temp_K
	auto push = [&](float r, float temp) {
		std::pair <float, float> var(r * 1000, temp);
		v_of_temp_K.push_back(var);
		};
	// Добавляются вычисленные граничные значения температур в вектор
	push(0, temp_max_K);
	push(outer_radius_fuel_tablet, temp_outer_radius_fuel_tablet_K);
	push(outer_radius_fuel_tablet + delta_He, temp_inner_shell_K);
	push(outer_radius_fuel_tablet + delta_He + delta_shell, temp_outer_shell_K);
	push(outer_radius_fuel_tablet + delta_He + delta_shell + delta_t, coolant_temp_avg_K);
	push(fuel_el_step / 2, coolant_temp_avg_K);

	//Температурное распределение оболочки
	float pred = outer_radius_fuel_tablet + delta_He + delta_shell;	// Радиус внешней стороны оболочки ТВЭЛа	[м]
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
// Геттер для v_of_temp_K
std::vector <std::pair <float, float>> TVEL::get_v() {
	return v_of_temp_K;
}
// Сортировка неупорядоченных значений вектора v_of_temp_K
void TVEL::sort() {
	std::stable_sort(v_of_temp_K.begin(), v_of_temp_K.end(),
		[&](std::pair <float, float> pair1, std::pair <float, float> pair2) {
			return (pair1.first < pair2.first);
		});
}