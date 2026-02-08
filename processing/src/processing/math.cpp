#include <processing/processing.hpp>

namespace processing
{

    class Xoshiro256PP
    {
    private:
        uint64_t s[4];

        static inline uint64_t rotl(uint64_t x, int k)
        {
            return (x << k) | (x >> (64 - k));
        }

    public:
        explicit Xoshiro256PP(uint64_t seed = 0x853c49e6748fea9bULL)
        {
            s[0] = seed;
            s[1] = seed * 0x9e3779b97f4a7c15ULL;
            s[2] = seed * 0xbf58476d1ce4e5b9ULL;
            s[3] = seed * 0x94d049bb133111ebULL;
        }

        uint64_t next()
        {
            uint64_t result = rotl(s[0] + s[3], 23) + s[0];
            uint64_t t = s[1] << 17;

            s[2] ^= s[0];
            s[3] ^= s[1];
            s[1] ^= s[2];
            s[0] ^= s[3];
            s[2] ^= t;
            s[3] = rotl(s[3], 45);

            return result;
        }

        f32 nextFloat()
        {
            return (next() >> 40) * 0x1.0p-24f;
        }
    };

    static Xoshiro256PP s_random;

    void randomSeed(const u64 seed)
    {
        s_random = Xoshiro256PP{seed};
    }

    f32 random(const f32 max)
    {
        return s_random.nextFloat() * max;
    }

    f32 random(const f32 min, const f32 max)
    {
        return min + s_random.nextFloat() * (max - min);
    }
} // namespace processing

namespace processing
{
    f32 map(const f32 value, const f32 istart, const f32 istop, const f32 ostart, const f32 ostop)
    {
        return ostart + (ostop - ostart) * ((value - istart) / (istop - istart));
    }
} // namespace processing
