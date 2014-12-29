#include <stdio.h>
#include <thread>
#include <mutex>
#include <condition_variable>

const int LOOP = 1000 * 1000;

void Produce(int* value, std::mutex* mutex, std::condition_variable* cond) {
  std::unique_lock<std::mutex> lock(*mutex);
  for (int i = 0; i < LOOP; i++) {
    if (*value >= 0) {
      cond->wait(lock);
    }
    *value = i;
    cond->notify_one();
  }
  // lock->unlock implicitly.
}

void Consume(int* value, std::mutex* mutex, std::condition_variable* cond) {
  std::unique_lock<std::mutex> lock(*mutex);
  while (true) {
    if (*value < 0) {
      cond->wait(lock);
    }
    int i = *value;
    *value = -1;
    cond->notify_one();
    if (i % (10 * 1000) == 0) {
      printf("i == %d\n", i);
    }
    if (i + 1 == LOOP) {
      break;
    }
  }
}

int main(int argc, char** argv) {
  int value = -1;
  std::mutex mutex;
  std::condition_variable cond;
  std::thread producer(Produce, &value, &mutex, &cond);
  std::thread consumer(Consume, &value, &mutex, &cond);
  producer.join();
  consumer.join();
  return 0;
}
