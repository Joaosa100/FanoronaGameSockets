#ifndef BOARDWIDGET_H
#define BOARDWIDGET_H

#include <optional>
#include <vector>

#include <QPointF>
#include <QWidget>

#include "board.h"

class BoardWidget : public QWidget {
    Q_OBJECT

public:
    explicit BoardWidget(QWidget *parent = nullptr);

    void setBoard(const Board &board);
    void setHighlighted(const std::vector<int> &destinations);
    void setSelected(std::optional<int> pointIndex);
    // Gira a exibicao em 180 graus, para que o jogador local sempre veja suas
    // proprias pecas na parte de baixo do tabuleiro.
    void setFlipped(bool flipped);

    QSize sizeHint() const override;

signals:
    void pointClicked(int index);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    QPointF pixelFor(int index) const;
    std::optional<int> indexAt(const QPointF &pos) const;
    qreal pointRadius() const;

    Board m_board;
    std::vector<int> m_highlighted;
    std::optional<int> m_selected;
    bool m_flipped = false;
};

#endif // BOARDWIDGET_H
