#include <iostream>
#include <vector>
#include <chrono>
#include <condition_variable>
#include <ostream>
#include <thread>
#include <functional>
#include <mutex>
#include <atomic>

using namespace std::chrono_literals;
using namespace std;

function<bool(int)> is_3x = [](int x){return x%3 == 0;};
function<bool(int)> is_5x = [](int x){return x%5 == 0;};
function<bool(int)> is_15x = [](int x){return x%15 == 0;};
function<bool(int)> is_simple = [](int x){return !(is_3x(x) || is_5x(x));};

function<void()> printFizz = []{cout << "Fizz" << flush;};
function<void()> printBuzz = []{cout << "Buzz" << flush;};
function<void()> printFizzBuzz = []{cout << "FizzBuzz" << flush;};
function<void(int)> printNumber = [](int x){cout << x << flush;};

class FizzBuzz {
    private:
        int n;
        int current = 1;
        mutex protecter;
        condition_variable cv;

    public:
        FizzBuzz(int n) {
            this->n = n;
        }

        // printFizz() outputs "fizz".
        void fizz(function<void()> printFizz) {
            bool go_on = true;
            while(go_on){
                unique_lock<mutex> locker(protecter);
                cv.wait(locker, [this]{return !is_15x(current) && is_3x(current);});
                printFizz();
                ++current;
                if(current > n) go_on = false;
                cv.notify_all();
            }
            cout << endl << this_thread::get_id() << endl;
        }

        // printBuzz() outputs "buzz".
        void buzz(function<void()> printBuzz) {
            bool go_on = true;
            while(go_on){
                unique_lock<mutex> locker(protecter);
                cv.wait(locker, [this]{return !is_15x(current) && is_5x(current);});
                printBuzz();
                ++current;
                if(current > n) go_on = false;
                cv.notify_all();
            }
            cout << endl << this_thread::get_id() << endl;
        }

        // printFizzBuzz() outputs "fizzbuzz".
        void fizzbuzz(function<void()> printFizzBuzz) {
            bool go_on = true;
            while(go_on){
                unique_lock<mutex> locker(protecter);
                cv.wait(locker, [this]{return is_15x(current);});
                printFizzBuzz();
                ++current;
                if(current > n) go_on = false;
                cv.notify_all();
            }
            cout << endl << this_thread::get_id() << endl;
        }

        // printNumber(x) outputs "x", where x is an integer.
        void number(function<void(int)> printNumber) {
            bool go_on = true;
            while(go_on){
                unique_lock<mutex> locker(protecter);
                cv.wait(locker, [this]{return is_simple(current);});
                printNumber(current);
                ++current;
                if(current > n) go_on = false;
                cv.notify_all();
            }
            cout << endl << this_thread::get_id() << endl;
        }
};

int main(){
    FizzBuzz obj(23);
    vector<thread> threads;
    threads.push_back(move(thread(&FizzBuzz::fizz, &obj, printFizz)));
    threads.push_back(move(thread(&FizzBuzz::buzz, &obj, printBuzz)));
    threads.push_back(move(thread(&FizzBuzz::fizzbuzz, &obj, printFizzBuzz)));
    threads.push_back(move(thread(&FizzBuzz::number, &obj, printNumber)));
    //    thread(&FizzBuzz::fizz, &obj, printFizz).detach();
    //    thread(&FizzBuzz::buzz, &obj, printBuzz).detach();
    //    thread(&FizzBuzz::fizzbuzz, &obj, printFizzBuzz).detach();
    //    thread(&FizzBuzz::number, &obj, printNumber).detach();
    for(int i = 0; i < threads.size(); ++i)
        if(threads[i].joinable()) threads[i].join();
    //    while(obj.go_on.load());
    //    this_thread::sleep_for(1ms);
    return 0;
}
