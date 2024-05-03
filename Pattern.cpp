#include "Pixel.h"
#include "Pattern.h"

    //constructor
    Pattern::Pattern(int id, int N) : id(id), N(N) {}
    bool Pattern::comparePixelPattern(const std::vector<Pixel>& otherPixeles) {

        if (otherPixeles.size() != pixeles.size()) {
            return false;
        }

        return otherPixeles == pixeles;
    }
    bool Pattern::compareCooPattern(const std::vector<int>& otherPixeles) {

        if (otherPixeles.size() != pixelesCoo.size()) {
            return false;
        }

        return otherPixeles == pixelesCoo;
    }

    std::vector<Pixel> Pattern::rotatePattern(const std::vector<Pixel>& Pattern) {
        std::vector<Pixel> newPattern;
        for (int i = 0; i < N; i++) {
            for (int j = N - 1; j >= 0; j--) {
                newPattern.push_back(Pattern[N * j + i]);
            }
        }
        return newPattern;
    }
    std::vector<int> Pattern::rotatePatternCoo(const std::vector<int>& Pattern) {
        std::vector<int> newPattern;
        for (int i = 0; i < N; i++) {
            for (int j = N - 1; j >= 0; j--) {
                newPattern.push_back(Pattern[N * j + i]);
            }
        }
        return newPattern;
    }
    std::vector<Pixel> Pattern::mirrorPattern(const std::vector<Pixel>& Pattern) {
        std::vector<Pixel> newPattern;

        for (int i = 0; i < N; i++) {
            for (int j = N - 1; j >= 0; j--) {
                newPattern.push_back(Pattern[j + i * N]);
            }
        }
        return newPattern;
    }
    std::vector<int> Pattern::mirrorPatternCoo(const std::vector<int>& Pattern) {
        std::vector<int> newPattern;

        for (int i = 0; i < N; i++) {
            for (int j = N - 1; j >= 0; j--) {
                newPattern.push_back(Pattern[j + i * N]);
            }
        }
        return newPattern;
    }

    void Pattern::addPixelVector(std::vector<Pixel> newPixeles) {
        pixeles = newPixeles;
    }
    void Pattern::addPixelCooVector(std::vector<int> newPixeles) {
        pixelesCoo = newPixeles;
    }

