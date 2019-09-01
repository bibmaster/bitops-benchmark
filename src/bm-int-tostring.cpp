#include <benchmark/benchmark.h>
#include <algorithm>
#include <array>
#include <iostream>
#include <random>
#include <string>

template<typename T>
constexpr unsigned to_chars_len(T x) noexcept {
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

template<class T>
unsigned raw_format_100(char* buf, unsigned len, T i) noexcept {
    static constexpr char digits[201] =
        "0001020304050607080910111213141516171819"
        "2021222324252627282930313233343536373839"
        "4041424344454647484950515253545556575859"
        "6061626364656667686970717273747576777879"
        "8081828384858687888990919293949596979899";
    unsigned pos = len - 1;
    while(i >= 100) {
        auto const num = (i % 100) * 2;
        i /= 100;
        buf[pos] = digits[num + 1];
        buf[pos - 1] = digits[num];
        pos -= 2;
    }
    if(i >= 10) {
        auto const num = i * 2;
        buf[pos] = digits[num + 1];
        --pos;
        buf[pos] = digits[num];
    }
    else
        buf[pos] = '0' + i;
    return pos;
}

template<typename T>
unsigned raw_format_10(char* buf, unsigned len, T i) noexcept {
    unsigned pos = len - 1;
    while(i >= 10) {
        buf[pos] = '0' + (i % 10);
        --pos;
        i /= 10;
    }
    buf[pos] = '0' + char(i);
    return pos;
}

inline std::string std_to_string(uint32_t i) {
    auto len = to_chars_len(i);
    std::string result(len, '\0');
    raw_format_100(&result[0], len, i);
    return result;
}

template<class T>
std::enable_if_t<std::is_unsigned_v<T>, std::string> to_string(T i) {
    char buf[sizeof(T) * 3];
    auto pos = raw_format_10(buf, sizeof(buf), i);
    return {buf + pos, sizeof(buf) - pos};
}

template<class T>
std::enable_if_t<std::is_signed_v<T>, std::string> to_string(T i) {
    char buf[sizeof(T) * 3 + 1];
    const bool neg = i < 0;
    using U = std::make_unsigned_t<T>;
    const U u = neg ? (U)~i + 1u : i;
    auto pos = raw_format_10(buf, sizeof(buf), u);
    if(neg)
        buf[--pos] = '-';
    return {buf + pos, sizeof(buf) - pos};
}

struct to_string_100 {
    std::string operator()(uint32_t i) {
        return std_to_string(i);
    }
};

struct to_string_10 {
    std::string operator()(uint32_t i) {
        return to_string(i);
    }
};

template<class Function>
struct BmToString : benchmark::Fixture {
    BmToString(std::string name) {
        SetName(name.c_str());
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<uint32_t> dis;
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
    std::array<uint32_t, input_size> input;
};

void RegisterBenchmarks() {
    ::benchmark::internal::RegisterBenchmarkInternal(
        new BmToString<to_string_100>("base100"));
    ::benchmark::internal::RegisterBenchmarkInternal(
        new BmToString<to_string_10>("base10"));
}

int main(int argc, char* argv[]) {
    RegisterBenchmarks();
    ::benchmark::Initialize(&argc, argv);
    if(::benchmark::ReportUnrecognizedArguments(argc, argv))
        return 1;
    ::benchmark::RunSpecifiedBenchmarks();
}
