#include <fstream>
#include <iostream>
#include <string_view>
#include <string>
#include <cstdio>
#include <list>
#include <iomanip>
#include <numeric>
#include <algorithm>
#include "Timer.h"
#include "Table5d.h"

using namespace std;
using Collection = array<int, 5>;

constexpr string_view INPUT_FILE    = "input.txt";
constexpr string_view OUTPUT_FILE   = "result.txt";
constexpr int RED       = 0;
constexpr int BLUE      = 1;
constexpr int YELLOW    = 2;
constexpr int PURPLE    = 3;
constexpr int WHITE     = 4;

constexpr int DAYS_LIMIT = 200;

Collection required = {};
Table5d<uint16_t> table;

Timer timer;

// 高速化
bool ignoreBlue = false;

int64_t count1 = 0, count2 = 0;

ostream& operator<<(ostream& os, const Collection& collection){
    os << "Collection(";
    for(int i=0;i<5;i++){
        os << setw(2) << collection[i];
        if(i!=4) os << ", ";
    }
    os << ")";
    return os;
}

constexpr int howManyBought(const int num, const int day, const int color){
    return day / 5 + (color == PURPLE ? day / 2 : 0) + (color == WHITE ? day : 0) + num;
}

constexpr int howManyBought(const Collection& collection, const int day, const int color){
    return howManyBought(collection[color], day, color);
}

constexpr int priceOf(const int num, const int day, const int color){
    return 1 + howManyBought(num, day, color) / 25;
}
constexpr int priceOf(const Collection& collection, const int day, const int color){
    return priceOf(collection[color], day, color);
}

class Status{
    public:
    int day = 1;
    int money = 5;
    Collection collection = Collection({0, 0, 0, 0, 0});
    list<int> history;
    Status(const int day, const int money, const Collection& collection = Collection({0, 0, 0, 0, 0})){
        this->day = day;
        this->money = money;
        this->collection = collection;
    }
    Status(){}
    constexpr bool isGoal() const{
        return collectionEquals(required);
    }
    /*const inline Status bought(const int color) const{
        Status ret = Status(day, money - priceOf(collection, day, color), collection);
        ret.collection[color]++;
        return ret;
    }*/
    constexpr bool operator==(const Status& status) const{
        return day == status.day && money == status.money && collectionEquals(status.collection);
    }
    constexpr int getCost() const{
        return day * 5 - money;
    }
    friend inline ostream& operator<<(ostream& os, const Status& status){
        os
        << "Status(day : " << status.day 
        << ", money : " << status.money 
        << ", collection : " << status.collection
        << ", cost : " << status.getCost() << ")";
        return os;
    }
    private:
    constexpr bool collectionEquals(const Collection& collection) const{
        for(int i=0;i<5;i++){
            if(this->collection[i] != collection[i]){
                return false;
            }
        }
        return true;
    }
};
const Status failedStatus = Status(DAYS_LIMIT, 0, required);

int updated = 0;
bool inline updateTable(const Status& status){
    if(status.getCost() < table(status.collection)){

        if(table(status.collection) == UINT16_MAX && (++updated) % 500000 == 0){
            const double progress = 100.0 * 2 * 31 / (31 + 1) * updated / (table.size());
            const double time = timer.toMilisec();

            printf("time = %3f sec. clear time (estimated) = %4f sec. progress = %f ",
                time / 1000,
                time * 100.0 / progress / 1000,
                (float)progress
            );
            puts("%");
        }

        table(status.collection) = status.getCost();
        return true;
    }
    else{
        return false;
    }
}

