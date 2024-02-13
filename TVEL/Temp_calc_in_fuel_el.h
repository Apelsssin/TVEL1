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
	float fuel_el_step;	// ��� ���������� ������	[�]
	float delta_shell;	// ������� �������� �����	[�]
	float delta_He;	// ������� �������� ������	[�]
	float outer_radius_fuel_tablet;	// �������� ������ ��������� ��������	[�]
	float inner_radius_fuel_tablet;	// ���������� ������ ��������� ��������	[�]
	float coolant_temp_in_K;	// ����������� ������������� �� �����	[K]
	float coolant_temp_out_K;	// ����������� ������������� �� ������	[K]
	float q_v;	// �������� ��������������� �������	[��/�3]
	float r_step = 0.0001;	// ��� ���������� ������� �� �������	[�]
	float Cp;	// �������� ������������ �������������	[��/��*�]
	float coolant_lambda;	// ����. ���������������� �������������	[��/�*�]
	float fuel_lambda;	// ����. ���������������� ��������� ��������	[��/�*�]
	float He_lambda;	// ����. ���������������� �������� ������	[��/�*�]
	float shell_lambda;	// ����. ���������������� �������� �����	[��/�*�]
	float K_r_max;	// ����. ����. ��������������� �� �������	[-]
	float K_z;	// ����. ��������������� �� ������	[-]
	const size_t n_assembly = 163;	// ����� ���	[-]
	const size_t n_fuel_el = 312;	// ����� ����	[-]
	std::vector <std::pair <float, float>> v_of_temp_K;	// ������ ������������� ���������� �� ������� � ���������
	float rho_coolant_avg;	// ������� ��������� �������������	[��/�^3]
	float mu_coolant_avg;	// ������������ �������� �������������	[��*�]
	float reactor_thermal_power;	// �������� �������� ��������	[���]
	float N_t;	// �������� �������� ������ �����	[��] 
	float consumption;	// ������ ����� ������� 1 ����	[��/�]   
	float enthalpy_in;	// ��������� �� �����	[��/��]
	float enthalpy_out;	// ��������� �� ������	[��/��]
	float height;	// ������ ������������ �����	[�]

public:
	TVEL() {
		// ����������� ������� ������ �� ����� consts.json
		std::ifstream file;
		file.open(R"(consts.json)", std::ios::in);
		if (!file.is_open()) throw Exceptions("������� ������� �������������� ����.");
		json Doc{ json::parse(file) };
		for (auto& [key, value] : Doc.items()) {
			if (value <= 0) throw Exceptions("�������� ������� ������ < ��� = 0.");
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
			else throw Exceptions("������������ ������� ������.");
		}
		// ����������� ������ �������� �������� � ������� ��� ������ ����� 
		N_t = reactor_thermal_power * 1000000 / n_assembly / n_fuel_el;
		consumption = N_t / (enthalpy_out - enthalpy_in);
	}
	// ���������� �������� ����������� �� ������� � ���������� ������ � v_of_temp_K
	void temperature_calculation();
	// ������ ��� v_of_temp_K
	std::vector <std::pair <float, float>> get_v();
	// ���������� ��������������� �������� ������� v_of_temp_K
	void sort();
	// ������ �� �������� ����� ������� ������
	TVEL(const TVEL&) = delete;
	void operator= (const TVEL&) = delete;
};