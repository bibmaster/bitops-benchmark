#include <benchmark/benchmark.h>
#include <algorithm>
#include <array>
#include <functional>
#include <random>

#ifdef _MSC_VER
#include <intrin.h>
constexpr uint32_t clz(uint32_t value) {
    unsigned long leading_zero = 0;
    return _BitScanReverse(&leading_zero, value) ? 31 - leading_zero : 32;
}
#else
#define clz(x) __builtin_clz(x)
#endif

struct CharLenFwd {
    template<class T>
    constexpr unsigned operator()(T x) noexcept {
        unsigned n = 1;
        for(;;) {
            if(x < 10)
                return n;
            if(x < 100)
                return n + 1;
            if(x < 1000)
                return n + 2;
            if(x < 10000)
                return n + 3;
            x /= 10000u;
            n += 4;
        }
        return n;
    }
};

struct CharLenClz {
    static constexpr uint32_t const powersOf10[] = {
        0,       10,       100,       1000,       10000,     100000,
        1000000, 10000000, 100000000, 1000000000, 0xFFFFFFFF};
    constexpr unsigned operator()(uint32_t x) noexcept {
        ++x;
        if(x == 0)
            return 10;
        auto t = unsigned(32 - clz(x)) * 5 / 16;
        return x <= powersOf10[t] ? t : t + 1;
    }
    constexpr unsigned operator()(uint64_t x) noexcept {
        return x < 0x100000000 ? operator()(uint32_t(x))
                               : (9 + operator()(x / 1000000000));
    }
};

template<class T, class Function>
struct BmCharLen : benchmark::Fixture {
    BmCharLen(std::string name, T s = 0, T e = std::numeric_limits<T>::max()) {
        SetName(name.c_str());
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<T> dis(s, e);
        std::generate(
            input.begin(), input.end(), [&gen, &dis] { return dis(gen); });
    }
    void BenchmarkCase(::benchmark::State& state) {
        size_t i = 0;
        for(auto _ : state)
            benchmark::DoNotOptimize(fn(input[i++ % input.size()]));
    }
    Function fn;
    constexpr static size_t input_size = 1024 * 64;
    std::array<T, input_size> input;
};

template<class T, class F>
void RegisterBenchmark(
    std::string fn, std::string distName, T s = 0,
    T e = std::numeric_limits<T>::max()) {
    ::benchmark::internal::RegisterBenchmarkInternal(
        new BmCharLen<T, F>(fn + ' ' + distName, s, e));
}

template<class F>
void RegisterBenchmarks(std::string fn) {
    RegisterBenchmark<uint32_t, F>(fn, "uniform");
    RegisterBenchmark<uint64_t, F>(fn, "uniform64");
    for(int i = 0; i < 10; ++i) {
        RegisterBenchmark<uint32_t, F>(
            fn, std::to_string(i + 1) + "-digit", CharLenClz::powersOf10[i],
            CharLenClz::powersOf10[i + 1] - 1);
    }
}

void RegisterBenchmarks() {
    RegisterBenchmarks<CharLenFwd>("fwd");
    RegisterBenchmarks<CharLenClz>("clz");
}

template<typename T1, typename T2>
bool test() {
    T1 c1;
    T2 c2;
    for(uint32_t i = 1; i != 0; i <<= 1)
        if(c1(i) != c2(i) || c1(i - 1) != c2(i - 1))
            return false;
    for(auto i : CharLenClz::powersOf10)
        if(c1(i) != c2(i) || c1(i - 1) != c2(i - 1))
            return false;
    return true;
}

int main(int argc, char* argv[]) {
    if(!test<CharLenClz, CharLenFwd>())
        return 1;
    RegisterBenchmarks();
    ::benchmark::Initialize(&argc, argv);
    if(::benchmark::ReportUnrecognizedArguments(argc, argv))
        return 1;
    ::benchmark::RunSpecifiedBenchmarks();
}
