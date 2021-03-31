#include <iostream>
#include <thread>
#include <future>
#include <random>
#include <vector>
#include <cmath>
#include "Timer.hpp"

std::size_t MonteCarlo(std::size_t N)
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> dis(0.0, 1.0);

	std::size_t a = 0;
	for (auto i = 0; i < N; ++i)
		if (std::pow(dis(gen), 2) + std::pow(dis(gen), 2) <= 1)
			a++;
	return a;
}

double SingleMonteCarlo(std::size_t N)
{
	return 4.0 * MonteCarlo(N) / N;
}

double MultyMonteCarlo(std::size_t N)
{
	if (std::thread::hardware_concurrency() <= 1)
	{
		std::cout << "Need more threads" << std::endl;
		return -1.0;
	}


	std::vector <std::future <std::size_t>> futures;
	std::vector <std::thread> threads;

	std::size_t CountN = N / std::thread::hardware_concurrency();

	for (auto i = 0U; i < (std::thread::hardware_concurrency() - 1); i++)
	{
		std::packaged_task <std::size_t(std::size_t)> task(MonteCarlo);
		futures.push_back(task.get_future());
		threads.push_back(std::thread(std::move(task), CountN));
	}

	std::size_t Th_res = MonteCarlo(CountN);
	std::size_t res = 0;
	for (auto i = 0U; i < std::size(futures); i++)
	{
		futures[i].wait();
		res += futures[i].get();
	}
	res += Th_res;

	for (auto i = 0U; i < std::size(threads); i++)
		threads[i].join();

	return 4.0 * res / N;
}

int main()
{
	{
		Timer single("Single Thread");

		std::cout << SingleMonteCarlo(31415926) << std::endl;
	}

	{
		Timer multy("Multy Thread");

		std::cout << MultyMonteCarlo(31415926) << std::endl;
	}

	return 0;
}