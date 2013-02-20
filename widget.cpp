#include<QPainter>

#include "widget.h"
#include "ui_widget.h"
#include "qtpropertygenerator.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget),
    write(false),
    chain(false),
    reset(false),
    notify(false),
    rev(false),
    designable(true),
    scriptable(true),
    stored(true),
    user(false),
    constant(false),
    final(false),
    comment(false),
    revision(0)
{
    ui->setupUi(this);
    prnt = qobject_cast<QtPropertyGenerator*>(parent);
    initGui();
    doConnections();
}

Widget::~Widget()
{
    delete ui;
}

void Widget::deleteProperty()
{
    emit deleteMe(this);
}

void Widget::doConnections()
{
    connect(ui->cWrite, SIGNAL(clicked()), this, SLOT(writeClicked()));
    connect(ui->cReset, SIGNAL(clicked()), this, SLOT(resetClicked()));
    connect(ui->cChain, SIGNAL(clicked()), this, SLOT(chainClicked()));
    connect(ui->cNotify, SIGNAL(clicked()), this, SLOT(notifyClicked()));
    connect(ui->cRev, SIGNAL(clicked()), this, SLOT(revClicked()));
    connect(ui->cDesignable, SIGNAL(clicked()), this, SLOT(designableClicked()));
    connect(ui->cScriptable, SIGNAL(clicked()), this, SLOT(scriptableClicked()));
    connect(ui->cStored, SIGNAL(clicked()), this, SLOT(storedClicked()));
    connect(ui->cUser, SIGNAL(clicked()), this, SLOT(userCliced()));
    connect(ui->cConstant, SIGNAL(clicked()), this, SLOT(constantClicked()));
    connect(ui->cFinal, SIGNAL(clicked()), this, SLOT(finalClicked()));
    connect(ui->cComment, SIGNAL(clicked()), this, SLOT(commentClicked()));

    connect(ui->sRev, SIGNAL(valueChanged(int)), this, SLOT(setRev(int)));

    connect(ui->lName, SIGNAL(editingFinished()), this, SLOT(validateName()));
    connect(ui->lType, SIGNAL(editingFinished()), this, SLOT(validateType()));

    connect(ui->pDelete, SIGNAL(clicked()), this, SLOT(deleteProperty()));
}

void Widget::initGui()
{
    ui->cDesignable->setChecked(designable);
    ui->cScriptable->setChecked(scriptable);
    ui->cStored->setChecked(stored);

    ui->cEmitType->addItem("void");
    ui->cEmitType->addItem("by value");
    ui->cEmitType->addItem("by reference");
    ui->cEmitType->setEnabled(false);
}


void Widget::generateDeclaration(QString &code)
{     
    code.append("Q_PROPERTY(" + type + " " + name + " READ " + name);

    if (write) code.append( " WRITE set" + capName);
    if (reset) code.append( " RESET reset" + capName);
    if (notify) code.append( " NOTIFY " + name + "Changed");
    if (rev) code.append( " REVISION " + QString::number(revision));
    if (!designable) code.append( " DESIGNABLE false");
    if (!scriptable) code.append( " SCRIPTABLE false");
    if (!stored) code.append( " STORED false");
    if (user) code.append( " USER true");
    if (constant) code.append( " CONSTANT");
    if (final) code.append(" FINAL");
    code.append(")\n");
}

void Widget::generateComment(QString &code)
{
    if (comment) code.append("// " + ui->pComment->toPlainText() + "\n");
}

void Widget::generatePrivate(QString &code)
{
    code.append(type +" m_" + name + ";\n");
}

void Widget::generateNameType(QString &code)
{
    code.append(name + " : " + type + "\n");
}

void Widget::generateGetterDeclaration(QString &code)
{
    code.append(type + " " + name + "() const;\n");
}

void Widget::generateGetterDefinition(QString &code)
{
    code.append(type + " " + prnt->getClassName() + "::" + name + "() const {\n");
    code.append("\treturn m_" + name + ";\n");
    code.append("}\n\n");
}

void Widget::generateGetterInline(QString &code)
{
    code.append("inline " + type + " " + name + "() const {\n");
    code.append("\treturn m_" + name + ";\n");
    code.append("}\n\n");
}

void Widget::generateSetterDeclaration(QString &code)
{
    if (write) {
        code.append("Q_SLOT ");
        if (chain) {
            code.append(prnt->getClassName() + " &");
        } else {
            code.append("void");
        }
        code.append(" set" + capName + "(const " + type + " &v);\n");
    }
}

