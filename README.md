# jquiz

Simple Japanese flash cards (mainly for my personal use).

Uses [Open JTalk](http://open-jtalk.sourceforge.net/) for speech synthesis.

## Usage

See the file `questions-sample` for a sample card file.

There are two modes:

* In *reading* mode (the default), the program will show you the kanji and you type
  the reading.
* In *kanji quiz* mode (start the program with `-k`) the program shows a prompt in
  English (or whatever), and you try to recall the Japanese kanji and reading.
  I usually write the kanji down on a piece of paper. Pressing any key shows the
  answer.

Cards can be in one of three decks: *normal*, *review* and *mastered*. Questions
in the *mastered* deck will only be shown with the `-m` command-line option.

To move cards between decks, use the following keys:

* Delete: move question to the *mastered* deck.
* Space: move question to the *review* deck.
* Tab: switch to review mode (show only questions in the *review* deck)
