// Implementa o componente visual que representa um tanque na interface gráfica

#include "cedatank.h"
#include <QPainter>
#include <QDebug>
#include <QPaintEvent>
#include <QPainterPath>

// Inicializa as unidades de medida e a fonte exibida no tanque
cedaTank::cedaTank(QWidget *parent) : QToolButton(parent)
{
    Deger1Suffix = "kg";
    Deger2Suffix = "cm";
  NameFont.setFamily("Arial");
  NameFont.setBold(true);
  NameFont.setPointSize(12);
}

// Desenha o tanque, seu nível atual e os valores exibidos
void cedaTank::paintEvent(QPaintEvent *)
{

  QPainter myPainter(this);
  myPainter.setRenderHint(QPainter::Antialiasing);

  QPen BorderPen(outColor);
  QBrush inlineBrush(inColor);
  BorderPen.setWidth(PenWidth);
  myPainter.setPen(BorderPen);
  myPainter.setBrush(inlineBrush);
  Calculate_outlineRect();
  myPainter.drawRoundedRect(outlineRect, 25.0, 25.0);
  QPainterPath painterPath;
  painterPath.addRoundedRect(outlineRect,25,25);
  myPainter.setClipPath(painterPath);

  BorderPen.setColor(outColor);
  BorderPen.setWidth(0);
  myPainter.setPen(BorderPen);

  inlineBrush.setColor(EmptyColor);
  myPainter.setBrush(EmptyColor);
  Calculate_inlineRect();
  myPainter.drawRect(inlineRect);

  BorderPen.setColor(Qt::white);
  myPainter.setPen(BorderPen);
  myPainter.setFont(this->font());
  if (ShowDeger1)
      myPainter.drawText(Deger1Rect,Qt::AlignCenter,QString(QString::number(w_value) + " %1").arg(Deger1Suffix));
  if (ShowDeger2)
      myPainter.drawText(Deger2Rect,Qt::AlignCenter,QString(QString::number(Deger2) + " %1").arg(Deger2Suffix));
  Deger2Rect =  QRectF(outlineRect.x(),
                       outlineRect.y() + PenWidth,
                       outlineRect.width() ,
                       this->fontMetrics().height() + 5);
  BorderPen.setColor(Qt::red);
  myPainter.setPen(BorderPen);
  myPainter.setFont(NameFont);
  myPainter.drawText(Deger2Rect,Qt::AlignCenter,text());
  myPainter.setClipping(false);
  myPainter.end();
}

// Atualiza o valor do tanque respeitando seus limites mínimo e máximo
void cedaTank::setValue(int value)
{
    if (w_value != value){
        if (value <= MaxValue){
            w_value = value;
            emit valuechanged(w_value);
        } else {
            w_value = MaxValue;
            emit valuechanged(w_value);
        }

    }
    repaint();
}

// Retorna o valor atual do tanque
int cedaTank::value(){
    return w_value;
}

// Define o texto exibido no tanque
void cedaTank::setText(QString s){
    QToolButton::setText(s);
}

// Calcula o retângulo interno que representa o nível do tanque
void cedaTank::Calculate_inlineRect()
{

    qreal carpan,Yukseklik;
    carpan = qreal(w_value - MinValue) / qreal(MaxValue - MinValue);
    Yukseklik = (outlineRect.height() - (carpan * outlineRect.height()));

    inlineRect =  QRectF(outlineRect.x() , outlineRect.y() ,outlineRect.width(),Yukseklik );

}

// Calcula o contorno e as áreas de texto do tanque
void cedaTank::Calculate_outlineRect()
{
    outlineRect = QRectF(Padding,Padding,this->width() - (Padding * 2 ) , this->height()- (Padding * 2 ));
    Deger1Rect = QRectF(outlineRect.x(),
                        outlineRect.y() + (outlineRect.height() - (outlineRect.height() /4)),
                        outlineRect.width() ,
                        this->fontMetrics().height() + 5);
    Deger2Rect = QRectF(outlineRect.x(),
                        outlineRect.y() + (outlineRect.height() /4),
                        outlineRect.width() ,
                        this->fontMetrics().height() + 5);
}
