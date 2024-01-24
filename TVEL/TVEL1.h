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
	const double l = 0.0000001;	// ����� ���������� ������� ������� �������������	[�]
	const size_t n_assembly = 1;	// ����� ���	[-]
	const size_t n_fuel_el = 1;	// ����� ����	[-]
	std::vector <std::pair <float, float>> v_of_temp_K;	// ������ ������������� ���������� �� ������� � ���������
	float rho_coolant_avg;	// ������� ��������� �������������	[��/�^3]
	float mu_coolant_avg;	// ������������ �������� �������������	[��*�]
	float reactor_thermal_power;	// �������� �������� ��������	[���]
	long double N_t;	// �������� �������� ������ �����	[��] 
	long double consumption;	// ������ ����� ������� 1 ����	[��/�]   
	float enthalpy_in;	// ��������� �� �����	[��/��]
	float enthalpy_out;	// ��������� �� ������	[��/��]
	float density_avg;	// ������� ��������� �������������	[��/�^3]
	int x;

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
			else if (key == "rho_coolant_avg") rho_coolant_avg = value;
			else if (key == "mu_coolant_avg") mu_coolant_avg = value;
			else if (key == "reactor_thermal_power") reactor_thermal_power = value;
			else if (key == "enthalpy_in") enthalpy_in = value;
			else if (key == "enthalpy_out") enthalpy_out = value;
		}
		N_t = reactor_thermal_power * 1000000 / 312 / 163;
		consumption = N_t / (enthalpy_out - enthalpy_in);
	}
	// ���������� �������� ����������� �� ������� � ���������� ������ � v_of_temp_K
	void temperature_calculation();
	// ������ ��� v_of_temp_K
	std::vector <std::pair <float, float>> get_v();
	// ���������� ��������������� �������� ������� v_of_temp_K
	void sort();

	TVEL(const TVEL&) = delete;
	void operator= (const TVEL&) = delete;
};