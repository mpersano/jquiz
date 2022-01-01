#include "kana.h"

#include <array>
#include <cassert>
#include <tuple>
#include <vector>

using namespace std::string_literals;

namespace {

class RomajiToKana
{
public:
    RomajiToKana();

    std::u32string convert(const std::u32string &text, bool fullText) const;

private:
    void initialize();
    void trieInsert(const char *romaji, const char32_t *kana);

    struct TrieNode {
        std::array<TrieNode *, 'z' - 'a' + 1> children = {};
        const char32_t *kana = nullptr;
    };

    TrieNode *trie = nullptr;
};

RomajiToKana::RomajiToKana()
{
    initialize();
}

void RomajiToKana::trieInsert(const char *romaji, const char32_t *kana)
{
    TrieNode *curNode, **parent = &trie;

    for (const char *p = romaji; *p; ++p) {
        int ch = *p;

        assert(ch >= 'a' && ch <= 'z');

        if ((curNode = *parent) == nullptr)
            *parent = curNode = new TrieNode;

        assert(curNode->kana == nullptr); // unique decodability

        parent = &curNode->children[ch - 'a'];
    }

    assert(!*parent); // unique decodability

    *parent = curNode = new TrieNode;
    curNode->kana = kana;
}

void RomajiToKana::initialize()
{
    static const struct {
        const char *romaji;
        const char32_t *kana;
    } romajiToKana[] {
        // clang-format off
        { "a", U"\U00003042" }, { "i", U"\U00003044" }, { "u", U"\U00003046" }, { "e", U"\U00003048" }, { "o", U"\U0000304a" },
        { "ka", U"\U0000304b" }, { "ki", U"\U0000304d" }, { "ku", U"\U0000304f" }, { "ke", U"\U00003051" }, { "ko", U"\U00003053" },
        { "sa", U"\U00003055" }, { "si", U"\U00003057" }, { "su", U"\U00003059" }, { "se", U"\U0000305b" }, { "so", U"\U0000305d" },
        { "shi", U"\U00003057" },
        { "ta", U"\U0000305f" }, { "ti", U"\U00003061" }, { "tu", U"\U00003064" }, { "te", U"\U00003066" }, { "to", U"\U00003068" },
        { "chi", U"\U00003061" }, { "tsu", U"\U00003064" },
        { "na", U"\U0000306a" }, { "ni", U"\U0000306b" }, { "nu", U"\U0000306c" }, { "ne", U"\U0000306d" }, { "no", U"\U0000306e" },
        { "ha", U"\U0000306f" }, { "hi", U"\U00003072" }, { "fu", U"\U00003075" }, { "he", U"\U00003078" }, { "ho", U"\U0000307b" },
        { "ma", U"\U0000307e" }, { "mi", U"\U0000307f" }, { "mu", U"\U00003080" }, { "me", U"\U00003081" }, { "mo", U"\U00003082" },
        { "ya", U"\U00003084" }, { "yu", U"\U00003086" }, { "yo", U"\U00003088" },
        { "ra", U"\U00003089" }, { "ri", U"\U0000308a" }, { "ru", U"\U0000308b" }, { "re", U"\U0000308c" }, { "ro", U"\U0000308d" },
        { "wa", U"\U0000308f" }, { "wo", U"\U00003092" },
        { "ga", U"\U0000304c" }, { "gi", U"\U0000304e" }, { "gu", U"\U00003050" }, { "ge", U"\U00003052" }, { "go", U"\U00003054" },
        { "za", U"\U00003056" }, { "ji", U"\U00003058" }, { "zu", U"\U0000305a" }, { "ze", U"\U0000305c" }, { "zo", U"\U0000305e" },
        { "da", U"\U00003060" }, { "di", U"\U00003062" }, { "du", U"\U00003065" }, { "de", U"\U00003067" }, { "do", U"\U00003069" },
        { "ba", U"\U00003070" }, { "bi", U"\U00003073" }, { "bu", U"\U00003076" }, { "be", U"\U00003079" }, { "bo", U"\U0000307c" },
        { "pa", U"\U00003071" }, { "pi", U"\U00003074" }, { "pu", U"\U00003077" }, { "pe", U"\U0000307a" }, { "po", U"\U0000307d" },
        { "kya", U"\U0000304d\U00003083" }, { "kyu", U"\U0000304d\U00003085" }, { "kyo", U"\U0000304d\U00003087" },
        { "sha", U"\U00003057\U00003083" }, { "shu", U"\U00003057\U00003085" }, { "sho", U"\U00003057\U00003087" },
        { "cha", U"\U00003061\U00003083" }, { "chu", U"\U00003061\U00003085" }, { "cho", U"\U00003061\U00003087" },
        { "nya", U"\U0000306b\U00003083" }, { "nyu", U"\U0000306b\U00003085" }, { "nyo", U"\U0000306b\U00003087" },
        { "hya", U"\U00003072\U00003083" }, { "hyu", U"\U00003072\U00003085" }, { "hyo", U"\U00003072\U00003087" },
        { "mya", U"\U0000307f\U00003083" }, { "myu", U"\U0000307f\U00003085" }, { "myo", U"\U0000307f\U00003087" },
        { "rya", U"\U0000308a\U00003083" }, { "ryu", U"\U0000308a\U00003085" }, { "ryo", U"\U0000308a\U00003087" },
        { "gya", U"\U0000304e\U00003083" }, { "gyu", U"\U0000304e\U00003085" }, { "gyo", U"\U0000304e\U00003087" },
        { "ja", U"\U00003058\U00003083" }, { "ju", U"\U00003058\U00003085" }, { "jo", U"\U00003058\U00003087" },
        { "bya", U"\U00003073\U00003083" }, { "byu", U"\U00003073\U00003085" }, { "byo", U"\U00003073\U00003087" },
        { "pya", U"\U00003074\U00003083" }, { "pyu", U"\U00003074\U00003085" }, { "pyo", U"\U00003074\U00003087" },
        { "nn", U"\U00003093" },
        // clang-format on
    };

    for (const auto &p : romajiToKana)
        trieInsert(p.romaji, p.kana);
}

std::u32string RomajiToKana::convert(const std::u32string &text, bool fullText) const
{
    std::u32string result;

    auto it = text.begin(), end = text.end();
    while (it != end) {
        bool found = false;
        TrieNode *curNode = trie;
        for (auto p = it; p != end; ++p) {
            if (*p < L'a' || *p > L'z')
                break;
            curNode = curNode->children[static_cast<int>(*p) - L'a'];
            if (!curNode)
                break;
            if (curNode->kana) {
                result.append(curNode->kana);
                it = std::next(p);
                found = true;
                break;
            }
        }
        if (!found) {
            auto nextIt = std::next(it);
            const auto isLast = nextIt == end;
            if (*it == L'n' && ((isLast && fullText) || (!isLast && *nextIt != L'y'))) {
                // n
                result.push_back(U'\U00003093');
                ++it;
            } else if (!isLast && (U"cpstk"s.find(*it) != std::u32string::npos && *nextIt == *it)) {
                // small 'tsu'
                result.push_back(U'\U00003063');
                ++it;
            } else {
                result.push_back(*it++);
            }
        }
    }

    return result;
}

} // namespace

QString romajiToKana(const QString &romajiText, bool fullText, bool katakanaInput)
{
    static RomajiToKana converter;

    auto kanaText = converter.convert(romajiText.toStdU32String(), fullText);

    if (katakanaInput) {
        for (auto &ch : kanaText) {
            constexpr auto HiraganaStart = 0x3040;
            constexpr auto HiraganaEnd = 0x309f;
            constexpr auto KatakanaStart = 0x30a0;
            if (ch >= HiraganaStart && ch <= HiraganaEnd)
                ch += KatakanaStart - HiraganaStart;
        }
    }

    return QString::fromStdU32String(kanaText);
}
