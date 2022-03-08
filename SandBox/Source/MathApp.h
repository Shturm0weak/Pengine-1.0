#pragma once

#include "Core/Core.h"
#include "Core/Application.h"

#include <vector>

class Task1 : public Pengine::Application
{
private:

	double a = 0.0, b = 0.0; // Пределы интегрирования.
	double h = 0.0; // Шаг.

	size_t n = 0; // Количество узлов.

	// Функция.
	std::function<double(double)> f;

	// Таблица значений x и y.
	std::vector<double> xi, yi;

	// Лагранж.
	double L(double x);

	// Ньютон.
	double P(double x);
public:

	Task1(double a, double b, size_t n, std::function<double(double)> f);

	virtual void OnUpdate() override;
};

class Task2 : public Pengine::Application
{
private:

	double a = 0.0, b = 0.0; // Пределы интегрирования.
	double h = 0.0; // Шаг.

	size_t n = 0; // Количество узлов.
	size_t m = 0; // Количество отрезков.

	// Функция
	std::function<double(double)> f;

	// Чебышев.
	double Ch();

	// Ньютон-Котес.
	double R();

	// X в формуле чебышева для a и b пределов.
	double X(size_t i, double a, double b);
public:

	Task2(double a, double b, size_t n, size_t m, std::function<double(double)> f);

	virtual void OnUpdate() override;
};