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
  int humidity = ((r_num*1000) % (upper - lower + 1)) + lower;

  return humidity;
}

int get_temperature(void)
{
  int upper = 50;
  int lower = -50;
  int r_num = (int) random_uint32();
  int temperature = ((r_num*1000) % (upper - lower + 1)) + lower;

  return temperature;
}

int get_rain(void)
{
  int upper = 50;
  int lower = 0;
  int r_num = (int) random_uint32();
  int temperature = ((r_num*1000) % (upper - lower + 1)) + lower;

  return temperature;
}

int get_wind(void)
{
  int upper = 100;
  int lower = 0;
  int r_num = (int) random_uint32();
  int temperature = ((r_num*1000) % (upper - lower + 1)) + lower;

  return temperature;
}

int get_wind_dir(void)
{
  int upper = 360;
  int lower = 0;
  int r_num = (int) random_uint32();
  int temperature = ((r_num*1000) % (upper - lower + 1)) + lower;

  return temperature;
}



int main(void)
{
    int temperature, humidity, rain, wind, wind_dir;

    while(1){
      temperature = get_temperature();
      humidity = get_humidity();
      rain = get_rain();
      wind = get_wind();
      wind_dir = get_wind_dir();
      printf("%d, %d, %d, %d, %d \n", temperature, humidity);
      xtimer_sleep((uint32_t) INTERVAL);
    }

    return 0;
}
