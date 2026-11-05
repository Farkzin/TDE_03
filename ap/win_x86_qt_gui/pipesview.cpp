// Implementa a visualização dos tubos, válvulas, bomba e fluxos da simulação

#include "pipesview.h"
#include <QPainter>
#include <QPolygon>

// Inicializa o componente visual dos tubos
PipesView::PipesView(QWidget *parent) : QWidget(parent) {}

// Atualiza o estado visual da válvula V1
void PipesView::setV1(bool open){
    if(v1 != open){ v1 = open; update(); }
}

// Atualiza o estado visual da bomba B1
void PipesView::setB1(bool on){
    if(b1 != on){ b1 = on; update(); }
}

// Atualiza a posição e o fluxo visual da válvula V2
void PipesView::setV2(double position, bool flowing){
    if(v2_pos != position || v2_flowing != flowing){
        v2_pos = position;
        v2_flowing = flowing;
        update();
    }
}

// Atualiza o fluxo visual do dreno do tanque 2
void PipesView::setDrain2(bool flowing){
    if(drain2 != flowing){ drain2 = flowing; update(); }
}

// Atualiza o fluxo visual do dreno do tanque 3
void PipesView::setDrain3(bool flowing){
    if(drain3 != flowing){ drain3 = flowing; update(); }
}

// Desenha os tubos, os equipamentos e os fluxos da planta
void PipesView::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    const QColor water(30, 110, 230);
    const QColor dry(150, 150, 150);

    auto pipe = [&](bool flowing, const QPolygon &pts){
        QPen pen(flowing ? water : dry, 8, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        p.setPen(pen);
        p.setBrush(Qt::NoBrush);
        p.drawPolyline(pts);
    };

    pipe(v1, QPolygon({QPoint(20,300), QPoint(230,300), QPoint(230,345)}));
    pipe(b1, QPolygon({QPoint(335,520), QPoint(460,520), QPoint(460,150), QPoint(525,150)}));
    pipe(v2_flowing, QPolygon({QPoint(675,272), QPoint(780,272), QPoint(885,272)}));
    pipe(drain2, QPolygon({QPoint(600,255), QPoint(600,335), QPoint(790,335)}));
    pipe(drain3, QPolygon({QPoint(920,255), QPoint(920,335), QPoint(1110,335)}));

    p.setPen(Qt::NoPen);
    p.setBrush(drain2 ? water : dry);
    p.drawPolygon(QPolygon({QPoint(790,323), QPoint(812,335), QPoint(790,347)}));
    p.setBrush(drain3 ? water : dry);
    p.drawPolygon(QPolygon({QPoint(1110,323), QPoint(1132,335), QPoint(1110,347)}));

    p.setPen(QPen(Qt::black, 1));
    p.setBrush(v1 ? QColor(60,200,90) : QColor(220,60,60));
    p.drawPolygon(QPolygon({QPoint(90,284), QPoint(90,316), QPoint(110,300)}));
    p.drawPolygon(QPolygon({QPoint(130,284), QPoint(130,316), QPoint(110,300)}));

    QColor v2_color = QColor(255,200,0);
    if(v2_pos <= 0.0)      v2_color = QColor(220,60,60);
    else if(v2_pos >= 1.0) v2_color = QColor(60,200,90);
    p.setBrush(v2_color);
    p.drawPolygon(QPolygon({QPoint(760,256), QPoint(760,288), QPoint(780,272)}));
    p.drawPolygon(QPolygon({QPoint(800,256), QPoint(800,288), QPoint(780,272)}));

    p.setBrush(b1 ? QColor(60,200,90) : QColor(210,210,210));
    p.setPen(QPen(Qt::black, 2));
    p.drawEllipse(QPoint(460,430), 26, 26);
    p.setFont(QFont("Arial", 10, QFont::Bold));
    p.drawText(QRect(434,404,52,52), Qt::AlignCenter, "B1");

    p.setFont(QFont("Arial", 9));
    p.setPen(Qt::black);
    p.drawText(QRect(20,306,70,18), Qt::AlignLeft, "rede");
    p.drawText(QRect(680,341,100,18), Qt::AlignLeft, "consumo frio");
    p.drawText(QRect(1005,341,110,18), Qt::AlignLeft, "consumo quente");
}
