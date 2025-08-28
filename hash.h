#include <vector>
#include <functional>
#include <boost/multiprecision/miller_rabin.hpp> //Prime testing

//INHERITANCE
//Base class that provides an interface the two subclasses will inherit
//This is an abstract (pure virtual) class that does nothing
//But it allows us to avoid copy/pasting code in main because the interface is the same for both hash tables
template <class T, size_t SIZE>
class Hash {
    public:
        virtual ~Hash() {}
        virtual void insert(T new_data) = 0;
        virtual void remove(T old_data) = 0;
        virtual bool search(T test_data) const = 0;
        virtual void change(T old_data, T new_data) = 0;
};

//YOU: Write three hash tables, one for linear probing, one for chaining, one using any other method you want (Quadratic, Double Hashing, or using the STL unordered_set class are all ok)

template <class T, size_t SIZE>
class LinearProbingHash final : public Hash<T,SIZE> { //This class cannot be inherited from
    //This bit of magic makes three constants: STATUS::OPEN, STATUS::FILLED, and STATUS::DELETED
    //OPEN means the bucket has never held a value in it
    //FILLED means it currently holds a valid value in it
    //DELETED means the value in it has been deleted and not replaced yet
    enum class STATUS : char {OPEN, FILLED, DELETED};
    std::vector<T> data;
    std::vector<STATUS> status; //Each element can be STATUS::OPEN, STATUS::FILLED or STATUS::DELETED
    public:
    LinearProbingHash() {
        data.resize(SIZE); //Set the vector to be SIZE big - SIZE is a template parameter
        status.resize(SIZE,STATUS::OPEN); //Set the state of every bucket to STATUS::OPEN
    }
    //RTTI Run Time Type Information - vtable
    //Final turns this off if you get a LinearProbingHash& in a function parameter, it is devirtualized
    void insert(T new_data) final override {
        //Step 1 - Compute the initial bucket to try
        size_t address = std::hash<T>{}(new_data) % SIZE;
        //Step 2 - We will start at the bucket at "address" and then move to the right,
        //  looping around to the start if necessary, to find first available bucket
        //  Either an OPEN bucket or a DELETED bucket will do.
        //TODO: Make this not infinite loop on a full hash table
        while (status.at(address) == STATUS::FILLED)
            address = (address+1)%SIZE; //Move one bucket to the right, looping around
        //Ok, we've found an open spot, write the data in and mark it as filled
        data.at(address) = new_data;
        status.at(address) = STATUS::FILLED;
    }
    //YOU:
    void remove(T old_data) override {
    }
    //YOU:
    bool search(T old_data) const override {
        return false;
    }
    //YOU:
    void change(T old_data, T new_data) override {
        //Remember, only change it if it actually is in the hash table
    }
};

//YOU: Do the same as the above, with with hashing with chaining instead of linear probing
//You can do chaining either with LLs or with BSTs
template <class T, size_t SIZE>
class ChainingHash : public Hash<T,SIZE> {
    //YOU:
    public:
    //YOU:
    ChainingHash() {
    }
    //YOU:
    void insert(T new_data) override { //Override means it is overriding a virtual function
    }
    //YOU:
    void remove(T old_data) override {
    }
    //YOU:
    bool search(T test_data) const override {
        return false;
    }
    //YOU:
    void change(T old_data, T new_data) override {
    }
};

//YOU: Write a third hashing class here, following the pattern above. You can do it using double hashing, quadratic hashing, or the unordered_set STL class, whatever you like.

template <class T, size_t SIZE>
class DoubleHash : public Hash<T, SIZE> {
    private:
        enum class STATUS : char {OPEN, FILLED, DELETED};
        std::vector<T> data;
        std::vector<STATUS> status; //Each element can be STATUS::OPEN, STATUS::FILLED or STATUS::DELETED
        int R = 2;
    public:
        DoubleHash() {
            /*
            if (!boost::multiprecision::miller_rabin_test(SIZE, 50)) {
                std::cerr << "Size is not prime! Raaaa!" << endl;
            }
            */

            data.resize(SIZE); //Set the vector to be SIZE big - SIZE is a template parameter
            status.resize(SIZE,STATUS::OPEN); //Set the state of every bucket to STATUS::OPEN
            for (int i = SIZE - 1; i > 2; i--) {
                if (boost::multiprecision::miller_rabin_test(i, 50)) {
                    R = i;
                    break;
                }
            }
        }

