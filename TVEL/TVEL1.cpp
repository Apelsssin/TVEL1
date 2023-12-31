#include "TVEL1.h"
#include <iostream>
#include <vector>
#include <utility>
#include <algorithm>
#include <cmath>
#include "gnuplot-iostream.h"
#include <nlohmann/json.hpp>
#include <fstream>
using json = nlohmann::json;
void TVEL::calculation() {
	double S = sqrt(3) / 2 * fuel_el_step * fuel_el_step - pi * (outer_radius_fuel_tablet + delta_He + delta_shell)
		* (outer_radius_fuel_tablet + delta_He + delta_shell); // ������� ������������� 1 ���� [�^2]
	double P = 2 * pi * (outer_radius_fuel_tablet + delta_He + delta_shell);// ����������� �������� 1 ���� [�]
	double coolant_velocity = Q_1 / S; //�������� ������������� [�/�]
	long double Pe = rho_coolant_avg * Cp * coolant_velocity * l / coolant_lambda; //����� �����
	double delta_t = sqrt(1 / Pe) * l; // ������� ��������� ������������ ���� ������������� [�]
	float coolant_temp_avg_K = (coolant_temp_in_K + coolant_temp_out_K) / 2; //������� ����������� ������������� [�]
	float nu_coolant_avg = mu_coolant_avg / rho_coolant_avg;//�������������� �������� [�^2/c]
	double d_g_assembly = 4 * S / P;//�������������� ������� [�]
	double coolant_alpha = nu_coolant_avg * coolant_lambda / d_g_assembly;//[��/�^2*�]

	//����������� �������� ����������. ��� ������� ������ �� z.
	float temp_between_max_and_outer_radius_fuel_tablet = q_v * outer_radius_fuel_tablet * outer_radius_fuel_tablet / 4 / fuel_lambda * K_r_max * K_z;
	float temp_between_outer_radius_fuel_tablet_and_inner_shell = delta_He * N_t / He_lambda * n_assembly / ((outer_radius_fuel_tablet) * 2) / pi / n_fuel_el * K_z * K_r_max;
	float temp_between_inner_and_outer_shell = delta_shell * N_t * n_assembly / shell_lambda / ((outer_radius_fuel_tablet + delta_He) * 2) / pi / n_fuel_el * K_z * K_r_max;
	float temp_between_outer_shell_and_coolant = N_t * n_assembly * K_z * K_r_max / coolant_alpha / pi / 2 / (outer_radius_fuel_tablet + delta_He + delta_shell);
	//����������� �������� ����������
	float temp_outer_shell_K = coolant_temp_avg_K + temp_between_outer_shell_and_coolant; // [�]
	float temp_inner_shell_K = temp_outer_shell_K + temp_between_inner_and_outer_shell; // [�]
	float temp_outer_radius_fuel_tablet_K = temp_inner_shell_K + temp_between_outer_radius_fuel_tablet_and_inner_shell;//  [�]
	float temp_max_K = temp_outer_radius_fuel_tablet_K + temp_between_max_and_outer_radius_fuel_tablet;// [�]

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

	//������������� ������������� ��������
	float pred = outer_radius_fuel_tablet + delta_He + delta_shell;
	for (float i = outer_radius_fuel_tablet + delta_He + r_step; i < pred; i += r_step) {
		float temp_var = temp_inner_shell_K - (temp_between_inner_and_outer_shell) / log(1 + delta_shell
			/ (outer_radius_fuel_tablet + delta_He)) * log(i / (outer_radius_fuel_tablet + delta_He));
		push(i, temp_var);
	}
	//������������� ������������� ��������
	for (float i = 0; i < outer_radius_fuel_tablet; i += r_step) {
		float temp_var = temp_outer_radius_fuel_tablet_K + q_v / 4 / fuel_lambda
			* (outer_radius_fuel_tablet * outer_radius_fuel_tablet - i * i);
		push(i, temp_var);
	}
}

std::vector <std::pair <float, float>> TVEL::get_v() {
	return v_of_temp_K_r;
}

void TVEL::sort() { //���������� �� �������� ������� �� �����������
	std::stable_sort(v_of_temp_K_r.begin(), v_of_temp_K_r.end(),
		[&](std::pair <float, float> pair1, std::pair <float, float> pair2) {
			return (pair1.first < pair2.first);
		});
}