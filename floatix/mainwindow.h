#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

class QPlainTextEdit;
class QShowEvent;
class QPropertyAnimation;
class QTextDocument;
class QPushButton;
class QLabel;
class QWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    bool event(QEvent *event) override;
    void showEvent(QShowEvent *event) override;

private slots:
    void expandWindow();
    void colapseWindow();
    void saveNote();

private:
    QRect calculatePosition(const QSize& size);
    void applyStyleSheet();
    void buildInterface();
    void setCompactMode(bool compact);

    QTimer timerExpand;
    QTimer timerColapse;
    QPropertyAnimation *geometryAnim;

    QSize  MIN_SIZE;
    QSize  MAX_SIZE;
    const int EXPAND_TIME = 300;
    const int MAX_TEXT_BLOCKS = 2000;

    bool mouseInside;
    bool expandedWindow;
    QWidget *panelWidget;
    QPushButton *saveButton;
    QPushButton *closeButton;
    QLabel *logoLabel;
    QPlainTextEdit *textEditor;
};
#endif // MAINWINDOW_H