     void insert(T new_data) override { //Override means it is overriding a virtual function
            int currIndex = new_data % SIZE;
            int step = (new_data % R) + 1;
            for (int i = 0; i < SIZE; i++) {
                if (status.at(currIndex) == STATUS::OPEN) {
                    data.at(currIndex) = new_data;
                    status.at(currIndex) = STATUS::FILLED;
                    return;
                }
                else if (status.at(currIndex) == STATUS::DELETED) {
                    data.at(currIndex) = new_data;
                    status.at(currIndex) = STATUS::FILLED;
                    for (int i = 0; i < SIZE - 1; i++) {
                        currIndex = (currIndex + step) % SIZE;
                        if (status.at(currIndex) == STATUS::OPEN) {
                            return;
                        }
                        if (status.at(currIndex) == STATUS::FILLED && data.at(currIndex) == new_data) {
                            status.at(currIndex) = STATUS::DELETED;
                            return;
                        }
                    }
                    return;
                }
                else if (data.at(currIndex) == new_data) {
                    return;
                }
                else {
                    currIndex = (currIndex + step) % SIZE;
                }
            }
        }

        void remove(T old_data) override {
            int currIndex = old_data % SIZE;
            int step = (old_data % R) + 1;
            for (int i = 0; i < SIZE; i++) {
                if (status.at(currIndex) == STATUS::OPEN) {
                    return;
                }
                if (status.at(currIndex) == STATUS::FILLED && data.at(currIndex) == old_data) {
                    status.at(currIndex) = STATUS::DELETED;
                    return;
                }
                currIndex = (currIndex + step) % SIZE;
            }
        }

        bool search(T test_data) const override {
            int currIndex = test_data % SIZE;
            int step = (test_data % R) + 1;
            for (int i = 0; i < SIZE; i++) {
                if (status.at(currIndex) == STATUS::OPEN) {
                    return false;
                }
                if (status.at(currIndex) == STATUS::FILLED && data.at(currIndex) == test_data) {
                    return true;
                }
                currIndex = (currIndex + step) % SIZE;
            }
            return false;
        }

        void change(T old_data, T new_data) override {
            if (search(old_data)) {
                remove(old_data);
                insert(new_data);
            }
        }
};

template <class T, size_t SIZE>
class QuadraticProbingHash : public Hash<T,SIZE> {
    enum class STATUS : char {OPEN, FILLED, DELETED};
    std::vector<T> data;
    std::vector<STATUS> status;
    public:
    QuadraticProbingHash(){
        data.resize(SIZE);
        status.resize(SIZE,STATUS::OPEN);
    }
    void insert(T new_data) override{
        size_t address = std::hash<T>{}(new_data) % SIZE;
        int i = 1;
        while (status.at(address) == STATUS::FILLED){
            address = (address + i*i) % SIZE;
            i++;
            if(i > SIZE) return;
        }
        data.at(address) = new_data;
        status.at(address) = STATUS::FILLED;
    }
    void remove(T old_data) override{
        size_t address = std::hash<T>{}(old_data) % SIZE;
        int i = 1;
        while(status.at(address) != STATUS::OPEN){
            if(status.at(address) == STATUS::FILLED && data.at(address) == old_data){
                status.at(address) = STATUS::DELETED;
                return;
            }
            address = (address + i*i) % SIZE;
            i++;
            if(i > SIZE) return;
        }
    }
    bool search(T old_data) const override{
        size_t address = std::hash<T>{}(old_data) % SIZE;
        int i = 1;
        while (status.at(address) != STATUS::OPEN){
            if (status.at(address) == STATUS::FILLED && data.at(address) == old_data){
                return true;
            }
            address = (address + i*i) % SIZE;
            i++;
            if(i > SIZE) return false;
        }
        return false;
    }
    void change(T old_data, T new_data) override{
        if(search(old_data)){
            remove(old_data);
            insert(new_data);
        }
    }
};

