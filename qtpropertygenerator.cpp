#include <QDebug>

#include "qtpropertygenerator.h"
#include "ui_qtpropertygenerator.h"

QtPropertyGenerator::QtPropertyGenerator(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QtPropertyGenerator),
    inlined(true),
    grouped(false),
    allowedCharacters(256)
{
    ui->setupUi(this);
    refreshGui();
    ui->pLayout->setAlignment(Qt::AlignTop);
    doConnections();
    initAllowedCharacters();
}

QtPropertyGenerator::~QtPropertyGenerator()
{
    delete ui;
}

void QtPropertyGenerator::addProperty()
{
    ui->toolBox->setCurrentIndex(0);
    Widget *p = new Widget(this);
    ui->pLayout->addWidget(p);
    properties.append(p);

    connect(p, SIGNAL(deleteMe(Widget*)), this, SLOT(deleteProperty(Widget*)));
}

void QtPropertyGenerator::deleteProperty(Widget *p)
{
    ui->pLayout->removeWidget(p);
    properties.removeOne(p);
    p->deleteLater();
}

void QtPropertyGenerator::cInlineClicked()
{
    inlined = ui->cInline->isChecked();
    refreshGui();
}

void QtPropertyGenerator::cGroupClicked()
{
    grouped = ui->cGroup->isChecked();
    refreshGui();
}

void QtPropertyGenerator::validateClassName()
{
    className = doValidation(ui->lClassName->text());
    ui->lClassName->setText(className);
}

void QtPropertyGenerator::getHeaderCode()
{
    ui->headerP->selectAll();
    ui->headerP->copy();
}

void QtPropertyGenerator::getSourceCode()
{
    ui->sourceP->selectAll();
    ui->sourceP->copy();
}

void QtPropertyGenerator::refreshGui()
{
    ui->cGroup->setChecked(grouped);
    ui->cInline->setChecked(inlined);

    ui->pGetSource->setVisible(!inlined);
    ui->sourceP->setVisible(!inlined);

    if (!properties.isEmpty()) generateCode();
}

void QtPropertyGenerator::doConnections()
{
    connect(ui->cGroup, SIGNAL(clicked()), this, SLOT(cGroupClicked()));
    connect(ui->cInline, SIGNAL(clicked()), this, SLOT(cInlineClicked()));
    connect(ui->btnAdd, SIGNAL(clicked()), this, SLOT(addProperty()));
    connect(ui->lClassName, SIGNAL(editingFinished()), this, SLOT(validateClassName()));
    connect(ui->btnGenerate, SIGNAL(clicked()), this, SLOT(generateCode()));
    connect(ui->pGetHeader, SIGNAL(clicked()), this, SLOT(getHeaderCode()));
    connect(ui->pGetSource, SIGNAL(clicked()), this, SLOT(getSourceCode()));
}

void QtPropertyGenerator::initAllowedCharacters()
{
    for (int i = 0; i < 256; ++i) allowedCharacters[i] = false;

    QString allowed("AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz_0123456789");

    for (int i = 0; i < allowed.length(); ++i) allowedCharacters[allowed.at(i).unicode()] = true;
}

QString QtPropertyGenerator::doValidation(const QString &source)
{
    if (!source.isEmpty()) {
        bool lastSpace = 0;
        bool isValid = 0;
        QString tempName;

        for (int i = 0; i < source.length(); ++i) {
            if (!isValid) {
                if (source.at(i).unicode() > 60) {
                    if (allowedCharacters[source.at(i).unicode()]) {
                        isValid = true;
                        tempName.append(source.at(i));
                    }
                }
            }
            else {
                if (source.at(i) != ' ') {
                    if (!lastSpace) {
                        if (allowedCharacters[source.at(i).unicode()]) {
                            tempName.append(source.at(i));
                        }
                    }
                    else {
                        if (allowedCharacters[source.at(i).unicode()]) {
                            tempName.append(source.at(i).toUpper());
                            lastSpace = false;
                        }
                    }
                }
                else lastSpace = true;
            }
        }
        if (!tempName.isEmpty()) return tempName;
        else return "INVALID";
    }
    else return "INVALID";
}

