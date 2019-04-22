#include "../include/defaults.h"
#include "../include/server.h"

using namespace rso;

int main() {
  Server server(defaults::port, 1);
  server.acceptClients();
  return 0;
}