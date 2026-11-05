// Declara o componente visual que representa um tanque na interface gráfica

#ifndef CEDATANK_H
#define CEDATANK_H

#include <QToolButton>

class cedaTank : public QToolButton
{
    Q_OBJECT
public:
    explicit cedaTank(QWidget *parent = 0); // Constrói o componente visual do tanque

    int MaxValue = 100;
    int MinValue = 0;
    int Padding = 1;
    QString Deger1Suffix;
    QString Deger2Suffix;
    bool ShowDeger1 = false;
    bool ShowDeger2 = false;
    int Deger2 = 0;
    // Define o texto exibido no tanque
    void setText(QString s);

protected:
    void paintEvent(QPaintEvent *); // Desenha o tanque e seus valores na interface
signals:
    void valuechanged(int value);

public slots:
    void setValue(int value); // Atualiza o valor do tanque
    int value(); // Retorna o valor atual do tanque
private:
   int w_value = 24;
   QRectF outlineRect;
   QRectF inlineRect;

   QRectF Deger1Rect;
   QRectF Deger2Rect;

   int PenWidth = 5;
   QColor outColor = Qt::darkGray;
   QColor inColor = Qt::blue;
   QColor EmptyColor = Qt::gray;
   QFont NameFont;
   // Calcula o retângulo interno que representa o nível do tanque
   void Calculate_inlineRect();
   // Calcula o contorno e as áreas de texto do tanque
   void Calculate_outlineRect();
};

#endif
