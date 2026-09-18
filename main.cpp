#include <iostream>
#include <pthread.h>
#include <random>

struct Californication {
  double r_;
  size_t tests_;
  size_t seed_;
};

bool isInside(double x, double y, double r) { return x * x + y * y <= r * r; }

void* calc(void* input /* double r, size_t tests, size_t seed */)
{
  Californication data = *static_cast<Californication*>(input);
  std::mt19937 gen(data.seed_);
  std::uniform_real_distribution<> distribution(-data.r_, data.r_);
  size_t successful = 0;
  for (size_t i = 0; i < data.tests_; ++i) {
    successful += isInside(distribution(gen), distribution(gen), data.r_);
  }
  return reinterpret_cast<void*>(successful);
}

double getArea(double r, size_t threads, size_t tests)
{
  size_t tests_per_thread = tests / threads;
  std::vector<pthread_t> descriptors(threads);
  std::vector<Californication> data(threads);
  size_t created = 0;
  auto DRY = [=, &descriptors, &data, &created](size_t i, size_t tests_count) {
    data[i] = {r, tests_count, i};
    int err = pthread_create(&descriptors[i], nullptr, calc, &data[i]);
    if (err) {
      std::cerr << strerror(err) << '\n';
      return 0;
    }
    ++created;
    return 1;
  };
  bool failed = false;
  for (size_t i = 0; i < threads - 1 && !failed; ++i) {
    failed = !DRY(i, tests_per_thread);
  }
  if (!failed) {
    failed = !DRY(threads - 1, tests % threads + tests_per_thread);
  }
  double successful = 0;
  for (size_t i = 0; i < created; ++i) {
    size_t* code = 0;
    int err = pthread_join(descriptors[i], reinterpret_cast<void**>(&code));
    if (err) {
      std::cerr << strerror(err) << '\n';
      failed = true;
      continue;
    }
    successful += reinterpret_cast<size_t>(code);
  }
  if (failed) {
    return -1.0;
  }
  return successful / tests * r * r * 4;
}

int main(int argc, char** argv)
{
  if (argc != 4) {
    std::cerr << "Incorrect arguments\n";
    return 1;
  }
  char* end;
  double radius = strtod(argv[1], &end);
  char* yaend;
  size_t tests = strtoull(argv[2], &yaend, 10);
  char* yaaend;
  unsigned threads = strtoul(argv[3], &yaaend, 10);
  if (*end != '\0' || *yaend != '\0' || *yaaend != '\0' || threads == 0) {
    std::cerr << "Incorrect arguments\n";
    return 1;
  }
  double area = getArea(radius, threads, tests);
  if (area < 0) {
    std::cerr << "Internal error\n";
    return 1;
  }
  std::cout << area << '\n';
}
