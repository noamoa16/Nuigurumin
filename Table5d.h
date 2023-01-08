#include <array>
#include <vector>

using namespace std;

template<class T>
class Table5d{
    public:
    Table5d(){}
    Table5d(const size_t s0, const size_t s1, const size_t s2, const size_t s3, const size_t s4, const T& initValue = T()){
        mSizes = {s0, s1, s2, s3, s4};
        data = vector<T>(s0 * s1 * s2 * s3 * s4, initValue);
    }
    Table5d(const array<size_t, 5>& s, const T& initValue = T()){
        sizes = s;
        data = vector<T>(s[0] * s[1] * s[2] * s[3] * s[4]);
    }
    inline T& operator()(const size_t s0, const size_t s1, const size_t s2, const size_t s3, const size_t s4){
        return data[s0 + (s1 + (s2 + (s3 + s4 * mSizes[3]) * mSizes[2]) * mSizes[1]) * mSizes[0]];
    }

    template<class U>
    inline T& operator()(const array<U, 5> s){
        return data[s[0] + (s[1] + (s[2] + (s[3] + s[4] * mSizes[3]) * mSizes[2]) * mSizes[1]) * mSizes[0]];
    }
    const inline array<size_t, 5> sizes() const{
        return mSizes;
    }
    const inline size_t size(const int n) const{
        return mSizes[n];
    }
    const inline size_t size() const{
        return data.size();
    }

    private:
    array<size_t, 5> mSizes;
    vector<T> data;
};