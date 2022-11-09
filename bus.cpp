#include "bus.h"

BusTransaction Bus::next_transc() {
  BusTransaction transc = requests.front();
  requests.pop();
  curr_transc = transc;
  return transc;
}
