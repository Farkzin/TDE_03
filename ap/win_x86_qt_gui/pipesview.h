// Declara o componente visual responsável pela representação dos tubos e fluxos

#ifndef PIPESVIEW_H
#define PIPESVIEW_H

#include <QWidget>

class PipesView : public QWidget
{
    Q_OBJECT
public:
    explicit PipesView(QWidget *parent = nullptr);

    void setV1(bool open);
    void setB1(bool on);
    void setV2(double position, bool flowing);
    void setDrain2(bool flowing);
    void setDrain3(bool flowing);

protected:
    void paintEvent(QPaintEvent *) override;

private:
    bool v1 = false;
    bool b1 = false;
    double v2_pos = 0.0;
    bool v2_flowing = false;
    bool drain2 = false;
    bool drain3 = false;
};

#endif
