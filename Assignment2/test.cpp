#include <iostream>
#include <signal.h>

using namespace std;

// void ctrl_c_handler(int signum) {
//   cout << endl;
//   signal(SIGINT, SIG_IGN);
//   cout
// }

int main() {
  // signal(SIGINT, ctrl_c_handler);
  int n;
  while (1) {
    sleep(1);
    cout << "$\n";
    // string str;
    // cin>>str;
  }

  return 0;
}