void QtPropertyGenerator::generateCode()
{
    validateClassName();
    headerCode.clear();
    sourceCode.clear();

    if (grouped) {
        if (inlined) {
            for (int i = 0; i < properties.size(); ++i) {
                headerCode.append("\n//Auto-generated property ");
                properties[i]->generateNameType(headerCode);
                properties[i]->generateComment(headerCode);
                properties[i]->generateDeclaration(headerCode);
                headerCode.append("private: ");
                properties[i]->generatePrivate(headerCode);
                headerCode.append("public: ");
                properties[i]->generateGetterInline(headerCode);
                properties[i]->generateSetterInline(headerCode);
                properties[i]->generateResetInline(headerCode);
                properties[i]->generateNotify(headerCode);
            }
        }
        else {
            for (int i = 0; i < properties.size(); ++i) {
                headerCode.append("\n//Auto-generated property ");
                properties[i]->generateNameType(headerCode);
                properties[i]->generateComment(headerCode);
                properties[i]->generateDeclaration(headerCode);
                headerCode.append("private: ");
                properties[i]->generatePrivate(headerCode);
                headerCode.append("public: ");
                properties[i]->generateGetterDeclaration(headerCode);
                properties[i]->generateGetterDefinition(sourceCode);
                properties[i]->generateSetterDeclaration(headerCode);
                properties[i]->generateSetterDefinition(sourceCode);
                properties[i]->generateResetDeclaration(headerCode);
                properties[i]->generateResetDefinition(sourceCode);
                properties[i]->generateNotify(headerCode);
            }
        }
    }
    else {
        if (inlined) {
            headerCode.append("//Auto-generated properties:\n");
            for (int i = 0; i < properties.size(); ++i) properties[i]->generateDeclaration(headerCode);
            headerCode.append("\nprivate:\n");
            for (int i = 0; i < properties.size(); ++i) properties[i]->generatePrivate(headerCode);
            headerCode.append("\npublic:\n");
            for (int i = 0; i < properties.size(); ++i) properties[i]->generateGetterInline(headerCode);
            headerCode.append("\n");
            for (int i = 0; i < properties.size(); ++i) properties[i]->generateSetterInline(headerCode);
            headerCode.append("\n");
            for (int i = 0; i < properties.size(); ++i) properties[i]->generateResetInline(headerCode);
            headerCode.append("\n");
            for (int i = 0; i < properties.size(); ++i) properties[i]->generateNotify(headerCode);
        }
        else {
            headerCode.append("//Auto-generated properties:\n");
            for (int i = 0; i < properties.size(); ++i) properties[i]->generateDeclaration(headerCode);
            headerCode.append("\nprivate:\n");
            for (int i = 0; i < properties.size(); ++i) properties[i]->generatePrivate(headerCode);
            headerCode.append("\npublic:\n");

            for (int i = 0; i < properties.size(); ++i) {
                properties[i]->generateGetterDeclaration(headerCode);
                properties[i]->generateGetterDefinition(sourceCode);
            }
            headerCode.append("\n");
            for (int i = 0; i < properties.size(); ++i) {
                properties[i]->generateSetterDeclaration(headerCode);
                properties[i]->generateSetterDefinition(sourceCode);
            }
            headerCode.append("\n");
            for (int i = 0; i < properties.size(); ++i) {
                properties[i]->generateResetDeclaration(headerCode);
                properties[i]->generateResetDefinition(sourceCode);
            }
            headerCode.append("\n");
            for (int i = 0; i < properties.size(); ++i) properties[i]->generateNotify(headerCode);
        }
    }

    ui->toolBox->setCurrentIndex(1);
    ui->headerP->setPlainText(headerCode);
    if (!inlined) ui->sourceP->setPlainText(sourceCode);
}
