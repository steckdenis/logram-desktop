#include <LCharacterTable.h>
#include <LCharacterWindow.h>

MainWindow::MainWindow(QTextEdit *parentText)
{
    text = parentText;

    QWidget *centralWidget = new QWidget;

    QLabel *fontLabel = new QLabel(tr("Font:"));
    fontCombo = new QFontComboBox;
    QLabel *sizeLabel = new QLabel(tr("Size:"));
    sizeCombo = new QComboBox;
    QLabel *styleLabel = new QLabel(tr("Style:"));
    styleCombo = new QComboBox;
    QLabel *fontMergingLabel = new QLabel(tr("Automatic Font Merging:"));
    fontMerging = new QCheckBox;
    fontMerging->setChecked(true);

    scrollArea = new QScrollArea;
    characterWidget = new CharacterWidget;
    scrollArea->setWidget(characterWidget);

    findStyles(fontCombo->currentFont());
    findSizes(fontCombo->currentFont());

    lineEdit = new QLineEdit;
    QPushButton *clipboardButton = new QPushButton(tr("&To clipboard"));

    clipboard = QApplication::clipboard();

    connect(fontCombo, SIGNAL(currentFontChanged(const QFont &)),
            this, SLOT(findStyles(const QFont &)));
    connect(fontCombo, SIGNAL(currentFontChanged(const QFont &)),
            this, SLOT(findSizes(const QFont &)));
    connect(fontCombo, SIGNAL(currentFontChanged(const QFont &)),
            characterWidget, SLOT(updateFont(const QFont &)));
    connect(sizeCombo, SIGNAL(currentIndexChanged(const QString &)),
            characterWidget, SLOT(updateSize(const QString &)));
    connect(styleCombo, SIGNAL(currentIndexChanged(const QString &)),
            characterWidget, SLOT(updateStyle(const QString &)));
    connect(characterWidget, SIGNAL(characterSelected(const QString &)),
            this, SLOT(insertCharacter(const QString &)));
    connect(clipboardButton, SIGNAL(clicked()), this, SLOT(updateClipboard()));
    connect(fontMerging, SIGNAL(toggled(bool)), characterWidget, SLOT(updateFontMerging(bool)));

    QHBoxLayout *controlsLayout = new QHBoxLayout;
    controlsLayout->addWidget(fontLabel);
    controlsLayout->addWidget(fontCombo, 1);
    controlsLayout->addWidget(sizeLabel);
    controlsLayout->addWidget(sizeCombo, 1);
    controlsLayout->addWidget(styleLabel);
    controlsLayout->addWidget(styleCombo, 1);
    controlsLayout->addWidget(fontMergingLabel);
    controlsLayout->addWidget(fontMerging, 1);
    controlsLayout->addStretch(1);

    QHBoxLayout *lineLayout = new QHBoxLayout;
    lineLayout->addWidget(lineEdit, 1);
    lineLayout->addSpacing(12);
    lineLayout->addWidget(clipboardButton);

    QVBoxLayout *centralLayout = new QVBoxLayout;
    centralLayout->addLayout(controlsLayout);
    centralLayout->addWidget(scrollArea, 1);
    centralLayout->addSpacing(4);
    centralLayout->addLayout(lineLayout);
    centralWidget->setLayout(centralLayout);

    setCentralWidget(centralWidget);
    setWindowTitle(tr("Character Map"));
}

void MainWindow::findStyles(const QFont &font)
{
    QFontDatabase fontDatabase;
    QString currentItem = styleCombo->currentText();
    styleCombo->clear();

    QString style;
    foreach (style, fontDatabase.styles(font.family()))
        styleCombo->addItem(style);

    int styleIndex = styleCombo->findText(currentItem);

    if (styleIndex == -1)
        styleCombo->setCurrentIndex(0);
    else
        styleCombo->setCurrentIndex(styleIndex);
}

void MainWindow::findSizes(const QFont &font)
{
    QFontDatabase fontDatabase;
    QString currentSize = sizeCombo->currentText();
    sizeCombo->blockSignals(true);
    sizeCombo->clear();

    int size;
    if(fontDatabase.isSmoothlyScalable(font.family(), fontDatabase.styleString(font))) {
        foreach(size, QFontDatabase::standardSizes()) {
            sizeCombo->addItem(QVariant(size).toString());
            sizeCombo->setEditable(true);
        }

    } else {
        foreach(size, fontDatabase.smoothSizes(font.family(), fontDatabase.styleString(font))) {
            sizeCombo->addItem(QVariant(size).toString());
            sizeCombo->setEditable(false);
        }
    }

    sizeCombo->blockSignals(false);

    int sizeIndex = sizeCombo->findText(currentSize);

    if(sizeIndex == -1)
        sizeCombo->setCurrentIndex(qMax(0, sizeCombo->count() / 3));
    else
        sizeCombo->setCurrentIndex(sizeIndex);
}

void MainWindow::insertCharacter(const QString &character)
{
    lineEdit->insert(character);
}

void MainWindow::updateClipboard()
{
    text->insertPlainText(lineEdit->text());
}
