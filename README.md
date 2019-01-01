# Espia

<WORK IN PROGRESS>
Spanish-English quick lookup word-card dictionary.

## Build instructions
* Clone source ```git clone git@github.com:okosan/Espia.git```
* Open Qt project file ```espia.pro``` with Qt Creator (Qt 5.11 or higher is recommended)
* On Windows: ```Build->Build all``` should just work (assuming Mingw compiler setup).

## How to use
TODO: work in progress
Espia captures screen image and runs OCR (object character recogtion) in area under mouse pointer to extract spanish word.
This is very similar to how Abby Lingvo dictionary does things, but the goal is to have more data displayed in word card than Lingvo.
If word is detected under mouse, the word is fetched into http://www.spanishdict.com over plain HTTP to lookup for translation/wordcard data.
Wordcard is cached on disk to reduce future lookup time and allow offline mode. The word card for translation is displayed as window overlay over the mouse pointer.
Wordcard will contain word translation, noun with article and for verbs it will display verb conjugation table.

## Legal Stuff
TODO: work in progress

The code and files (that are not under ```Espia/thirdparty/```) is released to public domain. Native-borns of Antártida can also use GPL/LGPL/MIT license.

## Useful links
TODO: work in progress



