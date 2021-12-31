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
        {  "a", U"あ" }, {  "i", U"い" }, {  "u", U"う" }, {  "e", U"え" }, {  "o", U"お" },
        { "ka", U"か" }, { "ki", U"き" }, { "ku", U"く" }, { "ke", U"け" }, { "ko", U"こ" },
        { "sa", U"さ" }, { "si", U"し" }, { "su", U"す" }, { "se", U"せ" }, { "so", U"そ" },
        { "shi", U"し" },
        { "ta", U"た" }, { "ti", U"ち" }, { "tu", U"つ" }, { "te", U"て" }, { "to", U"と" },
        { "chi", U"ち" }, { "tsu", U"つ" },
        { "na", U"な" }, { "ni", U"に" }, { "nu", U"ぬ" }, { "ne", U"ね" }, { "no", U"の" },
        { "ha", U"は" }, { "hi", U"ひ" }, { "fu", U"ふ" }, { "he", U"へ" }, { "ho", U"ほ" },
        { "ma", U"ま" }, { "mi", U"み" }, { "mu", U"む" }, { "me", U"め" }, { "mo", U"も" },
        { "ya", U"や" }, { "yu", U"ゆ" }, { "yo", U"よ" },
        { "ra", U"ら" }, { "ri", U"り" }, { "ru", U"る" }, { "re", U"れ" }, { "ro", U"ろ" },
        { "wa", U"わ" }, { "wo", U"を" },
        { "ga", U"が" }, { "gi", U"ぎ" }, { "gu", U"ぐ" }, { "ge", U"げ" }, { "go", U"ご" },
        { "za", U"ざ" }, { "ji", U"じ" }, { "zu", U"ず" }, { "ze", U"ぜ" }, { "zo", U"ぞ" },
        { "da", U"だ" }, { "di", U"ぢ" }, { "du", U"づ" }, { "de", U"で" }, { "do", U"ど" },
        { "ba", U"ば" }, { "bi", U"び" }, { "bu", U"ぶ" }, { "be", U"べ" }, { "bo", U"ぼ" },
        { "pa", U"ぱ" }, { "pi", U"ぴ" }, { "pu", U"ぷ" }, { "pe", U"ぺ" }, { "po", U"ぽ" },
        { "kya", U"きゃ" }, { "kyu", U"きゅ" }, { "kyo", U"きょ" },
        { "sha", U"しゃ" }, { "shu", U"しゅ" }, { "sho", U"しょ" },
        { "cha", U"ちゃ" }, { "chu", U"ちゅ" }, { "cho", U"ちょ" },
        { "nya", U"にゃ" }, { "nyu", U"にゅ" }, { "nyo", U"にょ" },
        { "hya", U"ひゃ" }, { "hyu", U"ひゅ" }, { "hyo", U"ひょ" },
        { "mya", U"みゃ" }, { "myu", U"みゅ" }, { "myo", U"みょ" },
        { "rya", U"りゃ" }, { "ryu", U"りゅ" }, { "ryo", U"りょ" },
        { "gya", U"ぎゃ" }, { "gyu", U"ぎゅ" }, { "gyo", U"ぎょ" },
        {  "ja", U"じゃ" }, {  "ju", U"じゅ" }, {  "jo", U"じょ" },
        { "bya", U"びゃ" }, { "byu", U"びゅ" }, { "byo", U"びょ" },
        { "pya", U"ぴゃ" }, { "pyu", U"ぴゅ" }, { "pyo", U"ぴょ" },
        {  "nn", U"ん" },
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
                result.push_back(L'ん');
                ++it;
            } else if (!isLast && (U"cpstk"s.find(*it) != std::u32string::npos && *nextIt == *it)) {
                // small 'tsu'
                result.push_back(L'っ');
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
