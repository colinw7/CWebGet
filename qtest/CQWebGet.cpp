#include <CQWebGet.h>
#include <CWebGet.h>

#include "ui_CQWebGet.h"

QWebGet::
QWebGet()
{
  Ui::QWebGet ui;

  ui.setupUi(this);

  webget_ = nullptr;

  url_text_ = ui.urlText;
  url_tree_ = ui.urlTree;

  show();
}

QWebGet::
~QWebGet()
{
  delete webget_;
}

void
QWebGet::
setText(const QString &text)
{
  url_text_->setText(text);

  load();
}

void
QWebGet::
load()
{
  QString url_str = url_text_->text();

  webget_ = new CWebGet(url_str.toStdString());

  if (debug_)
    webget_->setDebug(true);

  CUrl url(url_str.toStdString());

  CWebGetUrl web_url(&(*webget_), url);

  url_tree_->clear();

  QStringList headers;

  headers.push_back("Url");
  headers.push_back("Loaded");

  url_tree_->setHeaderLabels(headers);

  if (webget_->loadPage(web_url)) {
    std::vector<CWebGetUrl *> web_urls;

    if (! webget_->getWebUrls(web_url, web_urls))
      return;

    int num_web_urls = web_urls.size();

    for (int i = 0; i < num_web_urls; ++i) {
      if (! webget_->isValidSite(*web_urls[i]))
        continue;

      QStringList strs;

      strs << QString(web_urls[i]->getUrlStr().c_str());

      strs << QString(webget_->isLoaded(*web_urls[i]) ? "Yes" : "No");

      QTreeWidgetItem *item = new QTreeWidgetItem (url_tree_, strs);

      url_tree_->addTopLevelItem(item);
    }
  }
}
