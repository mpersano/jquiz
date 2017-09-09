#include "kana.h"

#include <vector>
#include <cstring>
#include <cwchar>
#include <cassert>
#include <tuple>

namespace {

class RomajiToKana
{
public:
    RomajiToKana();

    void convert(wchar_t *text, bool fullText) const;

private:
    void initialize();
    void trieInsert(const char *romaji, const wchar_t *kana);

    std::pair<const wchar_t *, int> convertToSingleKana(const wchar_t *text) const;

    struct TrieNode
    {
        TrieNode *children['z' - 'a' + 1] = {0};
        const wchar_t *kana = nullptr;
    };

    TrieNode *trie = nullptr;
};

RomajiToKana::RomajiToKana()
{
    initialize();
}

void RomajiToKana::trieInsert(const char *romaji, const wchar_t *kana)
{
    TrieNode *curNode, **parent = &trie;

    for (const char *p = romaji; *p; p++) {
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
        const wchar_t *kana;
    } romajiToKana[]
    {
        {  "a", L"あ" }, {  "i", L"い" }, {  "u", L"う" }, {  "e", L"え" }, {  "o", L"お" },
        { "ka", L"か" }, { "ki", L"き" }, { "ku", L"く" }, { "ke", L"け" }, { "ko", L"こ" },
        { "sa", L"さ" }, { "si", L"し" }, { "su", L"す" }, { "se", L"せ" }, { "so", L"そ" },
        { "shi", L"し" },
        { "ta", L"た" }, { "ti", L"ち" }, { "tu", L"つ" }, { "te", L"て" }, { "to", L"と" },
        { "chi", L"ち" }, { "tsu", L"つ" },
        { "na", L"な" }, { "ni", L"に" }, { "nu", L"ぬ" }, { "ne", L"ね" }, { "no", L"の" },
        { "ha", L"は" }, { "hi", L"ひ" }, { "fu", L"ふ" }, { "he", L"へ" }, { "ho", L"ほ" },
        { "ma", L"ま" }, { "mi", L"み" }, { "mu", L"む" }, { "me", L"め" }, { "mo", L"も" },
        { "ya", L"や" }, { "yu", L"ゆ" }, { "yo", L"よ" },
        { "ra", L"ら" }, { "ri", L"り" }, { "ru", L"る" }, { "re", L"れ" }, { "ro", L"ろ" },
        { "wa", L"わ" }, { "wo", L"を" },
        { "ga", L"が" }, { "gi", L"ぎ" }, { "gu", L"ぐ" }, { "ge", L"げ" }, { "go", L"ご" },
        { "za", L"ざ" }, { "ji", L"じ" }, { "zu", L"ず" }, { "ze", L"ぜ" }, { "zo", L"ぞ" },
        { "da", L"だ" }, { "di", L"ぢ" }, { "du", L"づ" }, { "de", L"で" }, { "do", L"ど" },
        { "ba", L"ば" }, { "bi", L"び" }, { "bu", L"ぶ" }, { "be", L"べ" }, { "bo", L"ぼ" },
        { "pa", L"ぱ" }, { "pi", L"ぴ" }, { "pu", L"ぷ" }, { "pe", L"ぺ" }, { "po", L"ぽ" },
        { "kya", L"きゃ" }, { "kyu", L"きゅ" }, { "kyo", L"きょ" },
        { "sha", L"しゃ" }, { "shu", L"しゅ" }, { "sho", L"しょ" },
        { "cha", L"ちゃ" }, { "chu", L"ちゅ" }, { "cho", L"ちょ" },
        { "nya", L"にゃ" }, { "nyu", L"にゅ" }, { "nyo", L"にょ" },
        { "hya", L"ひゃ" }, { "hyu", L"ひゅ" }, { "hyo", L"ひょ" },
        { "mya", L"みゃ" }, { "myu", L"みゅ" }, { "myo", L"みょ" },
        { "rya", L"りゃ" }, { "ryu", L"りゅ" }, { "ryo", L"りょ" },
        { "gya", L"ぎゃ" }, { "gyu", L"ぎゅ" }, { "gyo", L"ぎょ" },
        {  "ja", L"じゃ" }, {  "ju", L"じゅ" }, {  "jo", L"じょ" },
        { "bya", L"びゃ" }, { "byu", L"びゅ" }, { "byo", L"びょ" },
        { "pya", L"ぴゃ" }, { "pyu", L"ぴゅ" }, { "pyo", L"ぴょ" },
        {  "nn", L"ん" },
    };

    for (const auto& p : romajiToKana)
        trieInsert(p.romaji, p.kana);
}

std::pair<const wchar_t *, int> RomajiToKana::convertToSingleKana(const wchar_t *text) const
{
    if (*text == L'-') {
        return { L"゜", 1 };
    } else {
        TrieNode *curNode = trie;

        for (const wchar_t *p = text; *p; p++) {
            if (*p < L'a' || *p > L'z' ||
              (curNode = curNode->children[static_cast<int>(*p) - L'a']) == nullptr)
                break;

            if (curNode->kana)
                return { curNode->kana, p - text + 1 };
        }

        return { nullptr, 0 };
    }
}

void RomajiToKana::convert(wchar_t *text, bool fullText) const
{
    wchar_t *p, *q;

    q = p = text;

    while (*p) {
        int consumed;
        const wchar_t *kana;
        std::tie(kana, consumed) = convertToSingleKana(p);

        if (kana) {
            for (const wchar_t *r = kana; *r; r++)
                *q++ = *r;
            p += consumed;
        } else if (*p == L'n' && (fullText || p[1] != L'\0') && p[1] != L'y') {
            *q++ = L'ん';
            ++p;
        } else if (wcschr(L"cpstk", *p) && p[1] == *p) {
            // small 'tsu'
            *q++ = L'っ';
            ++p;
        } else {
            *q++ = *p++;
        }
    }

    *q = L'\0';
}

} // namespace

QString romajiToKana(const QString& romajiText, bool fullText)
{
    static RomajiToKana converter;

    std::vector<wchar_t> buffer(romajiText.size() + 1);

    romajiText.toWCharArray(&buffer[0]);
    buffer[romajiText.size()] = L'\0';

    converter.convert(&buffer[0], fullText);

    return QString::fromWCharArray(&buffer[0]);
}
