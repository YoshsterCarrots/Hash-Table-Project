#include "/public/read.h"
#include "hash.h"
#include <memory>
#include <unistd.h>
using namespace std;


int main(int argc, char **argv) {
    const size_t SIZE = 11; //Change to 1M for benchmarking, back to 16 to pass cases
    //Note the slight difference from the enum class in hash.h
    enum MODE { LINEAR, CHAINING, DOUBLE, QUADRATIC }; //Add quadratic maybe
    int mode = LINEAR;
    if (argc > 1) mode = atoi(argv[1]); //Read mode from command line parameters
    //If no command line parameter (a.out 0 or a.out 1) is provided, then read from keyboard
    else mode = read("Choose mode:\n0) Linear\n1) Chaining\n2) Double\n3) Quadratic");
    if (mode < LINEAR or mode > QUADRATIC) mode = LINEAR;
    cerr << "Mode == " << mode << endl;

    //A unique pointer is just a pointer that will delete itself. Nice!
    unique_ptr<Hash<int, SIZE>> table = nullptr; //Hash is an abstract class, but by C++ magic we can make it implement one of our three options
    if (mode == LINEAR) table = make_unique<LinearProbingHash<int, SIZE>>();
    else if (mode == CHAINING) table = make_unique<ChainingHash<int, SIZE>>();
    else if (mode == QUADRATIC) table = make_unique<QuadraticProbingHash<int, SIZE>>();
    //YOU: Add double hashing here, something like this:
    //else table = new DoubleHash<int,SIZE>;

    while (cin) {
        string s;
        cin >> s;
        if (!cin) break;
        if (s == "CHANGE") {
            int x = 0, y = 0;
            cin >> x >> y;
            if (!cin) break;
            table->change(x, y);
        } else {
            int x = 0;
            cin >> x;
            if (!cin) break;
            if (s == "INSERT") table->insert(x);
            else if (s == "SEARCH") {
                if (table->search(x))
                    cout << x << " IN TABLE\n";
                else
                    cout << x << " NOT IN TABLE\n";
            } else if (s == "REMOVE") table->remove(x);
            else break;
        }
    }

    //delete table; //Don't need this for a unique_ptr!
    //We could do reset() on it, but we don't need to since it's about to go away...
}
