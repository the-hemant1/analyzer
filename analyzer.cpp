#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>
#include <vector>
#include <map>

struct KeyTerm {
    std::string label;
    std::string value;
};

std::vector<KeyTerm> extractKeyTerms(const std::string& text) {
    std::vector<KeyTerm> terms;

    std::regex partyRegex("(Party\\s+[AB]:\\s*)([A-Za-z ]+)");
    std::regex dateRegex("(Effective Date:|Date of Commencement:|Start Date:)([^\\n]+)");
    std::regex amountRegex("(Payment Amount:|Total Fee:)([^\\n]+)");

    std::smatch match;

    auto begin = text.cbegin(), end = text.cend();
    while (std::regex_search(begin, end, match, partyRegex)) {
        terms.push_back({"Party", match[2]});
        begin = match.suffix().first;
    }

    begin = text.cbegin();
    while (std::regex_search(begin, end, match, dateRegex)) {
        terms.push_back({"Date", match[2]});
        begin = match.suffix().first;
    }

    begin = text.cbegin();
    while (std::regex_search(begin, end, match, amountRegex)) {
        terms.push_back({"Amount", match[2]});
        begin = match.suffix().first;
    }

    return terms;
}

std::vector<std::string> detectRisks(const std::string& text) {
    std::vector<std::string> risks;

    if (text.find("indemnify") == std::string::npos)
        risks.push_back("Missing indemnification clause.");

    if (text.find("termination") == std::string::npos)
        risks.push_back("No termination clause found.");

    if (std::regex_search(text, std::regex("may at any time terminate")))
        risks.push_back("Unilateral termination detected.");

    return risks;
}

std::string summarizeContract(const std::vector<KeyTerm>& terms) {
    std::ostringstream summary;
    summary << "Summary:\n";

    for (const auto& term : terms) {
        summary << term.label << ": " << term.value << "\n";
    }

    return summary.str();
}

int main() {
    std::ifstream file("contract.txt");
    if (!file) {
        std::cerr << "Failed to open contract file.\n";
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string contractText = buffer.str();

    auto terms = extractKeyTerms(contractText);
    auto risks = detectRisks(contractText);
    auto summary = summarizeContract(terms);

    std::cout << summary << "\nRisks:\n";
    for (const auto& risk : risks) {
        std::cout << "- " << risk << "\n";
    }

    return 0;
}
