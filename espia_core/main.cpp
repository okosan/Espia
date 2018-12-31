#include <QApplication>
#include <QDesktopWidget>
#include <QDir>

#include "httpwindow.h"

/* DONE:
 *  1.0, 2.5.1
 *
 * TODO:
 * 1) for any string text like "caza" - extract block of translated english card
 *    1.0) basic word extraction from internet dictionary
 *    1.1) full card with information on everything (from both translate and conjugation)
 * 2) when howering mouse over screen and clicking some button Ctrl or Shift
 *    2.1 take snapshot of the screen
 *    2.2 take pointer coordinates
 *    2.3 crop 256x256 px region around pointer (configurable dimensions)
 *    2.4 perform text "segmentation" and extract text from image using OCR
 *    2.5 find the nearest text word to pointer
 *      2.5.1 - isolate word - perform filtering - non-text caracters remove both trailing and leading
 *    2.6 fetch the text into online dictionary and extract text card
 * 3) displayed on overlay window the formatted text of translation (word_card)
 *
 * BONUS:
 * 4) do not thow away lookup result - store fetched data from internet into file on disk
 *    offline wordcard cache (to reduce internet lookup count)
 * 5) display word statistics "you should have learned this word by now, seen 9999 times!"
 * 6) for nouns - add gender-specific article "la casa"
 */

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    HttpWindow httpWin;
    const QRect availableSize = QApplication::desktop()->availableGeometry(&httpWin);
    httpWin.resize(availableSize.width() / 5, availableSize.height() / 5);
    httpWin.move((availableSize.width() - httpWin.width()) / 2, (availableSize.height() - httpWin.height()) / 2);

    httpWin.show();
    return app.exec();
}
