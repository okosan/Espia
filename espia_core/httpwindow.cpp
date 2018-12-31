#include <QtWidgets>
#include <QtNetwork>
#include <QUrl>

#include "httpwindow.h"
#include "ui_authenticationdialog.h"

#include <iostream>
#include <string>
#include <cctype>

const char defaultUrl[] = "http://www.spanishdict.com/translate/aquella";
const char defaultFileName[] = "";
const char defaultDirectoryName[] = "./";

ProgressDialog::ProgressDialog(const QUrl &url, QWidget *parent)
  : QProgressDialog(parent)
{
    setWindowTitle(tr("Download Progress"));
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setLabelText(tr("Downloading %1.").arg(url.toDisplayString()));
    setMinimum(0);
    setValue(0);
    setMinimumDuration(0);
    setMinimumSize(QSize(400, 75));
}

void ProgressDialog::networkReplyProgress(qint64 bytesRead, qint64 totalBytes)
{
    setMaximum(totalBytes);
    setValue(bytesRead);
}

HttpWindow::HttpWindow(QWidget *parent)
    : QDialog(parent)
    , urlLineEdit(new QLineEdit(defaultUrl))
    , downloadButton(new QPushButton(tr("Download")))
    , defaultFileLineEdit(new QLineEdit(defaultFileName))
    , downloadDirectoryLineEdit(new QLineEdit(QDir::toNativeSeparators(defaultDirectoryName)))
    , reply(nullptr)
    , file(nullptr)
    , httpRequestAborted(false)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowTitle(tr("HTTP"));

    connect(&qnam, &QNetworkAccessManager::authenticationRequired,
            this, &HttpWindow::slotAuthenticationRequired);
#ifndef QT_NO_SSL
    connect(&qnam, &QNetworkAccessManager::sslErrors,
            this, &HttpWindow::sslErrors);
#endif

    QFormLayout *formLayout = new QFormLayout;
    urlLineEdit->setClearButtonEnabled(true);
    connect(urlLineEdit, &QLineEdit::textChanged,
            this, &HttpWindow::enableDownloadButton);
    formLayout->addRow(tr("&URL:"), urlLineEdit);
    /*
    QString downloadDirectory = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
    if (downloadDirectory.isEmpty() || !QFileInfo(downloadDirectory).isDir())
        downloadDirectory = QDir::currentPath();
    downloadDirectoryLineEdit->setText(QDir::toNativeSeparators(downloadDirectory));
    */
    formLayout->addRow(tr("&Download directory:"), downloadDirectoryLineEdit);
    formLayout->addRow(tr("Default &file:"), defaultFileLineEdit);
    //launchCheckBox->setChecked(true);
    //formLayout->addRow(launchCheckBox);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(formLayout);

    mainLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Ignored, QSizePolicy::MinimumExpanding));

    downloadButton->setDefault(true);
    connect(downloadButton, &QAbstractButton::clicked, this, &HttpWindow::downloadFile);
    QPushButton *quitButton = new QPushButton(tr("Quit"));
    quitButton->setAutoDefault(false);
    connect(quitButton, &QAbstractButton::clicked, this, &QWidget::close);
    QDialogButtonBox *buttonBox = new QDialogButtonBox;
    buttonBox->addButton(downloadButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(quitButton, QDialogButtonBox::RejectRole);
    mainLayout->addWidget(buttonBox);

    urlLineEdit->setFocus();
}

void HttpWindow::startRequest(const QUrl &requestedUrl)
{
    url = requestedUrl;
    httpRequestAborted = false;

    reply = qnam.get(QNetworkRequest(url));
    connect(reply, &QNetworkReply::finished, this, &HttpWindow::httpFinished);
    connect(reply, &QIODevice::readyRead, this, &HttpWindow::httpReadyRead);

    ProgressDialog *progressDialog = new ProgressDialog(url, this);
    progressDialog->setAttribute(Qt::WA_DeleteOnClose);
    connect(progressDialog, &QProgressDialog::canceled, this, &HttpWindow::cancelDownload);
    connect(reply, &QNetworkReply::downloadProgress, progressDialog, &ProgressDialog::networkReplyProgress);
    connect(reply, &QNetworkReply::finished, progressDialog, &ProgressDialog::hide);
    progressDialog->show();
}

void HttpWindow::downloadFile()
{
    const QString urlSpec = urlLineEdit->text().trimmed();
    if (urlSpec.isEmpty())
        return;

    const QUrl newUrl = QUrl::fromUserInput(urlSpec);
    if (!newUrl.isValid()) {
        QMessageBox::information(this, tr("Error"),
                                 tr("Invalid URL: %1: %2").arg(urlSpec, newUrl.errorString()));
        return;
    }

    QString fileName = newUrl.fileName();
    if (fileName.isEmpty())
        fileName = defaultFileLineEdit->text().trimmed();
    if (fileName.isEmpty())
        fileName = defaultFileName;
    QString downloadDirectory = QDir::cleanPath(downloadDirectoryLineEdit->text().trimmed());
    bool useDirectory = !downloadDirectory.isEmpty() && QFileInfo(downloadDirectory).isDir();
    if (useDirectory)
        fileName.prepend(downloadDirectory + '/');
    if (QFile::exists(fileName)) {
        QFile::remove(fileName);
    }

    file = openFileForWrite(fileName);
    if (!file)
        return;

    downloadButton->setEnabled(false);

    // schedule the request
    startRequest(newUrl);
}

QFile *HttpWindow::openFileForWrite(const QString &fileName)
{
    QScopedPointer<QFile> file(new QFile(fileName));
    if (!file->open(QIODevice::WriteOnly)) {
        QMessageBox::information(this, tr("Error"),
                                 tr("Unable to save the file %1: %2.")
                                 .arg(QDir::toNativeSeparators(fileName),
                                      file->errorString()));
        return nullptr;
    }
    return file.take();
}

