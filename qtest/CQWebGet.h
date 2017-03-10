#include <QMainWindow>

class CWebGet;
class QLineEdit;
class QTreeWidget;

class QWebGet : public QMainWindow {
  Q_OBJECT

  Q_PROPERTY(bool debug READ isDebug WRITE setDebug)

 public:
  QWebGet();
 ~QWebGet();

  bool isDebug() const { return debug_; }
  void setDebug(bool b) { debug_ = b; }

  void setText(const QString &text);

 private slots:
  void load();

 private:
  bool         debug_ { false };
  CWebGet     *webget_ { nullptr };
  QLineEdit   *url_text_ { nullptr };
  QTreeWidget *url_tree_ { nullptr };
};