void Widget::generateSetterDefinition(QString &code)
{
    if (write) {
        if (chain) {
            code.append(prnt->getClassName() + " &" + prnt->getClassName() + "::set" + capName + "(const " + type + " &v) {\n");
        }
        else {
            code.append( "void " + prnt->getClassName() + "::set" + capName + "(const " + type + " &v) {\n");

        }

        code.append("\tif (m_" + name + " != v) {\n");
        code.append("\t\tm_" + name + " = v;\n");
        if (notify) {
            if (ui->cEmitType->currentIndex() > 0) code.append("\t\temit " + name + "Changed(" + "m_" + name + ");\n");
            else code.append("\t\temit " + name + "Changed();\n");
        }
        code.append("\t}\n");

        if (chain) code.append("\treturn *this;\n");
        code.append("}\n\n");
    }
}

void Widget::generateSetterInline(QString &code)
{
    if (write) {
        code.append("Q_SLOT ");
        if (chain) {
            code.append(prnt->getClassName() + " &");
        } else {
            code.append("void");
        }

        code.append(" set" + capName + "(const " + type + " &v) {\n");
        code.append("\tif (m_" + name + " != v) {\n");
        code.append("\t\tm_" + name + " = v;\n");
        if (notify) {
            if (ui->cEmitType->currentIndex() > 0) code.append("\t\temit " + name + "Changed(" + "m_" + name + ");\n");
            else code.append("\t\temit " + name + "Changed();\n");
        }
        code.append("\t}\n");

        if (chain) code.append("\treturn *this;\n");
        code.append("}\n\n");
    }
}

void Widget::generateResetDeclaration(QString &code)
{
    if (reset) {
        code.append("Q_SLOT void reset" + capName + "();\n");
    }
}

void Widget::generateResetDefinition(QString &code)
{
    if (reset) {
        code.append(type + " " + prnt->getClassName() + "::reset" + capName + "() {\n");
        code.append("\t//TODO reset\n}\n\n");
    }
}

void Widget::generateResetInline(QString &code)
{
    if (reset) {
        code.append("Q_SLOT void reset" + capName + "() {\n");
        code.append("\t//TODO reset\n}\n\n");
    }
}

void Widget::generateNotify(QString &code)
{
    if (notify) {
        code.append("Q_SIGNAL void " + name + "Changed(");
        if (ui->cEmitType->currentIndex() == 1) {
            code.append(type);
        } else if (ui->cEmitType->currentIndex() == 2) {
            code.append(type + " &");
        }
        code.append(");\n");
    }
}

void Widget::writeClicked()
{
    write = ui->cWrite->isChecked();
    ui->cChain->setEnabled(write);
}

void Widget::chainClicked()
{
    chain = ui->cChain->isChecked();
}

void Widget::resetClicked()
{
    reset = ui->cReset->isChecked();
}

void Widget::notifyClicked()
{
    notify = ui->cNotify->isChecked();
    ui->cEmitType->setEnabled(notify);
}

void Widget::revClicked()
{
    rev = ui->cRev->isChecked();
    ui->sRev->setEnabled(rev);
}

void Widget::designableClicked()
{
    designable = ui->cDesignable->isChecked();
}

void Widget::scriptableClicked()
{
    scriptable = ui->cScriptable->isChecked();
}

void Widget::storedClicked()
{
    stored = ui->cStored->isChecked();
}

void Widget::userCliced()
{
    user = ui->cUser->isChecked();
}

void Widget::constantClicked()
{
    constant = ui->cConstant->isChecked();
}

void Widget::finalClicked()
{
    final = ui->cFinal->isChecked();
}

void Widget::commentClicked()
{
    comment = ui->cComment->isChecked();
    ui->pComment->setEnabled(comment);
}

void Widget::setRev(int r)
{
    revision = r;
}

void Widget::validateName()
{
    name = prnt->doValidation(ui->lName->text());
    ui->lName->setText(name);
    capName = name;
    capName[0] = capName[0].toUpper();
}

void Widget::validateType()
{
    type = prnt->doValidation(ui->lType->text());
    ui->lType->setText(type);
}

void Widget::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    QLinearGradient g(rect().topRight(), rect().bottomRight());
    g.setColorAt(0.0f, Qt::white);
    g.setColorAt(1.0f, Qt::lightGray);
    p.fillRect(rect(), g);
}