void HttpWindow::cancelDownload()
{
    httpRequestAborted = true;
    reply->abort();
    downloadButton->setEnabled(true);
}

void HttpWindow::httpFinished()
{
    QFileInfo fi;
    if (file) {
        fi.setFile(file->fileName());
        file->close();
        delete file;
        file = nullptr;
    }

    if (httpRequestAborted) {
        reply->deleteLater();
        reply = nullptr;
        return;
    }

    if (reply->error()) {
        QFile::remove(fi.absoluteFilePath());
        downloadButton->setEnabled(true);
        reply->deleteLater();
        reply = nullptr;
        return;
    }

    const QVariant redirectionTarget = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);

    reply->deleteLater();
    reply = nullptr;

    if (!redirectionTarget.isNull()) {
        const QUrl redirectedUrl = url.resolved(redirectionTarget.toUrl());
        if (QMessageBox::question(this, tr("Redirect"),
                                  tr("Redirect to %1 ?").arg(redirectedUrl.toString()),
                                  QMessageBox::Yes | QMessageBox::No) == QMessageBox::No) {
            QFile::remove(fi.absoluteFilePath());
            downloadButton->setEnabled(true);
            return;
        }
        file = openFileForWrite(fi.absoluteFilePath());
        if (!file) {
            downloadButton->setEnabled(true);
            return;
        }
        startRequest(redirectedUrl);
        return;
    }

    downloadButton->setEnabled(true);
}

// TODO: Make it work with unicode
// Input:  "!!~1casa:)*$#@"
// Output: "casa"
std::string filter_function(const std::string &broken_word)
{
    std::string filtered_word;
    for (const char c_broken : broken_word)
    {
        if (std::isalpha(c_broken))
        {
            filtered_word += c_broken;
        }
    }
    return filtered_word;
}

void HttpWindow::httpReadyRead()
{
    // this slot gets called every time the QNetworkReply has new data.
    // We read all of its new data and write it into the file.
    // That way we use less RAM than when reading it at the finished()
    // signal of the QNetworkReply
    if (file)
    {
        QByteArray byte_array = reply->readAll();
        std::string text  = byte_array.toStdString();

        // Check if valid character card
        std::string page_indication  = "<!-- neodict-entry-->";
        if (text.find(page_indication) != std::string::npos)
        {
            std::cout << "Found entry!" << std::endl << std::flush;

            std::string start_delimiter  = "<!-- neodict-entry-->";
            std::string end_delimiter  = "<!-- end-neodict-entry-->";

            // Attempt to extract text block between start and end letters
            size_t start_position = text.find(start_delimiter);
            size_t end_position = text.find(end_delimiter);

            bool start_valid = (start_position  != std::string::npos);
            bool end_valid = (end_position  != std::string::npos);

            bool is_valid = start_valid && end_valid;

            if (is_valid)
            {
                size_t extract_start = start_position + start_delimiter.length();
                size_t len = end_position - extract_start + 1;

                std::string textcard = text.substr(extract_start, len);

                std::cout << std::endl
                          << "TextCardfilename=<" << textcard << ">"
                          << std::endl << std::flush;

                bool is_noun = false;
                std::string noun_article = "";
                if (textcard.find("feminine noun")  != std::string::npos)
                {
                    is_noun = true;
                    noun_article = "la";
                }
                else
                {
                    if (textcard.find("masculine noun")  != std::string::npos)
                    {
                        is_noun = true;
                        noun_article = "el";
                    }
                }
                //textcard = "FEMININE NOUN!!!!!\n\n\n\n" + textcard;

                // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                // !!!!!!!!!!!! 2.5.1 THIS PART WILL COME FROM OCR module
                std::string broken_word = "!!!~1casa:)*****$#@";
                std::string filtered_word = filter_function(broken_word);
                textcard = filtered_word + "\n\n" + textcard;
                // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

                file->write(textcard.c_str(), textcard.length());

                // for debug
                int i = 0;
                i++;
            }
        }
    }
}

void HttpWindow::enableDownloadButton()
{
    downloadButton->setEnabled(!urlLineEdit->text().isEmpty());
}

void HttpWindow::slotAuthenticationRequired(QNetworkReply *, QAuthenticator *authenticator)
{
    QDialog authenticationDialog;
    Ui::Dialog ui;
    ui.setupUi(&authenticationDialog);
    authenticationDialog.adjustSize();
    ui.siteDescription->setText(tr("%1 at %2").arg(authenticator->realm(), url.host()));

    // Did the URL have information? Fill the UI
    // This is only relevant if the URL-supplied credentials were wrong
    ui.userEdit->setText(url.userName());
    ui.passwordEdit->setText(url.password());

    if (authenticationDialog.exec() == QDialog::Accepted) {
        authenticator->setUser(ui.userEdit->text());
        authenticator->setPassword(ui.passwordEdit->text());
    }
}

#ifndef QT_NO_SSL
void HttpWindow::sslErrors(QNetworkReply *, const QList<QSslError> &errors)
{
    QString errorString;
    foreach (const QSslError &error, errors) {
        if (!errorString.isEmpty())
            errorString += '\n';
        errorString += error.errorString();
    }

    if (QMessageBox::warning(this, tr("SSL Errors"),
                             tr("One or more SSL errors has occurred:\n%1").arg(errorString),
                             QMessageBox::Ignore | QMessageBox::Abort) == QMessageBox::Ignore) {
        reply->ignoreSslErrors();
    }
}
#endif
