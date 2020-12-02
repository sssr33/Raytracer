#include "GenPermutations.h"

std::vector<std::string> GenPermutations::operator()(const std::string& letters, size_t minLetterCount)
{
	if (letters.size() < minLetterCount)
	{
		// not enough letters
		return std::vector<std::string>();
	}

	std::vector<std::string> permutations;

	for (size_t letterCount = (std::max)(minLetterCount, static_cast<size_t>(1)); letterCount <= letters.size(); letterCount++)
	{
		AddIntermediateLetters(letters, std::string(), permutations, letterCount - 1);
	}

	return permutations;
}

void GenPermutations::AddIntermediateLetters(const std::string& letters, const std::string& startLetters, std::vector<std::string>& sink, size_t letterCount)
{
	if (letterCount)
	{
		for (const char c : letters)
		{
			AddIntermediateLetters(letters, startLetters + c, sink, letterCount - 1);
		}
	}
	else
	{
		AddLastLetters(letters, startLetters, sink);
	}
}

void GenPermutations::AddLastLetters(const std::string& letters, const std::string& startLetters, std::vector<std::string>& sink)
{
	for (const char c : letters)
	{
		sink.push_back(startLetters + c);
	}
}
