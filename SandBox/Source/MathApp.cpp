#include "MathApp.h"

#include "Core/Visualizer.h"

double Task1::L(double x)
{
	double y = 0.0;
	for (size_t i = 0; i < n; i++)
	{
		double newY = 1.0;
		for (size_t j = 0; j < n; j++)
		{
			if (i != j)
			{
				newY *= ((x - xi[j]) / (xi[i] - xi[j]));
			}
		}
		y += yi[i] * newY;
	}

	return y;
}

double Task1::P(double x)
{
	std::vector<std::vector<double>> dy; // Матрица конечных разностей.
	const size_t n = xi.size() - 1;

	// Заполнение матрицы конечных разностей нулями.
	dy.resize(n);
	for (size_t i = 0; i < n; i++)
	{
		dy[i].resize(n);
		for (size_t j = 0; j < n; j++)
		{
			dy[i][j] = 0;
		}
	}

	// Вычисление конечных разностей первого рода.
	for (size_t i = 0; i < n; i++)
	{
		dy[i][0] = yi[i + 1] - yi[i];
	}
	
	// Вычисление конечных разностей второго рода.
	for (size_t i = 1; i < n; i++)
	{
		for (size_t j = 0; j < n - i; j++)
		{
			dy[j][i] = dy[j + 1][i - 1] - dy[j][i - 1];
		}
	}

	// Вычисление значения.
	double mult = 1.0;
	double p = yi[0];
	double q = (x - xi[0]) / h;
	double fact = 1.0;
	for (size_t i = 0; i < n; i++)
	{
		mult *= (q - i);
		fact *= i + 1;
		p += mult * dy[0][i] / (fact);
	}

	return p;
}

Task1::Task1(double a, double b, size_t n, std::function<double(double)> f)
{
	this->a = a;
	this->b = b;
	this->f = f;
	this->h = (b - a) / (double)n;
	
	n++;
	this->n = n;

	xi.resize(n);
	yi.resize(n);
	for (size_t i = 0; i < n; i++)
	{
		xi[i] = a + i * h;
		yi[i] = f(xi[i]);
	}
}

void Task1::OnUpdate()
{
	//for (double i = a; i + 10e-6 < b; i += 0.1)
	//{
	//	Pengine::Visualizer::DrawLine({ i, f(i), 0.0f }, { i + 0.1f, f(i + 0.1f), 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f });
	//}

	for (double i = a; i + 10e-6 < b; i += h)
	{
		//Pengine::Visualizer::DrawLine({ i, L(i), 0.0f }, { i + h, L(i + h), 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f });
		Pengine::Visualizer::DrawLine({ i, P(i), 0.0f }, { i + h, P(i + h), 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f });
	}

	for (double i = 0.5; i < b + 0.5; i += 0.5)
	{
		Pengine::Visualizer::DrawLine({ 0.0f, i, 0.0f }, { b, i, 0.0f }, { 0.3f, 0.3f, 0.3f, 1.0f });
		Pengine::Visualizer::DrawLine({ i, 0.0f, 0.0f }, { i, b, 0.0f }, { 0.3f, 0.3f, 0.3f, 1.0f });
	}
	
	//for (double i = a; i + 10e-6 < b; i += h * 0.25)
	//{
	//	Pengine::Visualizer::DrawQuad(glm::translate(glm::mat4(1.0f), glm::vec3(i, P(i), 0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(0.05f)),
	//		{ 0.0f, 1.0f, 0.0f, 1.0f });
	//}

	for (double i = a; i + 10e-6 < b; i += 0.1)
	{
		Pengine::Visualizer::DrawQuad(glm::translate(glm::mat4(1.0f), glm::vec3(i, f(i), 0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(0.05f)),
		{ 0.0f, 1.0f, 0.0f, 1.0f });
	}
	
	Pengine::Visualizer::DrawQuad(glm::translate(glm::mat4(1.0f), glm::vec3(b, f(b), 0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(0.05f)),
		{ 0.0f, 1.0f, 0.0f, 1.0f });

	Pengine::Visualizer::DrawLine({ 0.0f, 0.0f, 0.0f }, { b, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f });
	Pengine::Visualizer::DrawLine({ 0.0f, 0.0f, 0.0f }, { 0.0f, b, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f });
}

double Task2::Ch()
{
	// Пределы интегрирования для отдельных отрезков.
	double newA = a;
	double newB = a + h;

	double ch = 0.0;
	for (size_t j = 0; j < n; j++)
	{
		double newCh = 0.0;
		for (size_t i = 0; i < m; i++)
		{
			newCh += f(X(i, newA, newB));
		}
		newB += h;
		newA += h;
		newCh *= (newB - newA) / (double)m;
		ch += newCh;
	}
	
	printf_s("%.9lf\n", ch);
	return ch;
}

double Task2::X(size_t i, double a, double b)
{
	// Веса.
	const double w[3] = { 0.707107, 0, -0.707107 };

	return ((b + a) / 2.0) + ((b - a) / 2.0) * w[i];
}

double Task2::R()
{
	// Узлы.
	double E[3] = { -0.774597, 0, 0.774597 };
	
	// Веса.
	double W[3] = { 5.0/9.0, 8.0/9.0, 5.0/9.0 };

	// Пределы интегрирования для отдельных отрезков.
	double newA = a;
	double newB = a + h;

	double r = 0.0;
	for (size_t j = 0; j < n; j++)
	{
		double newR = 0.0;
		for (size_t i = 0; i < m; i++)
		{
			newR += W[i] * f(((newA + newB) / 2.0) + ((newB - newA) / 2.0) * E[i]);
		}
		newB += h;
		newA += h;
		newR *= (newB - newA) / 2.0;
		r += newR;
	}
	printf_s("%.9lf", r);
	return r;
}

Task2::Task2(double a, double b, size_t n, size_t m, std::function<double(double)> f)
{
	this->a = a;
	this->b = b;
	this->n = n;
	this->m = m;
	this->f = f;
	h = (b - a) / (double)n;

	Ch();
	R();
}

void Task2::OnUpdate()
{
}
