#pragma once

#include <vector>
#include <string>

class GenPermutations
{
public:
	std::vector<std::string> operator()(const std::string& letters, size_t minLetterCount);

private:
	static void AddIntermediateLetters(const std::string& letters, const std::string& startLetters, std::vector<std::string>& sink, size_t letterCount);
	static void AddLastLetters(const std::string& letters, const std::string& startLetters, std::vector<std::string>& sink);
};
