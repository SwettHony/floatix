#include "mainwindow.h"

#include <QPropertyAnimation>
#include <QGridLayout>
#include <QPushButton>
#include <QPlainTextEdit>
#include <QLabel>
#include <QPixmap>
#include <QStyle>
#include <QTextDocument>
#include <QTextOption>
#include <QApplication>
#include <QScreen>
#include <QShowEvent>
#include <QFileDialog>
#include <QSaveFile>
#include <QTextStream>
#include <QMessageBox>
#include <QStandardPaths>
#include <QFileInfo>
#include <QDir>
#include <QDateTime>
#include <QStringConverter>
#include <QCoreApplication>
#include <QFile>
#include <QStringList>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , geometryAnim(new QPropertyAnimation(this, "geometry", this))
    , mouseInside(false)
    , expandedWindow(false)
    , panelWidget(nullptr)
    , saveButton(nullptr)
    , closeButton(nullptr)
    , logoLabel(nullptr)
    , textEditor(nullptr)
{
    // Definir el tamaño de ambas pantallas
    MIN_SIZE = QSize(50, 50);
    MAX_SIZE = QSize(400, 600);

    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);

    timerExpand.setSingleShot(true);
    timerColapse.setSingleShot(true);

    connect(&timerExpand, &QTimer::timeout, this, &MainWindow::expandWindow);
    connect(&timerColapse, &QTimer::timeout, this, &MainWindow::colapseWindow);

    timerExpand.setInterval(100);
    timerColapse.setInterval(300);

    geometryAnim->setDuration(EXPAND_TIME);
    geometryAnim->setEasingCurve(QEasingCurve::OutCubic);

    buildInterface();
    applyStyleSheet();
    setCompactMode(true);
    setGeometry(calculatePosition(MIN_SIZE));

}

MainWindow::~MainWindow() = default;

