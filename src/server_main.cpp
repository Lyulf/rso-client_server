#include "../include/server.h"

using namespace rso;

int main() {
  Server server;
  server.acceptClients();
  return 0;
}