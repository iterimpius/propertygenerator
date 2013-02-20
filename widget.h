#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

namespace Ui {
class Widget;
}

class QtPropertyGenerator;

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

public slots:
    void deleteProperty();
    void generateDeclaration(QString &code);
    void generateComment(QString &code);
    void generatePrivate(QString &code);
    void generateNameType(QString &code);

    void generateGetterDeclaration(QString &code);
    void generateGetterDefinition(QString &code);
    void generateGetterInline(QString &code);

    void generateSetterDeclaration(QString &code);
    void generateSetterDefinition(QString &code);
    void generateSetterInline(QString &code);

    void generateResetDeclaration(QString &code);
    void generateResetDefinition(QString &code);
    void generateResetInline(QString &code);

    void generateNotify(QString &code);


private slots:
    void writeClicked();
    void chainClicked();
    void resetClicked();
    void notifyClicked();
    void revClicked();
    void designableClicked();
    void scriptableClicked();
    void storedClicked();
    void userCliced();
    void constantClicked();
    void finalClicked();
    void commentClicked();

    void setRev(int r);

    void validateName();
    void validateType();

protected:
    void paintEvent(QPaintEvent *);

signals:
    void deleteMe(Widget *);
    
private:
    Ui::Widget *ui;

    void initGui();
    void doConnections();

    QString name, capName, type, className;

    uint revision;

    bool    write,
            reset,
            chain,
            notify,
            rev,
            designable,
            scriptable,
            stored,
            user,
            constant,
            final,
            comment;

    QtPropertyGenerator *prnt;
};

#endif // WIDGET_H
