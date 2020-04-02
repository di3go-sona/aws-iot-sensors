#include <string.h>
#include <stdio.h>
#include <random.h>
#include <xtimer.h>


# define INTERVAL 1 

int get_humidity(void)
{
  int upper = 100;
  int lower = 0;
  int r_num = (int) random_uint32();
  int humidity = (r_num % (upper - lower + 1)) + lower;

  return humidity;
}

int get_temperature(void)
{
  int upper = 50;
  int lower = -50;
  int r_num = (int) random_uint32();
  int temperature = (r_num % (upper - lower + 1)) + lower;

  return temperature;
}


int main(void)
{
    int temperature;
    int humidity;

    while(1){
      temperature = get_temperature();
      humidity = get_humidity();
      printf("%d, %d \n", temperature, humidity);
      xtimer_sleep((uint32_t) INTERVAL);
    }

    return 0;
}