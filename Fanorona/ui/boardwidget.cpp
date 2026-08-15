#include "boardwidget.h"

#include <algorithm>
#include <cmath>

#include <QMouseEvent>
#include <QPainter>

#include "boardgeometry.h"
#include "moverules.h"

namespace {
constexpr int kMargin = 30;
}

BoardWidget::BoardWidget(QWidget *parent)
    : QWidget(parent)
{
    setMinimumSize(500, 320);
}

void BoardWidget::setBoard(const Board &board)
{
    m_board = board;
    update();
}

void BoardWidget::setHighlighted(const std::vector<int> &destinations)
{
    m_highlighted = destinations;
    update();
}

void BoardWidget::setSelected(std::optional<int> pointIndex)
{
    m_selected = pointIndex;
    update();
}

QSize BoardWidget::sizeHint() const
{
    return QSize(700, 420);
}

QPointF BoardWidget::pixelFor(int index) const
{
    const RowCol rc = BoardGeometry::toRowCol(index);
    const qreal usableWidth = width() - 2 * kMargin;
    const qreal usableHeight = height() - 2 * kMargin;
    const qreal stepX = usableWidth / (BoardGeometry::kCols - 1);
    const qreal stepY = usableHeight / (BoardGeometry::kRows - 1);
    return QPointF(kMargin + rc.col * stepX, kMargin + rc.row * stepY);
}

qreal BoardWidget::pointRadius() const
{
    const qreal usableWidth = width() - 2 * kMargin;
    const qreal stepX = usableWidth / (BoardGeometry::kCols - 1);
    return std::max(6.0, std::min(16.0, stepX * 0.28));
}

std::optional<int> BoardWidget::indexAt(const QPointF &pos) const
{
    const qreal hitRadius = pointRadius() * 1.8;
    std::optional<int> best;
    qreal bestDist = hitRadius;
    for (int i = 0; i < BoardGeometry::kPointCount; ++i) {
        const QPointF p = pixelFor(i);
        const qreal dist = std::hypot(p.x() - pos.x(), p.y() - pos.y());
        if (dist <= bestDist) {
            bestDist = dist;
            best = i;
        }
    }
    return best;
}

void BoardWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(rect(), QColor("#e8dcc4"));

    // Linhas de conexao (horizontais, verticais e diagonais alternadas).
    painter.setPen(QPen(QColor("#7a6a4f"), 1.5));
    for (int i = 0; i < BoardGeometry::kPointCount; ++i) {
        const QPointF from = pixelFor(i);
        for (const auto &[neighbor, dir] : BoardGeometry::neighborsOf(i)) {
            if (neighbor < i)
                continue; // desenha cada aresta uma unica vez
            painter.drawLine(from, pixelFor(neighbor));
        }
    }

    const qreal r = pointRadius();

    // Destaques de destinos validos.
    for (int dest : m_highlighted) {
        const QPointF p = pixelFor(dest);
        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(80, 160, 90, 140));
        painter.drawEllipse(p, r * 1.4, r * 1.4);
    }

    // Selecao atual.
    if (m_selected) {
        const QPointF p = pixelFor(*m_selected);
        painter.setPen(QPen(QColor("#2f6fb0"), 3));
        painter.setBrush(Qt::NoBrush);
        painter.drawEllipse(p, r * 1.6, r * 1.6);
    }

    // Pecas.
    for (int i = 0; i < BoardGeometry::kPointCount; ++i) {
        const Player p = m_board.at(i);
        const QPointF center = pixelFor(i);
        if (p == Player::None) {
            painter.setPen(Qt::NoPen);
            painter.setBrush(QColor("#7a6a4f"));
            painter.drawEllipse(center, r * 0.25, r * 0.25);
            continue;
        }
        painter.setPen(QPen(QColor("#3a3226"), 1.5));
        painter.setBrush(p == Player::PlayerA ? QColor("#f5f0e6") : QColor("#2b2620"));
        painter.drawEllipse(center, r, r);
    }
}

void BoardWidget::mousePressEvent(QMouseEvent *event)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    const QPointF pos = event->position();
#else
    const QPointF pos = event->pos();
#endif
    if (auto index = indexAt(pos))
        emit pointClicked(*index);
}