const inline Status solve(const Status& status/*, const int lastColor = -1*/){
    if(status.isGoal()){
        return status;
    }

    Status bestStatus;
    int bestColor;
    bool solved = false;

    array<int, 5> colors;

    colors = {WHITE, PURPLE, RED, BLUE, YELLOW};

    /*if(!status.history.empty()){
        cout << "not empty" << endl;
        const int back = status.history.back();
        if(howManyBought(status.collection, status.day, back) % 25 >= 2){
            for(int i=back-1;i>=0;i--){
                swap(colors[i], colors[i + 1]);
            }
            for(int i=0;i<5;i++) cout << colors[i] << " ";cout << endl;
        }
    }*/

    for(int color : colors){
        if(status.collection[color] == required[color]){
            continue;
        }

        if(color == BLUE && ignoreBlue && status.collection[RED] == status.collection[BLUE]){
            continue;
        }

        count1++;

        /*{
            Collection c = status.collection;
            c[color]++;
            if(status.getCost() + priceOf(status.collection, status.day, color) >= table(c)){
                continue;
            }
        }*/
        
        int daysAdvanced = 0;

        const auto moneyRemaining = [&](){
            return status.money + daysAdvanced * 5 - priceOf(status.collection, status.day + daysAdvanced, color);
        };
 
        // お金が足りるようになるまで
        while(moneyRemaining() < 0){
            if(status.day + daysAdvanced == DAYS_LIMIT){
                cout << "Failed : Days spent has reached the limit" << endl;
                cout << "DAYS_LIMIT = " << DAYS_LIMIT << endl;
                exit(0);
            }

            daysAdvanced++;
        }

        Status statusBought 
            = Status(status.day + daysAdvanced, moneyRemaining(), status.collection);
        statusBought.collection[color]++;
        
        if(!updateTable(statusBought)){
            continue;
        }

        count2++;

        Status solvedStatus = solve(statusBought/*, color*/);

        if(solvedStatus == failedStatus){
            continue;
        }

        if(solved){
            if(solvedStatus.getCost() < bestStatus.getCost()){
                bestStatus = solvedStatus;
                bestColor = color;
            }
        }
        else{
            bestStatus = solvedStatus;
            bestColor = color;
            solved = true;
        }
    }

    if(!solved){
        return failedStatus;
    }

    bestStatus.history.push_front(bestColor);
    bestStatus.history.insert(next(bestStatus.history.begin(), 0),
        status.history.begin(), status.history.end()
        );

    return bestStatus;
}

void init(){
    ifstream ifs(INPUT_FILE.data());
    string buf;
    int index = 0;
    while(getline(ifs, buf, ' ')){
        if(index < 5){
            required[index] = stoi(buf);
        }
        index++;
    }
    ifs.close();

    if(required[RED] == required[BLUE]){
        ignoreBlue = true;
    }
    
    table = Table5d<uint16_t>(
        required[0] + 1, required[1] + 1, required[2] + 1, required[3] + 1, required[4] + 1,
        UINT16_MAX);
}

/*
void preprocess(){
    Status status(1, 5);
    for(int color : {WHITE, PURPLE, RED, YELLOW, BLUE}){
        while(status.collection[color] < required[color]){
            int daysAdvanced = 0;

            // お金が足りるようになるまで
            while(status.money < priceOf(status.collection, status.day, color)){
                status.money += 5;
                status.day++;
            }

            status.money -= priceOf(status.collection, status.day + daysAdvanced, color);
            status.collection[color]++;
        }
    }
}
*/

void writeResult(const Status& result, const Timer& timer){
    ofstream ofs(OUTPUT_FILE.data());

    auto printResult = [&](ostream& os){
        os << "result = " << result << endl;
        os << "time = " << timer.toMilisec() << " milisec." << endl;
    };

    printResult(cout);
    printResult(ofs);

    const auto history = result.history;
    int index = 0;
    auto it = history.begin();
    int day = 1, money = 5;
    Collection collection = {};
    auto printStatus = [&](){
        ofs
        << "day = " << setw(3) << day
        << ", money = " << setw(1) << money
        << ", collection = " << collection
        << endl;
    };
    printStatus();
    while(true){
        if(money >= priceOf(collection, day, *it)){
            money -= priceOf(collection, day, *it);
            collection[*it]++;
            printStatus();
            if(++it == history.end()){
                break;
            }
        }
        else{
            day++;
            money += 5;
            printStatus();
        }
    }
    ofs.close();
}

int main(){
    timer.start();

    init();

    //preprocess();
    
    Status result = solve(Status(1, 5));
    timer.stop();
    
    writeResult(result, timer);

    cout << "count1 = " << count1 << endl;
    cout << "count2 = " << count2 << endl;
    cout << "filled = " << ((int64_t)table.size() * (31 + 1) / (2 * 31)) << endl;
}