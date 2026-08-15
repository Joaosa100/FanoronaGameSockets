#include "capturechoicedialog.h"

#include <QCloseEvent>
#include <QKeyEvent>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

namespace {

QString labelFor(const MoveOption &option)
{
    switch (option.type) {
    case CaptureType::None:
        return QObject::tr("Movimento simples (sem captura)");
    case CaptureType::Approach:
        return QObject::tr("Captura por aproximação (%1 peça(s))").arg(option.captured.size());
    case CaptureType::Withdrawal:
        return QObject::tr("Captura por afastamento (%1 peça(s))").arg(option.captured.size());
    }
    return {};
}

} // namespace

CaptureChoiceDialog::CaptureChoiceDialog(const std::vector<MoveOption> &options, QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Escolha o movimento"));
    setWindowFlags((windowFlags() | Qt::CustomizeWindowHint) & ~Qt::WindowCloseButtonHint);
    setModal(true);

    auto *layout = new QVBoxLayout(this);
    layout->addWidget(new QLabel(tr("Mais de um movimento é possível. Escolha um:"), this));

    for (const auto &option : options) {
        auto *button = new QPushButton(labelFor(option), this);
        connect(button, &QPushButton::clicked, this, [this, option] {
            m_chosen = option;
            accept();
        });
        layout->addWidget(button);
    }
}

void CaptureChoiceDialog::closeEvent(QCloseEvent *event)
{
    // Forca o jogador a escolher uma das opcoes clicando em um botao.
    event->ignore();
}

void CaptureChoiceDialog::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) {
        event->ignore();
        return;
    }
    QDialog::keyPressEvent(event);
}

MoveOption CaptureChoiceDialog::choose(QWidget *parent, const std::vector<MoveOption> &options)
{
    CaptureChoiceDialog dialog(options, parent);
    dialog.exec();
    return dialog.m_chosen.value_or(options.front());
}
