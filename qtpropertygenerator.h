#ifndef QTPROPERTYGENERATOR_H
#define QTPROPERTYGENERATOR_H

#include <QWidget>
#include <QList>
#include <QBitArray>

#include "widget.h"

namespace Ui {
class QtPropertyGenerator;
}

class QtPropertyGenerator : public QWidget
{
    Q_OBJECT
    
public:
    explicit QtPropertyGenerator(QWidget *parent = 0);
    ~QtPropertyGenerator();

    QString getClassName(){ return className;}

public slots:
    void addProperty();
    void deleteProperty(Widget *p);
    void initAllowedCharacters();
    QString doValidation(const QString &source);

    void generateCode();

private slots:
    void cInlineClicked();
    void cGroupClicked();
    void validateClassName();
    void getHeaderCode();
    void getSourceCode();
    
private:
    Ui::QtPropertyGenerator *ui;

    QString headerCode, sourceCode, className;
    bool inlined, grouped;

    void refreshGui();
    void doConnections();

    QList<Widget *> properties;

    QBitArray allowedCharacters;
};

#endif // QTPROPERTYGENERATOR_H