void MainWindow::buildInterface(){
    panelWidget = new QWidget();
    QGridLayout *expandedLayout = new QGridLayout(panelWidget);
    panelWidget->setObjectName("panel");
    expandedLayout->setContentsMargins(12, 12, 12, 12);
    expandedLayout->setSpacing(8);

    saveButton = new QPushButton("Save");
    closeButton = new QPushButton("X");
    closeButton->setObjectName("closeButton");
    QObject::connect(closeButton, &QPushButton::clicked, this, &QApplication::quit);

    logoLabel = new QLabel();
    logoLabel->setObjectName("logoLabel");
    logoLabel->setAlignment(Qt::AlignCenter);
    logoLabel->setMinimumSize(24, 24);

    QPixmap logo("/home/SwettHony/Desktop/SwettHony/iconos/waos.png");
    if (!logo.isNull()) {
        logoLabel->setPixmap(logo.scaled(24, 24, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else {
        logoLabel->setText("N");
    }

    textEditor = new QPlainTextEdit();
    textEditor->setPlaceholderText(tr("Escribe tu nota aqui..."));
    textEditor->setUndoRedoEnabled(false);
    textEditor->setLineWrapMode(QPlainTextEdit::WidgetWidth);
    textEditor->setWordWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    textEditor->document()->setMaximumBlockCount(MAX_TEXT_BLOCKS);
    connect(saveButton, &QPushButton::clicked, this, &MainWindow::saveNote);

    expandedLayout->addWidget(saveButton, 0, 0);
    expandedLayout->addWidget(closeButton, 0, 1);
    expandedLayout->addWidget(textEditor, 1, 0, 1, 2);
    expandedLayout->addWidget(logoLabel, 0, 0, 2, 2, Qt::AlignCenter);

    setCentralWidget(panelWidget);
}

void MainWindow::applyStyleSheet()
{
    const QString appDir = QCoreApplication::applicationDirPath();
    const QStringList styleCandidates = {
        appDir + "/style.qss",
        appDir + "/../style.qss",
        QDir::currentPath() + "/style.qss"
    };

    for (const QString &path : styleCandidates) {
        QFile styleFile(path);
        if (styleFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            setStyleSheet(QString::fromUtf8(styleFile.readAll()));
            return;
        }
    }

}

void MainWindow::setCompactMode(bool compact)
{
    if (!panelWidget || !saveButton || !closeButton || !textEditor || !logoLabel) {
        return;
    }

    saveButton->setVisible(!compact);
    closeButton->setVisible(!compact);
    textEditor->setVisible(!compact);
    logoLabel->setVisible(compact);

    panelWidget->setProperty("compact", compact);
    style()->unpolish(panelWidget);
    style()->polish(panelWidget);
    panelWidget->update();
}

bool MainWindow::event(QEvent *event){
    switch(event->type()){
    case QEvent::Enter:
        timerColapse.stop();
        if(!timerExpand.isActive() || !expandedWindow){
            timerExpand.start();
        }
        mouseInside = true;
        break;
    case QEvent::Leave:
        timerExpand.stop();
        if(!timerColapse.isActive() || expandedWindow){
            timerColapse.start();
        }
        mouseInside = false;
        break;
    default:
        break;
    }

    return QMainWindow::event(event);

}

void MainWindow::showEvent(QShowEvent *event)
{
    QMainWindow::showEvent(event);
    QTimer::singleShot(0, this, [this] {
        setGeometry(calculatePosition(size()));
    });
}

QRect MainWindow::calculatePosition(const QSize& size){
    QRect screenGeometry = QApplication::primaryScreen()->availableGeometry();

    QRect rect;
    rect.setSize(size);

    const int MARGIN = 10;

    int x = screenGeometry.right() - size.width() - MARGIN + 1;
    int y = screenGeometry.bottom() - size.height() - MARGIN + 1;

    x = qMax(screenGeometry.left() + MARGIN,
            qMin(x, screenGeometry.right() - size.width() - MARGIN + 1));
    y = qMax(screenGeometry.top() + MARGIN,
            qMin(y, screenGeometry.bottom() - size.height() - MARGIN + 1));

    rect.moveTo(x, y);

    return rect;
}

void MainWindow::expandWindow()
{
    if (!mouseInside || expandedWindow) return;

    expandedWindow = true;
    setCompactMode(false);
    geometryAnim->stop();
    geometryAnim->setStartValue(geometry());
    geometryAnim->setEndValue(calculatePosition(MAX_SIZE));
    geometryAnim->start();
}

void MainWindow::colapseWindow()
{
    if (mouseInside || !expandedWindow) return;

    expandedWindow = false;
    setCompactMode(true);
    geometryAnim->stop();
    geometryAnim->setStartValue(geometry());
    geometryAnim->setEndValue(calculatePosition(MIN_SIZE));
    geometryAnim->start();
}

void MainWindow::saveNote()
{
    if (!textEditor) {
        return;
    }

    const QString docsPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    const QString defaultName = QString("nota_%1.txt")
                                    .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd_HH-mm"));
    const QString defaultPath = QDir(docsPath.isEmpty() ? QDir::homePath() : docsPath)
                                    .filePath(defaultName);

    QString filePath = QFileDialog::getSaveFileName(
        this,
        tr("Guardar nota"),
        defaultPath,
        tr("Texto (*.txt);;Markdown (*.md);;LaTeX (*.tex);;Word (*.docx)"));

    if (filePath.isEmpty()) {
        return;
    }

    QFileInfo info(filePath);
    QString suffix = info.suffix().toLower();

    if (suffix.isEmpty()) {
        filePath += ".txt";
        suffix = "txt";
        info = QFileInfo(filePath);
    }

    if (suffix == "docx") {
        QMessageBox::information(
            this,
            tr("Formato no soportado"),
            tr("Guardar en .docx todavía no está implementado. Se guardará como .txt."));
        filePath = QDir(info.path()).filePath(info.completeBaseName() + ".txt");
        suffix = "txt";
    }

    const QString content = textEditor->toPlainText();

    QSaveFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(
            this,
            tr("Error al guardar"),
            tr("No se pudo abrir el archivo para escribir:\n%1").arg(filePath));
        return;
    }

    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);
    out << content;

    if (!file.commit()) {
        QMessageBox::critical(
            this,
            tr("Error al guardar"),
            tr("No se pudo guardar el archivo:\n%1").arg(filePath));
        return;
    }
}
