#include "../include/server.h"
#include <unistd.h>

using namespace rso;

int main() {
  Server server;
  server.acceptClients();
  while(true) sleep(1000);
  return 0;
}