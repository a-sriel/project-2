#include <thread>

class Customer {
    private:
        std::thread::id customer_id;
        int wait_time;
        int teller_number;
    public:
        int getWaitTime() const;
}