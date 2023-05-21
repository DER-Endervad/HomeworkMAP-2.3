#include <iostream>
#include <Windows.h>
#include <mutex>

class Date {
private:
    int year;
    int month;
    int day;
    std::atomic_flag flag = ATOMIC_FLAG_INIT;
    std::mutex mut;
public:
    Date(int day, int month, int year) {    
        this->day = day;
        this->month = month;
        this->year = year;
    }
    int get_day() const { return day; }
    int get_month() const { return month; }
    int get_year() const { return year; }

    void lock() { while (flag.test_and_set()); }
    void unlock() { flag.clear(); }

    void swap (Date& one, Date& two) {
        int swap = 0;

        swap = one.day;
        one.day = two.day;
        two.day = swap;

        swap = one.month;
        one.month = two.month;
        two.month = swap;

        swap = one.year;
        one.year = two.year;
        two.year = swap;
    }

    void swap_L(Date& one, Date& two) {
        one.mut.lock(); two.mut.lock();
        swap(one, two);
        one.mut.unlock(); two.mut.unlock();
    }

    void swap_SL(Date& one, Date& two) {
        std::scoped_lock<std::mutex, std::mutex> lock(one.mut, two.mut);
        swap(one, two);
    }

    void swap_UL(Date& one, Date& two) {
        std::unique_lock lock{one.mut, std::defer_lock };
        std::unique_lock lock2{two.mut, std::defer_lock };
        std::lock(lock, lock2);
        swap(one, two);

    }
};
std::ostream& operator << (std::ostream& os, const Date& date) {
    return os << date.get_day() << "." << date.get_month() << "." << date.get_year();
}

int main()
{
    setlocale(LC_ALL, "Russian");
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    Date date_1(21, 5, 2023);
    Date date_2(22, 6, 2024);
    std::cout << "Первая дата: " << date_1 << "     Вторая дата: " << date_2 << std::endl;
    date_1.swap_L(date_1, date_2);
    std::cout << "1. После свапа" << std::endl;
    std::cout << "Первая дата: " << date_1 << "     Вторая дата: " << date_2 << std::endl;

    date_1.swap_SL(date_1, date_2);
    std::cout << "2. После свапа" << std::endl;
    std::cout << "Первая дата: " << date_1 << "     Вторая дата: " << date_2 << std::endl;

    date_1.swap_UL(date_1, date_2);
    std::cout << "3. После свапа" << std::endl;
    std::cout << "Первая дата: " << date_1 << "     Вторая дата: " << date_2 << std::endl;
}